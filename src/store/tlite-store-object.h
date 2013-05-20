/*
 * Copyright (C) 2013, Wind River Systems
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

#ifndef __TLITE_STORE_OBJECT_H__
#define __TLITE_STORE_OBJECT_H__

#include <glib-object.h>

#include "tlite-miner-object.h"
#include "ce-device.h"

G_BEGIN_DECLS

#define TLITE_TYPE_STORE         (tlite_store_get_type())
#define TLITE_STORE(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), TLITE_TYPE_STORE, TLiteStore))
#define TLITE_STORE_CLASS(c)     (G_TYPE_CHECK_CLASS_CAST ((c),    TLITE_TYPE_STORE, TLiteStoreClass))
#define TLITE_IS_STORE(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), TLITE_TYPE_STORE))
#define TLITE_IS_STORE_CLASS(c)  (G_TYPE_CHECK_CLASS_TYPE ((c),    TLITE_TYPE_STORE))
#define TLITE_STORE_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o),  TLITE_TYPE_STORE, TLiteStoreClass))

#define TLITE_STORE_ERROR_DOMAIN "TLiteStore"
#define TLITE_STORE_ERROR        tlite_store_error_quark()

typedef struct _TLiteStore TLiteStore;
typedef struct _TLiteStorePrivate TLiteStorePrivate;

struct _TLiteStore {
	GObject parent_instance;

	TLiteStorePrivate *priv;
};

typedef struct {
	GObjectClass parent_class;

	/* signals */
	void (* started)            (TLiteStore *store);
	void (* progress)           (TLiteStore *store,
	                             gint       progress);
	void (* finished)           (TLiteStore *store,
	                             gint       stored_files);

} TLiteStoreClass;

GType                    tlite_store_get_type            (void) G_GNUC_CONST;
GQuark                   tlite_store_error_quark         (void);

TLiteStore				*tlite_store_new      			 (TLiteCeDevice		 *device);

void                     tlite_store_start               (TLiteStore         *store,
                                                          TLiteMiner		 *miner);
gboolean				 tlite_store_create_db		     (TLiteCeDevice      *device);
gboolean                 tlite_store_add_metadata        (TLiteCeDevice	     *device,
                                                          TLiteMetadataInfo  *info);

G_END_DECLS

#endif /* __TLITE_STORE_OBJECT_H__ */
