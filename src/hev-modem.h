/*
 ============================================================================
 Name        : hev-modem.h
 Author      : Heiher <admin@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2011 everyone.
 Description : 
 ============================================================================
 */

#ifndef __HEV_MODEM_H__
#define __HEV_MODEM_H__

#include <gio/gio.h>

G_BEGIN_DECLS

#define HEV_TYPE_MODEM	(hev_modem_get_type())
#define HEV_MODEM(obj)	(G_TYPE_CHECK_INSTANCE_CAST((obj), HEV_TYPE_MODEM, HevModem))
#define HEV_IS_MODEM(obj)	(G_TYPE_CHECK_INSTANCE_TYPE((obj), HEV_TYPE_MODEM))
#define HEV_MODEM_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass), HEV_TYPE_MODEM, HevModemClass))
#define HEV_IS_MODEM_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), HEV_TYPE_MODEM))
#define HEV_MODEM_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), HEV_TYPE_MODEM, HevModemClass))

typedef struct _HevModem HevModem;
typedef struct _HevModemClass HevModemClass;

struct _HevModem
{
	GDBusProxy parent_instance;
};

struct _HevModemClass
{
	GDBusProxyClass parent_class;
};

GType hev_modem_get_type(void);

void hev_modem_new(GDBusConnection *connection, const gchar *path,
			GCancellable *cancellable, GAsyncReadyCallback callback,
			gpointer user_data);
GObject * hev_modem_new_finish(GAsyncResult *res, GError **error);
void hev_modem_enable(HevModem *self, GCancellable *cancellable,
			GAsyncReadyCallback callback, gpointer user_data);
void hev_modem_enable_finish(HevModem *self, GAsyncResult *res,
			GError **error);
void hev_modem_disable(HevModem *self, GCancellable *cancellable,
			GAsyncReadyCallback callback, gpointer user_data);
void hev_modem_disable_finish(HevModem *self, GAsyncResult *res,
			GError **error);
gboolean hev_modem_is_enabled(HevModem *self);
GObject * hev_modem_get_cdma_phone(HevModem *self);
void hev_modem_get_info(HevModem *self, GCancellable *cancellable,
			GAsyncReadyCallback callback, gpointer user_data);
GVariant * hev_modem_get_info_finish(HevModem *self, GAsyncResult *res,
			GError **error);

G_END_DECLS

#endif /* __HEV_MODEM_H__ */

