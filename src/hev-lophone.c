/*
 ============================================================================
 Name        : hev-lophone.c
 Author      : Heiher <admin@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2011 everyone.
 Description : 
 ============================================================================
 */

#include <gio/gio.h>

#include "hev-lophone.h"
#include "hev-modem-manager.h"
#include "hev-modem.h"

static void hev_lophone_dbus_bus_get_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data);
static void hev_lophone_dbus_connection_close_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data);
static void hev_modem_manager_new_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data);
static void hev_modem_manager_call_enumerate_devices_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data);
static void hev_modem_new_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data);
static void hev_modem_enable_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data);
static void hev_modem_manager_gsignal_handler(GDBusProxy *proxy,
			gchar *sender_name, gchar *signal_name, GVariant *parameters, gpointer user_data);

#define HEV_LOPHONE_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE((obj), HEV_TYPE_LOPHONE, HevLOPhonePrivate))

typedef struct _HevLOPhonePrivate HevLOPhonePrivate;

struct _HevLOPhonePrivate
{
	GDBusConnection *connection;
	GObject *modem_manager;
	GHashTable *modem_hash_table;
};

G_DEFINE_TYPE(HevLOPhone, hev_lophone, G_TYPE_OBJECT);

static void hev_lophone_dispose(GObject * obj)
{
	HevLOPhone * self = HEV_LOPHONE(obj);
	HevLOPhonePrivate * priv = HEV_LOPHONE_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	if(priv->modem_hash_table)
	{
		g_hash_table_destroy(priv->modem_hash_table);
		priv->modem_hash_table = NULL;
	}

	if(priv->modem_manager)
	{
		g_object_unref(G_OBJECT(priv->modem_manager));
		priv->modem_manager = NULL;
	}

	if(priv->connection)
	{
		g_dbus_connection_close(priv->connection, NULL,
					hev_lophone_dbus_connection_close_async_handler,
					NULL);
	}

	G_OBJECT_CLASS(hev_lophone_parent_class)->dispose(obj);
}

static void hev_lophone_finalize(GObject * obj)
{
	HevLOPhone * self = HEV_LOPHONE(obj);
	HevLOPhonePrivate * priv = HEV_LOPHONE_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	G_OBJECT_CLASS(hev_lophone_parent_class)->finalize(obj);
}

static GObject * hev_lophone_constructor(GType type, guint n, GObjectConstructParam * param)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	return G_OBJECT_CLASS(hev_lophone_parent_class)->constructor(type, n, param);
}

static void hev_lophone_constructed(GObject * obj)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
}

static void hev_lophone_class_init(HevLOPhoneClass * klass)
{
	GObjectClass * obj_class = G_OBJECT_CLASS(klass);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	obj_class->constructor = hev_lophone_constructor;
	obj_class->constructed = hev_lophone_constructed;
	obj_class->dispose = hev_lophone_dispose;
	obj_class->finalize = hev_lophone_finalize;

	g_type_class_add_private(klass, sizeof(HevLOPhonePrivate));
}

static void hev_lophone_init(HevLOPhone * self)
{
	HevLOPhonePrivate * priv = HEV_LOPHONE_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	/* DBus Connection */
	g_bus_get(G_BUS_TYPE_SYSTEM, NULL,
				hev_lophone_dbus_bus_get_async_handler,
				self);

	/* Modem Proxy Hash Table */
	priv->modem_hash_table = g_hash_table_new_full(g_str_hash,
				g_str_equal, g_free, g_object_unref);
}

GObject * hev_lophone_new(void)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	return g_object_new(HEV_TYPE_LOPHONE, NULL);
}

static void hev_lophone_dbus_bus_get_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data)
{
	HevLOPhone *self = HEV_LOPHONE(user_data);
	HevLOPhonePrivate * priv = HEV_LOPHONE_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	priv->connection = g_bus_get_finish(res, NULL);
	g_return_if_fail(G_IS_DBUS_CONNECTION(priv->connection));

	/* ModemManager DBus Proxy */
	hev_modem_manager_new(priv->connection, NULL,
				hev_modem_manager_new_async_handler,
				self);
}

static void hev_lophone_dbus_connection_close_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_dbus_connection_close_finish(G_DBUS_CONNECTION(source_object),
				res, NULL);
}

