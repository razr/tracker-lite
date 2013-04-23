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

#ifndef __TLITE_MINER_OBJECT_H__
#define __TLITE_MINER_OBJECT_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define TLITE_TYPE_MINER         (tlite_miner_get_type())
#define TLITE_MINER(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), TLITE_TYPE_MINER, TLiteMiner))
#define TLITE_MINER_CLASS(c)     (G_TYPE_CHECK_CLASS_CAST ((c),    TLITE_TYPE_MINER, TLiteMinerClass))
#define TLITE_IS_MINER(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), TLITE_TYPE_MINER))
#define TLITE_IS_MINER_CLASS(c)  (G_TYPE_CHECK_CLASS_TYPE ((c),    TLITE_TYPE_MINER))
#define TLITE_MINER_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o),  TLITE_TYPE_MINER, TLiteMinerClass))

#define TLITE_MINER_ERROR_DOMAIN "TLiteMiner"
#define TLITE_MINER_ERROR        tlite_miner_error_quark()

typedef struct _TLiteMiner TLiteMiner;
typedef struct _TLiteMinerPrivate TLiteMinerPrivate;

/**
 * TLiteMiner:
 *
 * Abstract miner object.
 **/
struct _TLiteMiner {
	GObject parent_instance;
	TLiteMinerPrivate *priv;
};

/**
 * TLiteMinerClass:
 * @parent_class: parent object class.
 * @started: Called when the miner is told to start collecting data.
 * @stopped: Called when the miner is told to stop collecting data.
 * @paused: Called when the miner is told to pause.
 * @resumed: Called when the miner is told to resume activity.
 * @progress: progress.
 * @ignore_next_update: Called after ignore on next update event happens.
 *
 * Virtual methods left to implement.
 **/
typedef struct {
	GObjectClass parent_class;

	/* signals */
	void (* started)            (TLiteMiner *miner);
	void (* stopped)            (TLiteMiner *miner);

	void (* paused)             (TLiteMiner *miner);
	void (* resumed)            (TLiteMiner *miner);

	void (* progress)           (TLiteMiner *miner,
	                             const gchar   *status,
	                             gint       progress);

} TLiteMinerClass;

GType                    tlite_miner_get_type            (void) G_GNUC_CONST;
GQuark                   tlite_miner_error_quark         (void);

void                     tlite_miner_start               (TLiteMiner         *miner);
void                     tlite_miner_stop                (TLiteMiner         *miner);

gboolean                 tlite_miner_is_started          (TLiteMiner         *miner);
gboolean                 tlite_miner_is_paused           (TLiteMiner         *miner);


gint                     tlite_miner_pause               (TLiteMiner         *miner,
                                                          GError             **error);
gboolean                 tlite_miner_resume              (TLiteMiner         *miner,
                                                          GError              **error);

G_END_DECLS

#endif /* __TLITE_MINER_OBJECT_H__ */
