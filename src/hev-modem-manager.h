/*
 ============================================================================
 Name        : hev-modem-manager.h
 Author      : Heiher <admin@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2011 everyone.
 Description : 
 ============================================================================
 */

#ifndef __HEV_MODEM_MANAGER_H__
#define __HEV_MODEM_MANAGER_H__

#include <gio/gio.h>

G_BEGIN_DECLS

#define HEV_TYPE_MODEM_MANAGER	(hev_modem_manager_get_type())
#define HEV_MODEM_MANAGER(obj)	(G_TYPE_CHECK_INSTANCE_CAST((obj), HEV_TYPE_MODEM_MANAGER, HevModemManager))
#define HEV_IS_MODEM_MANAGER(obj)	(G_TYPE_CHECK_INSTANCE_TYPE((obj), HEV_TYPE_MODEM_MANAGER))
#define HEV_MODEM_MANAGER_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass), HEV_TYPE_MODEM_MANAGER, HevModemManagerClass))
#define HEV_IS_MODEM_MANAGER_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), HEV_TYPE_MODEM_MANAGER))
#define HEV_MODEM_MANAGER_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), HEV_TYPE_MODEM_MANAGER, HevModemManagerClass))

typedef struct _HevModemManager HevModemManager;
typedef struct _HevModemManagerClass HevModemManagerClass;

struct _HevModemManager
{
	GDBusProxy parent_instance;
};

struct _HevModemManagerClass
{
	GDBusProxyClass parent_class;
};

GType hev_modem_manager_get_type(void);

void hev_modem_manager_new(GDBusConnection *connection,
			GCancellable *cancellable, GAsyncReadyCallback callback,
			gpointer user_data);
GObject * hev_modem_manager_new_finish(GAsyncResult *res, GError **error);
void hev_modem_manager_enumerate_devices(HevModemManager *self,
			GCancellable *cancellable, GAsyncReadyCallback callback,
			gpointer user_data);
GVariant * hev_modem_manager_enumerate_devices_finish(HevModemManager *self,
			GAsyncResult *res, GError **error);

G_END_DECLS

#endif /* __HEV_MODEM_MANAGER_H__ */

