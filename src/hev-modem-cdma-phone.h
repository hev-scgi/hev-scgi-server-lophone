/*
 ============================================================================
 Name        : hev-modem-cdma-phone.h
 Author      : Heiher <admin@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2011 everyone.
 Description : 
 ============================================================================
 */

#ifndef __HEV_MODEM_CDMA_PHONE_H__
#define __HEV_MODEM_CDMA_PHONE_H__

#include <gio/gio.h>

G_BEGIN_DECLS

#define HEV_TYPE_MODEM_CDMA_PHONE	(hev_modem_cdma_phone_get_type())
#define HEV_MODEM_CDMA_PHONE(obj)	(G_TYPE_CHECK_INSTANCE_CAST((obj), HEV_TYPE_MODEM_CDMA_PHONE, HevModemCdmaPhone))
#define HEV_IS_MODEM_CDMA_PHONE(obj)	(G_TYPE_CHECK_INSTANCE_TYPE((obj), HEV_TYPE_MODEM_CDMA_PHONE))
#define HEV_MODEM_CDMA_PHONE_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass), HEV_TYPE_MODEM_CDMA_PHONE, HevModemCdmaPhoneClass))
#define HEV_IS_MODEM_CDMA_PHONE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), HEV_TYPE_MODEM_CDMA_PHONE))
#define HEV_MODEM_CDMA_PHONE_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), HEV_TYPE_MODEM_CDMA_PHONE, HevModemCdmaPhoneClass))

typedef struct _HevModemCdmaPhone HevModemCdmaPhone;
typedef struct _HevModemCdmaPhoneClass HevModemCdmaPhoneClass;

struct _HevModemCdmaPhone
{
	GDBusProxy parent_instance;
};

struct _HevModemCdmaPhoneClass
{
	GDBusProxyClass parent_class;
};

GType hev_modem_cdma_phone_get_type(void);

void hev_modem_cdma_phone_new(GDBusConnection *connection, const gchar *path,
			GCancellable *cancellable, GAsyncReadyCallback callback,
			gpointer user_data);
GObject * hev_modem_cdma_phone_new_finish(GAsyncResult *res, GError **error);
void hev_modem_cdma_phone_call(HevModemCdmaPhone *self, const gchar *number,
			GCancellable *cancellable, GAsyncReadyCallback callback, gpointer user_data);
GVariant * hev_modem_cdma_phone_call_finish(HevModemCdmaPhone *self, GAsyncResult *res,
			GError **error);
void hev_modem_cdma_phone_end(HevModemCdmaPhone *self, GCancellable *cancellable,
			GAsyncReadyCallback callback, gpointer user_data);
GVariant * hev_modem_cdma_phone_end_finish(HevModemCdmaPhone *self, GAsyncResult *res,
			GError **error);
void hev_modem_cdma_phone_answer(HevModemCdmaPhone *self, GCancellable *cancellable,
			GAsyncReadyCallback callback, gpointer user_data);
GVariant * hev_modem_cdma_phone_answer_finish(HevModemCdmaPhone *self, GAsyncResult *res,
			GError **error);
void hev_modem_cdma_phone_send_dtmf(HevModemCdmaPhone *self,
			const gchar *id, const gchar *digit,
			GCancellable *cancellable, GAsyncReadyCallback callback, gpointer user_data);
GVariant * hev_modem_cdma_phone_send_dtmf_finish(HevModemCdmaPhone *self, GAsyncResult *res,
			GError **error);
void hev_modem_cdma_phone_get_status(HevModemCdmaPhone *self, GCancellable *cancellable,
			GAsyncReadyCallback callback, gpointer user_data);
GVariant * hev_modem_cdma_phone_get_status_finish(HevModemCdmaPhone *self, GAsyncResult *res,
			GError **error);

G_END_DECLS

#endif /* __HEV_MODEM_CDMA_PHONE_H__ */