static void hev_modem_manager_new_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data)
{
	HevLOPhone *self = HEV_LOPHONE(user_data);
	HevLOPhonePrivate * priv = HEV_LOPHONE_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	priv->modem_manager = hev_modem_manager_new_finish(res, NULL);
	g_return_if_fail(HEV_IS_MODEM_MANAGER(priv->modem_manager));

	/* Connect signal 'g-signal' */
	g_signal_connect(G_OBJECT(priv->modem_manager), "g-signal",
				G_CALLBACK(hev_modem_manager_gsignal_handler),
				self);

	/* Call Method 'EnumerateDevices' */
	hev_modem_manager_enumerate_devices(HEV_MODEM_MANAGER(priv->modem_manager), NULL,
				hev_modem_manager_call_enumerate_devices_async_handler,
				self);
}

static void hev_modem_manager_call_enumerate_devices_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data)
{
	HevLOPhone *self = HEV_LOPHONE(user_data);
	HevLOPhonePrivate * priv = HEV_LOPHONE_GET_PRIVATE(self);
	GVariant *retval = NULL, *devices = NULL;
	gsize i = 0, num = 0;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	retval = hev_modem_manager_enumerate_devices_finish(
				HEV_MODEM_MANAGER(priv->modem_manager),
				res, NULL);
	g_return_if_fail(retval);

	devices = g_variant_get_child_value(retval, 0);
	num = g_variant_n_children(devices);
	for(i=0; i<num; i++)
	{
		const gchar *obj_path = NULL;

		obj_path = g_variant_get_string(
					g_variant_get_child_value(devices, i),
					NULL);
		hev_modem_new(priv->connection, obj_path,
					NULL, hev_modem_new_async_handler,
					self);
	}

	g_variant_unref(retval);
}

static void hev_modem_new_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data)
{
	HevLOPhone *self = HEV_LOPHONE(user_data);
	HevLOPhonePrivate * priv = HEV_LOPHONE_GET_PRIVATE(self);
	GObject *modem = NULL;
	gchar *obj_path = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	modem = hev_modem_new_finish(res, NULL);
	g_return_if_fail(HEV_IS_MODEM(modem));

	/* Enable modem */
	if(!hev_modem_is_enabled(HEV_MODEM(modem)))
	{
		hev_modem_enable(HEV_MODEM(modem), NULL,
					hev_modem_enable_async_handler, self);
	}

	g_object_get(G_OBJECT(modem),
				"g-object-path", &obj_path, NULL);
	g_hash_table_insert(priv->modem_hash_table, obj_path,
				modem);
}

static void hev_modem_enable_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data)
{
	HevLOPhone *self = HEV_LOPHONE(user_data);
	HevLOPhonePrivate * priv = HEV_LOPHONE_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	hev_modem_enable_finish(HEV_MODEM(source_object), res, NULL);
}

static void hev_modem_manager_gsignal_handler(GDBusProxy *proxy,
			gchar *sender_name, gchar *signal_name, GVariant *parameters, gpointer user_data)
{
	HevLOPhone *self = HEV_LOPHONE(user_data);
	HevLOPhonePrivate * priv = HEV_LOPHONE_GET_PRIVATE(self);
	GVariant *device = NULL;
	const gchar *obj_path = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	device = g_variant_get_child_value(parameters, 0);
	obj_path = g_variant_get_string(device, NULL);

	if(0 == g_strcmp0(signal_name, "DeviceAdded"))
	{
		hev_modem_new(priv->connection, obj_path,
					NULL, hev_modem_new_async_handler,
					self);
	}
	else if(0 == g_strcmp0(signal_name, "DeviceRemoved"))
	{
		g_hash_table_remove(priv->modem_hash_table, obj_path);
	}
}

GList * hev_lophone_get_modems(HevLOPhone *self)
{
	HevLOPhonePrivate * priv = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	
	g_return_val_if_fail(HEV_IS_LOPHONE(self), NULL);
	priv = HEV_LOPHONE_GET_PRIVATE(self);

	return g_hash_table_get_keys(priv->modem_hash_table);
}

GObject * hev_lophone_lookup_modem(HevLOPhone *self,
			const gchar *path)
{
	HevLOPhonePrivate * priv = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	
	g_return_val_if_fail(HEV_IS_LOPHONE(self), NULL);
	priv = HEV_LOPHONE_GET_PRIVATE(self);

	return g_hash_table_lookup(priv->modem_hash_table,
				path);
}

