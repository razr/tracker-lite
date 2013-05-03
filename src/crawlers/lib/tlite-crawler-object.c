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

#include "tlite-crawler-object.h"

#define TLITE_CRAWLER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TLITE_TYPE_CRAWLER, TLiteCrawlerPrivate))

struct TLiteCrawlerPrivate {

	TLiteCeDevice *device;
		
	/* Idle handler for processing found data */
	guint           idle_id;

	/* Statistics */
	gint        scanned_files;
	gint        scanned_dirs;
};

enum {
	FOUND,
	PROCESSED,
	FINISHED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0, };
static GQuark file_info_quark = 0;

G_DEFINE_TYPE (TLiteCrawler, tlite_crawler, G_TYPE_OBJECT)

static void
crawler_finalize (GObject *object)
{
	G_OBJECT_CLASS (tlite_crawler_parent_class)->finalize (object);
}


static void
tlite_crawler_class_init (TLiteCrawlerClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->finalize = crawler_finalize;

	signals[FOUND] =
		g_signal_new ("found",
		              G_TYPE_FROM_CLASS (klass),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TLiteCrawlerClass, found),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__VOID,
		              G_TYPE_NONE, 0);
	signals[PROCESSED] =
		g_signal_new ("processed",
		              G_TYPE_FROM_CLASS (klass),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TLiteCrawlerClass, processed),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__VOID,
		              G_TYPE_NONE, 0);
	signals[FINISHED] =
		g_signal_new ("finished",
		              G_TYPE_FROM_CLASS (klass),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TLiteCrawlerClass, finished),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__VOID,
		              G_TYPE_NONE, 0);

	g_type_class_add_private (object_class, sizeof (TLiteCrawlerPrivate));

	file_info_quark = g_quark_from_static_string ("tlite-crawler-file-info");
}

static void
tlite_crawler_init (TLiteCrawler *crawler)
{
	crawler->priv = TLITE_CRAWLER_GET_PRIVATE (crawler);
	crawler->priv->scanned_files = 0;
	crawler->priv->scanned_dirs = 0;
}


TLiteCrawler *
tlite_crawler_new (void)
{
	TLiteCrawler *crawler;

	crawler = g_object_new (TLITE_TYPE_CRAWLER, NULL);

	return crawler;
}

static gboolean
scan_dir (TLiteCrawler *crawler, GFile *dir)
{
	GFileEnumerator		*enumerator;
	GError				*error = NULL;
	GFileInfo			*info;
	GFileType			type; 
	const char			*name;
	static gboolean     found = FALSE;

	g_printf ("%s\n", g_file_get_path (dir));

	/* TODO: shall be cancellable */
	enumerator =  g_file_enumerate_children (dir,
											 G_FILE_ATTRIBUTE_STANDARD_TYPE,
											 G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
		                                     NULL,
		                                     &error);
	g_object_unref (dir);

	if (error != NULL) {
		g_error ("could not enumerate folder content : %s", error->message );
		g_signal_emit (crawler, signals[FINISHED], 0);
		return TRUE;
	}

	while ((info = g_file_enumerator_next_file (enumerator, NULL, NULL)) != NULL) {

		type = g_file_info_get_file_type (info);
		name = g_file_info_get_name (info);

		switch (type) {
			case G_FILE_TYPE_DIRECTORY:
				/* scan new folder - recurse */
				crawler->priv->scanned_dirs++;
				scan_dir (crawler, g_file_enumerator_get_child (enumerator, info));
				break;

			case G_FILE_TYPE_REGULAR:
				/* add to the list of miner */
				/* TODO: shall be a limit on the amount of files */
				crawler->priv->scanned_files++;
				if (!found) {
					g_signal_emit (crawler, signals[FOUND], 0);
					found = TRUE;
				}
				break;

			default:
				break;
		}
		g_object_unref (info);
	}

	g_file_enumerator_close (enumerator, NULL, &error);
	g_object_unref (enumerator);

	return FALSE;
}

static gboolean
process_func (gpointer data)
{
	TLiteCrawler      	*crawler;
	TLiteCrawlerPrivate	*priv;
	GFile *file;

	g_printf ("%s\n",__FUNCTION__);
	crawler = TLITE_CRAWLER (data);
	priv = TLITE_CRAWLER_GET_PRIVATE (crawler);

	file = tlite_ce_device_get_file (priv->device);
	scan_dir (crawler, file);

	g_signal_emit (crawler, signals[FINISHED], 0);

	g_printf ("dirs = %d files = %d\n",priv->scanned_dirs, priv->scanned_files);
	return FALSE;
}

static gboolean
process_func_start (TLiteCrawler *crawler)
{
	if (crawler->priv->idle_id == 0) {
		crawler->priv->idle_id = g_idle_add (process_func, crawler);
	}

	return TRUE;
}


gboolean
tlite_crawler_start (TLiteCrawler *crawler,
                     TLiteCeDevice *device)
{
	TLiteCrawlerPrivate *priv;

	g_return_val_if_fail (TLITE_IS_CRAWLER (crawler), FALSE);
	g_return_val_if_fail (TLITE_IS_CE_DEVICE (device), FALSE);

	priv = TLITE_CRAWLER_GET_PRIVATE (crawler);
	priv->device = device;

	process_func_start (crawler);

	return TRUE;
}

TLiteCeDevice*
tlite_crawler_get_device (TLiteCrawler *crawler) {
	TLiteCrawlerPrivate *priv;

	g_return_val_if_fail (TLITE_IS_CRAWLER (crawler), FALSE);

	priv = TLITE_CRAWLER_GET_PRIVATE (crawler);
	return priv->device;
}
