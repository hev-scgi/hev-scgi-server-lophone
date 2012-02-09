/*
 ============================================================================
 Name        : hev-modem.c
 Author      : Heiher <admin@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2011 everyone.
 Description : 
 ============================================================================
 */

#include "hev-modem.h"
#include "hev-modem-cdma-phone.h"

static void hev_modem_cdma_phone_new_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data);

#define HEV_MODEM_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE((obj), HEV_TYPE_MODEM, HevModemPrivate))

typedef struct _HevModemPrivate HevModemPrivate;

struct _HevModemPrivate
{
	GObject *cdma_phone;
};

G_DEFINE_TYPE(HevModem, hev_modem, G_TYPE_DBUS_PROXY);

static void hev_modem_dispose(GObject * obj)
{
	HevModem * self = HEV_MODEM(obj);
	HevModemPrivate * priv = HEV_MODEM_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	if(priv->cdma_phone)
	{
		g_object_unref(priv->cdma_phone);
		priv->cdma_phone = NULL;
	}

	G_OBJECT_CLASS(hev_modem_parent_class)->dispose(obj);
}

static void hev_modem_finalize(GObject * obj)
{
	HevModem * self = HEV_MODEM(obj);
	HevModemPrivate * priv = HEV_MODEM_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	G_OBJECT_CLASS(hev_modem_parent_class)->finalize(obj);
}

static GObject * hev_modem_constructor(GType type, guint n, GObjectConstructParam * param)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	return G_OBJECT_CLASS(hev_modem_parent_class)->constructor(type, n, param);
}

static void hev_modem_constructed(GObject * obj)
{
	HevModem *self = HEV_MODEM(obj);
	HevModemPrivate * priv = HEV_MODEM_GET_PRIVATE(self);
	GDBusConnection *connection = NULL;
	gchar *obj_path = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	G_OBJECT_CLASS(hev_modem_parent_class)->constructed(obj);

	g_object_get(G_OBJECT(self), "g-connection",
				&connection, "g-object-path",
				&obj_path, NULL);
	hev_modem_cdma_phone_new(connection,
				obj_path, NULL,
				hev_modem_cdma_phone_new_async_handler,
				self);
	g_object_unref(G_OBJECT(connection));
	g_free(obj_path);
}

static void hev_modem_class_init(HevModemClass * klass)
{
	GObjectClass * obj_class = G_OBJECT_CLASS(klass);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	obj_class->constructor = hev_modem_constructor;
	obj_class->constructed = hev_modem_constructed;
	obj_class->dispose = hev_modem_dispose;
	obj_class->finalize = hev_modem_finalize;

	g_type_class_add_private(klass, sizeof(HevModemPrivate));
}

static void hev_modem_init(HevModem * self)
{
	HevModemPrivate * priv = HEV_MODEM_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
}

void hev_modem_new(GDBusConnection *connection, const gchar *path,
			GCancellable *cancellable, GAsyncReadyCallback callback,
			gpointer user_data)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_return_if_fail(G_IS_DBUS_CONNECTION(connection));
	g_return_if_fail(path);
	g_async_initable_new_async(HEV_TYPE_MODEM,
                              G_PRIORITY_DEFAULT,
                              cancellable,
                              callback,
                              user_data,
                              "g-flags", G_DBUS_PROXY_FLAGS_NONE,
                              "g-interface-info", NULL,
                              "g-name", "org.freedesktop.ModemManager",
                              "g-connection", connection,
                              "g-object-path", path,
                              "g-interface-name",
							  "org.freedesktop.ModemManager.Modem",
                              NULL);
}

GObject * hev_modem_new_finish(GAsyncResult *res, GError **error)
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

void hev_modem_enable(HevModem *self, GCancellable *cancellable,
			GAsyncReadyCallback callback, gpointer user_data)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_dbus_proxy_call(G_DBUS_PROXY(self), "Enable",
				g_variant_new("(b)", TRUE),
				G_DBUS_CALL_FLAGS_NONE,	5000,
				cancellable, callback, user_data);
}

void hev_modem_enable_finish(HevModem *self, GAsyncResult *res,
			GError **error)
{
	GVariant *retval = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	retval = g_dbus_proxy_call_finish(G_DBUS_PROXY(self),
				res, error);
	g_variant_unref(retval);
}

void hev_modem_disable(HevModem *self, GCancellable *cancellable,
			GAsyncReadyCallback callback, gpointer user_data)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_dbus_proxy_call(G_DBUS_PROXY(self), "Enable",
				g_variant_new("(b)", FALSE),
				G_DBUS_CALL_FLAGS_NONE,	5000,
				cancellable, callback, user_data);
}

void hev_modem_disable_finish(HevModem *self, GAsyncResult *res,
			GError **error)
{
	GVariant *retval = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	retval = g_dbus_proxy_call_finish(G_DBUS_PROXY(self),
				res, error);
	g_variant_unref(retval);
}

gboolean hev_modem_is_enabled(HevModem *self)
{
	HevModemPrivate *priv = NULL;
	GVariant *enabled = NULL;
	gboolean retval = FALSE;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_return_val_if_fail(HEV_IS_MODEM(self), FALSE);
	priv = HEV_MODEM_GET_PRIVATE(self);

	enabled = g_dbus_proxy_get_cached_property(G_DBUS_PROXY(self),
				"Enabled");
	g_return_val_if_fail(enabled, FALSE);
	retval = g_variant_get_boolean(enabled);
	g_variant_unref(enabled);

	return retval;
}

GObject * hev_modem_get_cdma_phone(HevModem *self)
{
	HevModemPrivate *priv = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_return_val_if_fail(HEV_IS_MODEM(self), NULL);
	priv = HEV_MODEM_GET_PRIVATE(self);

	return priv->cdma_phone;
}

static void hev_modem_cdma_phone_new_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data)
{
	HevModem *self = HEV_MODEM(user_data);
	HevModemPrivate *priv = HEV_MODEM_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	
	priv->cdma_phone = hev_modem_cdma_phone_new_finish(res, NULL);
}

void hev_modem_get_info(HevModem *self, GCancellable *cancellable,
			GAsyncReadyCallback callback, gpointer user_data)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_dbus_proxy_call(G_DBUS_PROXY(self), "GetInfo", NULL,
				G_DBUS_CALL_FLAGS_NONE,	5000,
				cancellable, callback, user_data);
}

GVariant * hev_modem_get_info_finish(HevModem *self, GAsyncResult *res,
			GError **error)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	return g_dbus_proxy_call_finish(G_DBUS_PROXY(self),
				res, error);
}

