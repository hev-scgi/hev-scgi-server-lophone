/*
 ============================================================================
 Name        : hev-modem-manager.c
 Author      : Heiher <admin@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2011 everyone.
 Description : 
 ============================================================================
 */

#include "hev-modem-manager.h"

#define HEV_MODEM_MANAGER_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE((obj), HEV_TYPE_MODEM_MANAGER, HevModemManagerPrivate))

typedef struct _HevModemManagerPrivate HevModemManagerPrivate;

struct _HevModemManagerPrivate
{
	gchar c;
};

G_DEFINE_TYPE(HevModemManager, hev_modem_manager, G_TYPE_DBUS_PROXY);

static void hev_modem_manager_dispose(GObject * obj)
{
	HevModemManager * self = HEV_MODEM_MANAGER(obj);
	HevModemManagerPrivate * priv = HEV_MODEM_MANAGER_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	G_OBJECT_CLASS(hev_modem_manager_parent_class)->dispose(obj);
}

static void hev_modem_manager_finalize(GObject * obj)
{
	HevModemManager * self = HEV_MODEM_MANAGER(obj);
	HevModemManagerPrivate * priv = HEV_MODEM_MANAGER_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	G_OBJECT_CLASS(hev_modem_manager_parent_class)->finalize(obj);
}

static GObject * hev_modem_manager_constructor(GType type, guint n, GObjectConstructParam * param)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	return G_OBJECT_CLASS(hev_modem_manager_parent_class)->constructor(type, n, param);
}

static void hev_modem_manager_constructed(GObject * obj)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
}

static void hev_modem_manager_class_init(HevModemManagerClass * klass)
{
	GObjectClass * obj_class = G_OBJECT_CLASS(klass);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	obj_class->constructor = hev_modem_manager_constructor;
	obj_class->constructed = hev_modem_manager_constructed;
	obj_class->dispose = hev_modem_manager_dispose;
	obj_class->finalize = hev_modem_manager_finalize;

	g_type_class_add_private(klass, sizeof(HevModemManagerPrivate));
}

static void hev_modem_manager_init(HevModemManager * self)
{
	HevModemManagerPrivate * priv = HEV_MODEM_MANAGER_GET_PRIVATE(self);
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
}

void hev_modem_manager_new(GDBusConnection *connection,
			GCancellable *cancellable, GAsyncReadyCallback callback,
			gpointer user_data)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_return_if_fail(G_IS_DBUS_CONNECTION(connection));
	g_async_initable_new_async(HEV_TYPE_MODEM_MANAGER,
                              G_PRIORITY_DEFAULT,
                              cancellable,
                              callback,
                              user_data,
                              "g-flags", G_DBUS_PROXY_FLAGS_NONE,
                              "g-interface-info", NULL,
                              "g-name", "org.freedesktop.ModemManager",
                              "g-connection", connection,
                              "g-object-path", "/org/freedesktop/ModemManager",
                              "g-interface-name", "org.freedesktop.ModemManager",
                              NULL);
}

GObject * hev_modem_manager_new_finish(GAsyncResult *res, GError **error)
{
	GObject *object;
	GObject *source_object;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	source_object = g_async_result_get_source_object (res);
	g_assert (source_object != NULL);

	object = g_async_initable_new_finish(G_ASYNC_INITABLE(source_object),
				res, error);
	g_object_unref (source_object);

	return object;
}

void hev_modem_manager_enumerate_devices(HevModemManager *self,
			GCancellable *cancellable, GAsyncReadyCallback callback,
			gpointer user_data)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_dbus_proxy_call(G_DBUS_PROXY(self), "EnumerateDevices",
				NULL, G_DBUS_CALL_FLAGS_NONE, 5000, cancellable,
				callback, user_data);
}

GVariant * hev_modem_manager_enumerate_devices_finish(HevModemManager *self,
			GAsyncResult *res, GError **error)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	return g_dbus_proxy_call_finish(G_DBUS_PROXY(self), res, error);
}

