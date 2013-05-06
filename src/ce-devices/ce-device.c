#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gio/gio.h>
#include <sqlite3.h>

#include "ce-device.h"

#define TLITE_CE_DEVICE_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TLITE_TYPE_CE_DEVICE, TLiteCeDevicePrivate))

struct _TLiteCeDevicePrivate {
	GFile	   *file;

	gchar	   *name;
	sqlite3	   *database;
};

static void ce_device_initable_iface_init (GInitableIface *iface);

G_DEFINE_TYPE_WITH_CODE (TLiteCeDevice, tlite_ce_device, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (G_TYPE_INITABLE,
                                                ce_device_initable_iface_init));

enum {
	INDEXED,
	LAST_SIGNAL
};

static guint signals [LAST_SIGNAL] = { 0 };

static void
ce_device_finalize (GObject *object)
{
	TLiteCeDevicePrivate *priv;

	priv = TLITE_CE_DEVICE_GET_PRIVATE (object);

	if (priv->database) {
		sqlite3_close(priv->database);
	}

	G_OBJECT_CLASS (tlite_ce_device_parent_class)->finalize (object);
}

static void
tlite_ce_device_class_init (TLiteCeDeviceClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->finalize = ce_device_finalize;

	signals [INDEXED] =
		g_signal_new ("indexed",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TLiteCeDeviceClass, indexed),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__STRING,
		              G_TYPE_NONE, 1,
		              G_TYPE_STRING);

	g_type_class_add_private (object_class, sizeof (TLiteCeDevicePrivate));
}


static void
tlite_ce_device_init (TLiteCeDevice *manager)
{
	manager->priv = TLITE_CE_DEVICE_GET_PRIVATE (manager);
}


static gboolean
ce_device_initable_init (GInitable     *initable,
                         GCancellable  *cancellable,
                         GError       **error)
{
	TLiteCeDevice *device;
	GError *inner_error = NULL;
	TLiteCeDevicePrivate *priv;
		
	device = TLITE_CE_DEVICE (initable);
	priv = TLITE_CE_DEVICE_GET_PRIVATE (device);

	return TRUE;
}


static void
ce_device_initable_iface_init (GInitableIface *iface)
{
	iface->init = ce_device_initable_init;
}


TLiteCeDevice *
tlite_ce_device_new (GFile *file)
{
	GError *error = NULL;
	TLiteCeDevice *device;
	TLiteCeDevicePrivate *priv;

	g_return_val_if_fail (G_IS_FILE (file), FALSE);

	device = g_initable_new (TLITE_TYPE_CE_DEVICE,
	                          NULL,
	                          &error,
	                          NULL);
	if (!device) {
		g_critical ("Couldn't create new TLiteCeDevice: '%s'",
		            error ? error->message : "unknown error");
		g_clear_error (&error);
		return NULL;
	}

	priv = TLITE_CE_DEVICE_GET_PRIVATE (device);

	priv->file = file;

	priv->name = g_file_get_parse_name (file);
	g_printf ("%s\n", priv->name);

	return device;
}

gboolean
tlite_ce_device_add_db (TLiteCeDevice *device)
{
	TLiteCeDevicePrivate *priv;
	int result;

	g_return_val_if_fail (TLITE_IS_CE_DEVICE (device), FALSE);

	priv = TLITE_CE_DEVICE_GET_PRIVATE (device);

	/* create DB */
	result = sqlite3_open ("aaaa", &priv->database);
	if (result != SQLITE_OK) {
		g_critical ("Couldn't create database: '%s'", priv->name);
		return FALSE;
	}

	/* folders */
	sqlite3_exec (priv->database,
	              "CREATE TABLE IF NOT EXISTS folders(folder_id INTEGER PRIMARY KEY AUTOINCREMENT, folder_path VARCHAR(4096), folder_modified_time INTEGER)",
	              NULL, NULL, NULL);
	sqlite3_exec (priv->database,
				  "CREATE INDEX IF NOT EXISTS fk_folders_id ON folders (folder_path ASC);",
	              NULL, NULL, NULL);

	/* files */
	sqlite3_exec (priv->database,
	              "CREATE TABLE IF NOT EXISTS files(file_id INTEGER PRIMARY KEY AUTOINCREMENT,file_path VARCHAR(4096),file_creation_time INTEGER,file_modified_time INTEGER,file_size INTEGER,parent_folder_id INTEGER)",
	              NULL, NULL, NULL);
	sqlite3_exec (priv->database,
				  "CREATE INDEX IF NOT EXISTS fk_files_id ON files (file_path, file_creation_time, file_modified_time ASC);",
	              NULL, NULL, NULL);

	/* titles */
	sqlite3_exec (priv->database,
				  "CREATE TABLE IF NOT EXISTS titles(title_id INTEGER PRIMARY KEY AUTOINCREMENT,file_id INTEGER,title_name VARCHAR(256))",
	              NULL, NULL, NULL);
	sqlite3_exec (priv->database,
				  "CREATE INDEX IF NOT EXISTS fk_titles_id ON titles (file_id ASC);",
	              NULL, NULL, NULL);

	/* artists */
	sqlite3_exec (priv->database,
				  "CREATE TABLE IF NOT EXISTS artists(artist_id INTEGER PRIMARY KEY AUTOINCREMENT,file_id INTEGER,artist_name VARCHAR(256))",
	              NULL, NULL, NULL);
	sqlite3_exec (priv->database,
				  "CREATE INDEX IF NOT EXISTS fk_artists_id ON artists (file_id  ASC);",
	              NULL, NULL, NULL);

	/* albums */
	sqlite3_exec (priv->database,
				  "CREATE TABLE IF NOT EXISTS albums(album_id INTEGER PRIMARY KEY AUTOINCREMENT,file_id INTEGER,album_name VARCHAR(256))",
	              NULL, NULL, NULL);
	sqlite3_exec (priv->database,
				  "CREATE INDEX IF NOT EXISTS fk_albums_id ON albums (file_id ASC);",
	              NULL, NULL, NULL);

	/* composers */
	sqlite3_exec (priv->database,
				  "CREATE TABLE IF NOT EXISTS composers(composer_id INTEGER PRIMARY KEY AUTOINCREMENT,file_id INTEGER,composer_name VARCHAR(256))",
	              NULL, NULL, NULL);
	sqlite3_exec (priv->database,
				  "CREATE INDEX IF NOT EXISTS fk_composers_id ON composers (file_id ASC);",
	              NULL, NULL, NULL);

	/* genres */
	sqlite3_exec (priv->database,
				  "CREATE TABLE IF NOT EXISTS genres(genre_id INTEGER PRIMARY KEY AUTOINCREMENT,file_id INTEGER,genre_name VARCHAR(256))",
	              NULL, NULL, NULL);

	sqlite3_exec (priv->database,
				  "CREATE INDEX IF NOT EXISTS fk_genres_id ON genres (file_id ASC);",
	              NULL, NULL, NULL);

	return TRUE;
}

GFile *
tlite_ce_device_get_file (TLiteCeDevice  *device)
{
	g_return_val_if_fail (TLITE_IS_CE_DEVICE (device), FALSE);
	TLiteCeDevicePrivate *priv;

	priv = TLITE_CE_DEVICE_GET_PRIVATE (device);
	return priv->file;
}
