#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gio/gio.h>

#include "ce-device.h"

#define TLITE_CE_DEVICE_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TLITE_TYPE_CE_DEVICE, TLiteCeDevicePrivate))

struct _TLiteCeDevicePrivate {
	GFile	   *file;
	gchar	   *name;

	gpointer   *database;
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
	TLiteCeDevicePrivate *priv = TLITE_CE_DEVICE_GET_PRIVATE (object);

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
tlite_ce_device_init (TLiteCeDevice *device)
{
	device->priv = TLITE_CE_DEVICE_GET_PRIVATE (device);
	device->priv->database = NULL;
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

GFile *
tlite_ce_device_get_file (TLiteCeDevice  *device)
{
	g_return_val_if_fail (TLITE_IS_CE_DEVICE (device), FALSE);
	TLiteCeDevicePrivate *priv;

	priv = TLITE_CE_DEVICE_GET_PRIVATE (device);
	return priv->file;
}

gboolean
tlite_ce_device_set_db (TLiteCeDevice  *device,
                        gpointer db)
{
	g_return_val_if_fail (TLITE_IS_CE_DEVICE (device), FALSE);
	device->priv->database = db;

	return TRUE;
}

gpointer
tlite_ce_device_get_db (TLiteCeDevice  *device)
{
	g_return_val_if_fail (TLITE_IS_CE_DEVICE (device), NULL);
	return device->priv->database;
}

gchar *
tlite_ce_device_get_name (TLiteCeDevice  *device)
{
	g_return_val_if_fail (TLITE_IS_CE_DEVICE (device), NULL);
	return device->priv->name;
}