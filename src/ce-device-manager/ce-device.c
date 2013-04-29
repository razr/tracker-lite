#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gio/gio.h>
#include <sqlite3.h>

#include "ce-device.h"

#define TLITE_CE_DEVICE_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TLITE_TYPE_CE_DEVICE, TLiteCeDevicePrivate))

struct _TLiteCeDevicePrivate {
	GMount	  *mount;

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
tlite_ce_device_new (GMount *mount)
{
	GError *error = NULL;
	TLiteCeDevice *device;
	TLiteCeDevicePrivate *priv;
	GFile *file;

	g_return_val_if_fail (G_IS_MOUNT (mount), FALSE);

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

	priv->mount = mount;

	file = g_mount_get_default_location(priv->mount);
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

	return TRUE;
}
