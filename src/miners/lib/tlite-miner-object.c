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

#include "config.h"

#include <glib.h>

#include <tag_c.h>

#include "tlite-miner-object.h"
#include "tlite-crawler-object.h"

#define TLITE_MINER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TLITE_TYPE_MINER, TLiteMinerPrivate))

static GQuark miner_error_quark = 0;

struct _TLiteMinerPrivate {
	gboolean started;
	gboolean paused;

	gchar *name;
	gchar *status;
	gint progress;
	gint remaining_time;

	GThreadPool *thread_pool;
};

enum {
	PROP_0,
	PROP_NAME,
	PROP_STATUS,
	PROP_PROGRESS,
	PROP_REMAINING_TIME
};

enum {
	STARTED,
	STOPPED,
	PAUSED,
	RESUMED,
	PROGRESS,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

static void       miner_finalize               (GObject                *object);
static void       miner_initable_iface_init    (GInitableIface         *iface);
static gboolean   miner_initable_init          (GInitable              *initable,
                                                GCancellable           *cancellable,
                                                GError                **error);

/* 
G_DEFINE_ABSTRACT_TYPE_WITH_CODE (TLiteMiner, tlite_miner, G_TYPE_OBJECT,
                                  G_IMPLEMENT_INTERFACE (G_TYPE_INITABLE,
                                                         miner_initable_iface_init));
*/

G_DEFINE_TYPE (TLiteMiner, tlite_miner, G_TYPE_OBJECT)

static void
tlite_miner_class_init (TLiteMinerClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->finalize     = miner_finalize;

	signals[STARTED] =
		g_signal_new ("started",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TLiteMinerClass, started),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__VOID,
		              G_TYPE_NONE, 0);

	signals[STOPPED] =
		g_signal_new ("stopped",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TLiteMinerClass, stopped),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__VOID,
		              G_TYPE_NONE, 0);

	signals[PAUSED] =
		g_signal_new ("paused",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TLiteMinerClass, paused),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__VOID,
		              G_TYPE_NONE, 0);
	signals[RESUMED] =
		g_signal_new ("resumed",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TLiteMinerClass, resumed),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__VOID,
		              G_TYPE_NONE, 0);

	signals[PROGRESS] =
		g_signal_new ("progress",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TLiteMinerClass, progress),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__INT,
		              G_TYPE_NONE, 1,
		              G_TYPE_INT);

	g_type_class_add_private (object_class, sizeof (TLiteMinerPrivate));

	miner_error_quark = g_quark_from_static_string ("TLiteMiner");
}


static void
miner_initable_iface_init (GInitableIface *iface)
{
	iface->init = miner_initable_init;
}

static void
get_metadata (GList *files,
              gpointer user_data)
{
	GList   *iter;
	TagLib_File *file;
	TagLib_Tag *tag;

	g_printf ("%s %d\n", __FUNCTION__, g_list_length (files));
	for (iter = files; iter; iter = g_list_next (iter))
	{
		file = taglib_file_new_type (g_file_get_path ((GFile *)iter->data), TagLib_File_MPEG);
		tag = taglib_file_tag (file);
/*		g_printf ("%s %s %s %s\n",
		          taglib_tag_title (tag),
		          taglib_tag_artist (tag),
		          taglib_tag_album (tag),
		          taglib_tag_genre (tag));
*/
		taglib_tag_free_strings ();
		taglib_file_free (file);		
	}
}

static gboolean
miner_initable_init (GInitable     *initable,
                     GCancellable  *cancellable,
                     GError       **error)
{
	TLiteMiner *miner = TLITE_MINER (initable);

	/* Check miner has a proper name */
	if (!miner->priv->name) {
		g_set_error (error,
		             TLITE_MINER_ERROR,
		             0,
		             "Miner '%s' should have been given a name, bailing out",
		             G_OBJECT_TYPE_NAME (miner));
		return FALSE;
	}

	return TRUE;
}


static void
tlite_miner_init (TLiteMiner *miner)
{
	miner->priv = TLITE_MINER_GET_PRIVATE (miner);
	miner->priv->started = FALSE;
	miner->priv->thread_pool = g_thread_pool_new ((GFunc) get_metadata,
	                                   			   miner, 10, TRUE, NULL);
}


GQuark
tlite_miner_error_quark (void)
{
	return g_quark_from_static_string (TLITE_MINER_ERROR_DOMAIN);
}


static void
miner_scanned_cb (TLiteCrawler *crawler,
				  GList *files,
                  TLiteMiner *miner)
{
	GList *iter;

	g_printf ("%s\n", __FUNCTION__);

/*	for (iter = files; iter; iter = g_list_next (iter))
	{
		g_printf ("%s\n", g_file_get_path ((GFile *)iter->data));
	}
*/
	g_thread_pool_push (miner->priv->thread_pool, files, NULL);
}

static void
miner_finished_cb (TLiteCrawler *crawler,
                  TLiteMiner *miner)
{
	g_printf ("%s\n", __FUNCTION__);
}

void
tlite_miner_start (TLiteMiner *miner,
                   TLiteCrawler *crawler)
{
	g_return_if_fail (TLITE_IS_MINER (miner));
	g_return_if_fail (miner->priv->started == FALSE);
	g_return_if_fail (TLITE_IS_CRAWLER (crawler));

	miner->priv->started = TRUE;

	g_signal_connect_object (crawler, "scanned",
					             G_CALLBACK (miner_scanned_cb), miner, 0);
	g_signal_connect_object (crawler, "finished",
	              				 G_CALLBACK (miner_finished_cb), miner, 0);

	g_signal_emit (miner, signals[STARTED], 0);
}


void
tlite_miner_stop (TLiteMiner *miner)
{
	g_return_if_fail (TLITE_IS_MINER (miner));
	g_return_if_fail (miner->priv->started == TRUE);

	miner->priv->started = FALSE;

	g_signal_emit (miner, signals[STOPPED], 0);
}


gboolean
tlite_miner_is_started (TLiteMiner *miner)
{
	g_return_val_if_fail (TLITE_IS_MINER (miner), TRUE);

	return miner->priv->started;
}


gboolean
tlite_miner_is_paused (TLiteMiner *miner)
{
	g_return_val_if_fail (TLITE_IS_MINER (miner), TRUE);

	return miner->priv->paused;
}


gint
tlite_miner_pause (TLiteMiner  *miner,
                   GError       **error)
{
	g_return_val_if_fail (TLITE_IS_MINER (miner), 10);

	g_message ("Miner:'%s' is pausing", miner->priv->name);
	g_signal_emit (miner, signals[PAUSED], 0);

	miner->priv->paused = TRUE;

	return miner->priv->paused;
}


gboolean
tlite_miner_resume (TLiteMiner  *miner,
                    GError       **error)
{
	g_return_val_if_fail (TLITE_IS_MINER (miner), FALSE);

	g_message ("Miner:'%s' is resuming", miner->priv->name);
	g_signal_emit (miner, signals[RESUMED], 0);

	return TRUE;
}


static void
miner_finalize (GObject *object)
{
	TLiteMiner *miner = TLITE_MINER (object);

	g_free (miner->priv->status);
	g_free (miner->priv->name);

	g_thread_pool_free (miner->priv->thread_pool, TRUE, FALSE);
	G_OBJECT_CLASS (tlite_miner_parent_class)->finalize (object);
}


TLiteMiner *
tlite_miner_new (void)
{
	TLiteMiner *miner;

	miner = g_object_new (TLITE_TYPE_MINER, NULL);

	return miner;
}