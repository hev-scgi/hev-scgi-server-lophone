/*
 ============================================================================
 Name        : hev-modem-cdma-phone.c
 Author      : Heiher <admin@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2011 everyone.
 Description : 
 ============================================================================
 */

#include "hev-modem-cdma-phone.h"

#define HEV_MODEM_CDMA_PHONE_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE((obj), HEV_TYPE_MODEM_CDMA_PHONE, HevModemCdmaPhonePrivate))

typedef struct _HevModemCdmaPhonePrivate HevModemCdmaPhonePrivate;

struct _HevModemCdmaPhonePrivate
{
	gchar c;
};

G_DEFINE_TYPE(HevModemCdmaPhone, hev_modem_cdma_phone, G_TYPE_DBUS_PROXY);

static void hev_modem_cdma_phone_dispose(GObject * obj)
{
	HevModemCdmaPhone * self = HEV_MODEM_CDMA_PHONE(obj);
	HevModemCdmaPhonePrivate * priv = HEV_MODEM_CDMA_PHONE_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	G_OBJECT_CLASS(hev_modem_cdma_phone_parent_class)->dispose(obj);
}

static void hev_modem_cdma_phone_finalize(GObject * obj)
{
	HevModemCdmaPhone * self = HEV_MODEM_CDMA_PHONE(obj);
	HevModemCdmaPhonePrivate * priv = HEV_MODEM_CDMA_PHONE_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	G_OBJECT_CLASS(hev_modem_cdma_phone_parent_class)->finalize(obj);
}

static GObject * hev_modem_cdma_phone_constructor(GType type, guint n, GObjectConstructParam * param)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	return G_OBJECT_CLASS(hev_modem_cdma_phone_parent_class)->constructor(type, n, param);
}

static void hev_modem_cdma_phone_constructed(GObject * obj)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
}

static void hev_modem_cdma_phone_class_init(HevModemCdmaPhoneClass * klass)
{
	GObjectClass * obj_class = G_OBJECT_CLASS(klass);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	obj_class->constructor = hev_modem_cdma_phone_constructor;
	obj_class->constructed = hev_modem_cdma_phone_constructed;
	obj_class->dispose = hev_modem_cdma_phone_dispose;
	obj_class->finalize = hev_modem_cdma_phone_finalize;

	g_type_class_add_private(klass, sizeof(HevModemCdmaPhonePrivate));
}

static void hev_modem_cdma_phone_init(HevModemCdmaPhone * self)
{
	HevModemCdmaPhonePrivate * priv = HEV_MODEM_CDMA_PHONE_GET_PRIVATE(self);
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
}

void hev_modem_cdma_phone_new(GDBusConnection *connection, const gchar *path,
			GCancellable *cancellable, GAsyncReadyCallback callback,
			gpointer user_data)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_return_if_fail(G_IS_DBUS_CONNECTION(connection));
	g_return_if_fail(path);
	g_async_initable_new_async(HEV_TYPE_MODEM_CDMA_PHONE,
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
							  "org.freedesktop.ModemManager.Modem.Cdma.Phone",
                              NULL);
}

GObject * hev_modem_cdma_phone_new_finish(GAsyncResult *res, GError **error)
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

void hev_modem_cdma_phone_call(HevModemCdmaPhone *self, const gchar *number,
			GCancellable *cancellable, GAsyncReadyCallback callback, gpointer user_data)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_dbus_proxy_call(G_DBUS_PROXY(self), "Call",
				g_variant_new("(s)", number),
				G_DBUS_CALL_FLAGS_NONE,	5000,
				cancellable, callback, user_data);
}

GVariant * hev_modem_cdma_phone_call_finish(HevModemCdmaPhone *self, GAsyncResult *res,
			GError **error)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	return g_dbus_proxy_call_finish(G_DBUS_PROXY(self),
				res, error);
}

void hev_modem_cdma_phone_end(HevModemCdmaPhone *self, GCancellable *cancellable,
			GAsyncReadyCallback callback, gpointer user_data)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_dbus_proxy_call(G_DBUS_PROXY(self), "End", NULL,
				G_DBUS_CALL_FLAGS_NONE,	5000,
				cancellable, callback, user_data);
}

GVariant * hev_modem_cdma_phone_end_finish(HevModemCdmaPhone *self, GAsyncResult *res,
			GError **error)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	return g_dbus_proxy_call_finish(G_DBUS_PROXY(self),
				res, error);
}

void hev_modem_cdma_phone_answer(HevModemCdmaPhone *self, GCancellable *cancellable,
			GAsyncReadyCallback callback, gpointer user_data)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_dbus_proxy_call(G_DBUS_PROXY(self), "Answer", NULL,
				G_DBUS_CALL_FLAGS_NONE,	5000,
				cancellable, callback, user_data);
}

GVariant * hev_modem_cdma_phone_answer_finish(HevModemCdmaPhone *self, GAsyncResult *res,
			GError **error)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	return g_dbus_proxy_call_finish(G_DBUS_PROXY(self),
				res, error);
}

void hev_modem_cdma_phone_send_dtmf(HevModemCdmaPhone *self,
			const gchar *id, const gchar *digit,
			GCancellable *cancellable, GAsyncReadyCallback callback, gpointer user_data)
{
	GVariant *value = NULL;
	GVariantBuilder *builder = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	builder = g_variant_builder_new(G_VARIANT_TYPE("a{sv}"));
	g_variant_builder_add(builder, "{sv}",
				"index", g_variant_new_string(id));
	g_variant_builder_add(builder, "{sv}",
				"digit", g_variant_new_string(digit));
	value = g_variant_builder_end(builder);
	value = g_variant_new("(@a{sv})", value);

	g_dbus_proxy_call(G_DBUS_PROXY(self), "SendDTMF",
				value, G_DBUS_CALL_FLAGS_NONE,	5000,
				cancellable, callback, user_data);
}

GVariant * hev_modem_cdma_phone_send_dtmf_finish(HevModemCdmaPhone *self, GAsyncResult *res,
			GError **error)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	return g_dbus_proxy_call_finish(G_DBUS_PROXY(self),
				res, error);
}

void hev_modem_cdma_phone_get_status(HevModemCdmaPhone *self, GCancellable *cancellable,
			GAsyncReadyCallback callback, gpointer user_data)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_dbus_proxy_call(G_DBUS_PROXY(self), "GetStatus", NULL,
				G_DBUS_CALL_FLAGS_NONE,	5000,
				cancellable, callback, user_data);
}

GVariant * hev_modem_cdma_phone_get_status_finish(HevModemCdmaPhone *self, GAsyncResult *res,
			GError **error)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	return g_dbus_proxy_call_finish(G_DBUS_PROXY(self),
				res, error);
}

