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
#include <sqlite3.h>

#include "tlite-store-object.h"
#include "ce-device.h"
#include "tlite-miner-object.h"
#include "tlite-metadata-info.h"

#define TLITE_STORE_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TLITE_TYPE_STORE, TLiteStorePrivate))

static GQuark store_error_quark = 0;

struct _TLiteStorePrivate {
	gchar *name;

	TLiteCeDevice *device;

	gint stored_files;
	gint minered_files;

	GThreadPool *thread_pool;
};

enum {
	PROP_0,
	PROP_NAME,
	PROP_STATUS,
	PROP_PROGRESS
};

enum {
	STARTED,
	PROGRESS,
	FINISHED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

static void       store_finalize               (GObject                *object);
static void       store_initable_iface_init    (GInitableIface         *iface);
static gboolean   store_initable_init          (GInitable              *initable,
                                                GCancellable           *cancellable,
                                                GError                **error);

G_DEFINE_TYPE (TLiteStore, tlite_store, G_TYPE_OBJECT)

static void
tlite_store_class_init (TLiteStoreClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->finalize     = store_finalize;

	signals[STARTED] =
		g_signal_new ("started",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TLiteStoreClass, started),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__VOID,
		              G_TYPE_NONE, 0);


	signals[PROGRESS] =
		g_signal_new ("progress",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TLiteStoreClass, progress),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__INT,
		              G_TYPE_NONE, 1,
		              G_TYPE_INT);

	signals[FINISHED] =
		g_signal_new ("finished",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TLiteStoreClass, finished),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__INT,
		              G_TYPE_NONE, 1,
		              G_TYPE_INT);

	g_type_class_add_private (object_class, sizeof (TLiteStorePrivate));

	store_error_quark = g_quark_from_static_string ("TLiteStore");
}


static void
store_initable_iface_init (GInitableIface *iface)
{
	iface->init = store_initable_init;
}

gboolean
tlite_store_create_db (TLiteCeDevice *device)
{
	int result;
	sqlite3 *db;

	g_return_val_if_fail (TLITE_IS_CE_DEVICE (device), FALSE);

	/* create DB */
	result = sqlite3_open ("aaa.db", &db);
	if (result != SQLITE_OK) {
		g_critical ("Couldn't create database: '%s'", tlite_ce_device_get_name (device));
		return FALSE;
	}

	/* folders */
	sqlite3_exec (db,
	              "CREATE TABLE IF NOT EXISTS folders(folder_id INTEGER PRIMARY KEY AUTOINCREMENT, folder_path VARCHAR(4096), folder_modified_time INTEGER)",
	              NULL, NULL, NULL);
	sqlite3_exec (db,
				  "CREATE INDEX IF NOT EXISTS fk_folders_id ON folders (folder_path ASC);",
	              NULL, NULL, NULL);

	/* files */
	sqlite3_exec (db,
	              "CREATE TABLE IF NOT EXISTS files(file_id INTEGER PRIMARY KEY AUTOINCREMENT,file_path VARCHAR(4096),file_creation_time INTEGER,file_modified_time INTEGER,file_size INTEGER,parent_folder_id INTEGER)",
	              NULL, NULL, NULL);
	sqlite3_exec (db,
				  "CREATE INDEX IF NOT EXISTS fk_files_id ON files (file_path, file_creation_time, file_modified_time ASC);",
	              NULL, NULL, NULL);

	/* titles */
	sqlite3_exec (db,
				  "CREATE TABLE IF NOT EXISTS titles(title_id INTEGER PRIMARY KEY AUTOINCREMENT,file_id INTEGER,title_name VARCHAR(256))",
	              NULL, NULL, NULL);
	sqlite3_exec (db,
				  "CREATE INDEX IF NOT EXISTS fk_titles_id ON titles (file_id ASC);",
	              NULL, NULL, NULL);

	/* albums */
	sqlite3_exec (db,
				  "CREATE TABLE IF NOT EXISTS albums(album_id INTEGER PRIMARY KEY AUTOINCREMENT,file_id INTEGER,album_name VARCHAR(256))",
	              NULL, NULL, NULL);
	sqlite3_exec (db,
				  "CREATE INDEX IF NOT EXISTS fk_albums_id ON albums (file_id ASC);",
	              NULL, NULL, NULL);

	/* artists */
	sqlite3_exec (db,
				  "CREATE TABLE IF NOT EXISTS artists(artist_id INTEGER PRIMARY KEY AUTOINCREMENT,file_id INTEGER,artist_name VARCHAR(256))",
	              NULL, NULL, NULL);
	sqlite3_exec (db,
				  "CREATE INDEX IF NOT EXISTS fk_artists_id ON artists (file_id  ASC);",
	              NULL, NULL, NULL);

	/* genres */
	sqlite3_exec (db,
				  "CREATE TABLE IF NOT EXISTS genres(genre_id INTEGER PRIMARY KEY AUTOINCREMENT,file_id INTEGER,genre_name VARCHAR(256))",
	              NULL, NULL, NULL);

	sqlite3_exec (db,
				  "CREATE INDEX IF NOT EXISTS fk_genres_id ON genres (file_id ASC);",
	              NULL, NULL, NULL);

	/* composers */
	sqlite3_exec (db,
				  "CREATE TABLE IF NOT EXISTS composers(composer_id INTEGER PRIMARY KEY AUTOINCREMENT,file_id INTEGER,composer_name VARCHAR(256))",
	              NULL, NULL, NULL);
	sqlite3_exec (db,
				  "CREATE INDEX IF NOT EXISTS fk_composers_id ON composers (file_id ASC);",
	              NULL, NULL, NULL);

	tlite_ce_device_set_db (device, (gpointer)db);
	return TRUE;
}


