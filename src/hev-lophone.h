/*
 ============================================================================
 Name        : hev-lophone.h
 Author      : Heiher <admin@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2011 everyone.
 Description : 
 ============================================================================
 */

#ifndef __HEV_LOPHONE_H__
#define __HEV_LOPHONE_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define HEV_TYPE_LOPHONE	(hev_lophone_get_type())
#define HEV_LOPHONE(obj)	(G_TYPE_CHECK_INSTANCE_CAST((obj), HEV_TYPE_LOPHONE, HevLOPhone))
#define HEV_IS_LOPHONE(obj)	(G_TYPE_CHECK_INSTANCE_TYPE((obj), HEV_TYPE_LOPHONE))
#define HEV_LOPHONE_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass), HEV_TYPE_LOPHONE, HevLOPhoneClass))
#define HEV_IS_LOPHONE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), HEV_TYPE_LOPHONE))
#define HEV_LOPHONE_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), HEV_TYPE_LOPHONE, HevLOPhoneClass))

typedef struct _HevLOPhone HevLOPhone;
typedef struct _HevLOPhoneClass HevLOPhoneClass;

struct _HevLOPhone
{
	GObject parent_instance;
};

struct _HevLOPhoneClass
{
	GObjectClass parent_class;
};

GType hev_lophone_get_type(void);

GObject * hev_lophone_new(void);
GList * hev_lophone_get_modems(HevLOPhone *self);
GObject * hev_lophone_lookup_modem(HevLOPhone *self,
			const gchar *path);

G_END_DECLS

#endif /* __HEV_LOPHONE_H__ */

