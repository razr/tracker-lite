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

#ifndef __TLITE_SUBSYSTEM_OBJECT_H__
#define __TLITE_SUBSYSTEM_OBJECT_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define TLITE_TYPE_SUBSYSTEM          (tlite_subsystem_get_type ())
#define TLITE_SUBSYSTEM(object)         (G_TYPE_CHECK_INSTANCE_CAST ((object), TLITE_TYPE_SUBSYSTEM, TLiteSubsystem))
#define TLITE_SUBSYSTEM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), TLITE_TYPE_SUBSYSTEM, TLiteSubsystemClass))
#define TLITE_IS_SUBSYSTEM(object)      (G_TYPE_CHECK_INSTANCE_TYPE ((object), TLITE_TYPE_SUBSYSTEM))
#define TLITE_IS_SUBSYSTEM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TLITE_TYPE_SUBSYSTEM))
#define TLITE_SUBSYSTEM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), TLITE_TYPE_SUBSYSTEM, TLiteSubsystemClass))

typedef struct TLiteSubsystem   		TLiteSubsystem;
typedef struct TLiteSubsystemClass		TLiteSubsystemClass;
typedef struct TLiteSubsystemPrivate	TLiteSubsystemPrivate;

struct TLiteSubsystem {
	GObject parent;
	TLiteSubsystemPrivate *priv;
};

struct TLiteSubsystemClass {
	GObjectClass parent;

	void     (* device_added)	  (TLiteSubsystem *subsystem);
	void     (* device_removed)   (TLiteSubsystem *subsystem);
};

GType           tlite_subsystem_get_type     (void);
TLiteSubsystem *tlite_subsystem_new          (void);

G_END_DECLS

#endif /* __TLITE_SUBSYSTEM_OBJECT_H__ */