gboolean
tlite_store_add_metadata (TLiteCeDevice	*device,
                          TLiteMetadataInfo *info)
{
	sqlite3 *db;
	gint64 rowid;
	gchar **metadata = NULL;
	gchar *path = NULL, str[1024];

	g_return_val_if_fail (TLITE_IS_CE_DEVICE (device), FALSE);

	db = (sqlite3 *) tlite_ce_device_get_db (device);
	/* folders */
/*	sqlite3_exec (db,
				  "INSERT INTO folders(folder_path,folder_modified_time) VALUES ('aaa',1234,)",
	              NULL, NULL, NULL);
*/

	/* files */
	path = tlite_metadata_info_get_path (info);
	g_sprintf (str, "INSERT INTO files(file_path,file_creation_time,file_modified_time,file_size) VALUES ('%s',%d,%d,%d)",
			   path, 1, 1, 1);

	sqlite3_exec (db, str, NULL, NULL, NULL);
	rowid = sqlite3_last_insert_rowid (db);

	metadata = tlite_metadata_info_get_metadata (info);

	/* titles */
	g_sprintf (str, "INSERT INTO titles(file_id,title_name) VALUES (%d,'%s')",
			   rowid, metadata[0]);
	sqlite3_exec (db, str, NULL, NULL, NULL);

	/* albums */
	g_sprintf (str, "INSERT INTO albums(file_id,album_name) VALUES (%d,'%s')",
			   rowid, metadata[1]);
	sqlite3_exec (db, str, NULL, NULL, NULL);

	/* artists */
	g_sprintf (str, "INSERT INTO artists(file_id,artist_name) VALUES (%d,'%s')",
			   rowid, metadata[2]);
	sqlite3_exec (db, str, NULL, NULL, NULL);

	/* genres */
	g_sprintf (str, "INSERT INTO genres(file_id,genre_name) VALUES (%d,'%s')",
			   rowid, metadata[3]);
	sqlite3_exec (db, str, NULL, NULL, NULL);

	/* composers */
	g_sprintf (str, "INSERT INTO composers(file_id,composer_name) VALUES (%d,'%s')",
			   rowid, metadata[4]);
	sqlite3_exec (db, str, NULL, NULL, NULL);

	return TRUE;
}

static void
store_get_metadata_info (GList *infos,
                   TLiteStore *store)
{
	GList   *iter;
	sqlite3 *db;
	g_printf ("%s %d %d %d\n", __FUNCTION__, g_list_length (infos), store->priv->stored_files, store->priv->minered_files);

	db = (sqlite3 *) tlite_ce_device_get_db (store->priv->device);

	sqlite3_exec (db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	for (iter = infos; iter; iter = g_list_next (iter))
	{
		gchar **metadata;
		TLiteMetadataInfo *info = (TLiteMetadataInfo *)iter->data;

		tlite_store_add_metadata (store->priv->device, info);
	}
	store->priv->stored_files += g_list_length (infos);
	g_list_free (infos);
	sqlite3_exec (db, "COMMIT TRANSACTION;", NULL, NULL, NULL);

	if (store->priv->stored_files == store->priv->minered_files) {
		g_signal_emit (store, signals[FINISHED], 0, store->priv->stored_files);
	}
}

static gboolean
store_initable_init (GInitable     *initable,
                     GCancellable  *cancellable,
                     GError       **error)
{
	TLiteStore *store = TLITE_STORE (initable);

	/* Check store has a proper name */
	if (!store->priv->name) {
		g_set_error (error,
		             TLITE_STORE_ERROR,
		             0,
		             "Store '%s' should have been given a name, bailing out",
		             G_OBJECT_TYPE_NAME (store));
		return FALSE;
	}

	return TRUE;
}


static void
tlite_store_init (TLiteStore *store)
{
	store->priv = TLITE_STORE_GET_PRIVATE (store);
	store->priv->thread_pool = g_thread_pool_new ((GFunc) store_get_metadata_info,
	                                   			   store, 10, TRUE, NULL);
	store->priv->stored_files = 0;
	store->priv->minered_files = 0;
}


GQuark
tlite_store_error_quark (void)
{
	return g_quark_from_static_string (TLITE_STORE_ERROR_DOMAIN);
}


static void
store_miner_minered_cb (TLiteMiner *miner,
				        GList *infos,
                        TLiteStore *store)
{
	g_printf ("%s %d\n", __FUNCTION__, g_list_length (infos));
	g_thread_pool_push (store->priv->thread_pool, infos, NULL);
}

static void
store_miner_finished_cb (TLiteMiner *miner,
                         gint minered_files, 
                         TLiteStore *store)
{
	g_printf ("%s %d\n", __FUNCTION__, minered_files);
	store->priv->minered_files = minered_files;
}

void
tlite_store_start (TLiteStore *store,
                   TLiteMiner *miner)
{
	g_return_if_fail (TLITE_IS_STORE (store));
	g_return_if_fail (TLITE_IS_MINER (miner));

	g_signal_connect_object (miner, "minered",
					             G_CALLBACK (store_miner_minered_cb), store, 0);
	g_signal_connect_object (miner, "finished",
	              				 G_CALLBACK (store_miner_finished_cb), store, 0);

	g_signal_emit (store, signals[STARTED], 0);
}

static void
store_finalize (GObject *object)
{
	TLiteStore *store = TLITE_STORE (object);

	g_free (store->priv->name);

	g_object_unref (store->priv->device);
	g_thread_pool_free (store->priv->thread_pool, TRUE, FALSE);
	G_OBJECT_CLASS (tlite_store_parent_class)->finalize (object);
}


TLiteStore *
tlite_store_new (TLiteCeDevice *device)
{
	TLiteStore *store;

	store = g_object_new (TLITE_TYPE_STORE, NULL);

	store->priv->device = g_object_ref (device);
	return store;
}