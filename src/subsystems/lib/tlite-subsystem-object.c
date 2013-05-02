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

#include <gio/gio.h>
#include <gio/gunixinputstream.h>
#include <gudev/gudev.h>

#include "tlite-subsystem-object.h"
#include "ce-device.h"

#define TLITE_SUBSYSTEM_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TLITE_TYPE_SUBSYSTEM, TLiteSubsystemPrivate))

struct TLiteSubsystemPrivate {
	GVolumeMonitor *volume_monitor; /* valid only for the mounted devices */
	GUdevClient *udev_client;		/* more generic, not used currently */
	GDataInputStream *input_stream; /* for testing */

	GList *devices;
};

enum {
	DEVICE_ADDED,
	DEVICE_REMOVED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0, };
static GQuark file_info_quark = 0;

G_DEFINE_TYPE (TLiteSubsystem, tlite_subsystem, G_TYPE_OBJECT)

static void
subsystem_finalize (GObject *object)
{
	TLiteSubsystemPrivate *priv;

	priv = TLITE_SUBSYSTEM_GET_PRIVATE (object);

	if (priv->volume_monitor) {
		g_object_unref (priv->volume_monitor);
	}

	if (priv->udev_client) {
		g_object_unref (priv->udev_client);
	}

	G_OBJECT_CLASS (tlite_subsystem_parent_class)->finalize (object);
}


static void
tlite_subsystem_class_init (TLiteSubsystemClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->finalize = subsystem_finalize;

	signals[DEVICE_ADDED] =
		g_signal_new ("device-added",
		              G_TYPE_FROM_CLASS (klass),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TLiteSubsystemClass, device_added),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__POINTER,
		              G_TYPE_NONE, 1, TLITE_TYPE_CE_DEVICE);
	signals[DEVICE_REMOVED] =
		g_signal_new ("device-removed",
		              G_TYPE_FROM_CLASS (klass),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TLiteSubsystemClass, device_removed),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__POINTER,
		              G_TYPE_NONE, 1, TLITE_TYPE_CE_DEVICE);

	g_type_class_add_private (object_class, sizeof (TLiteSubsystemPrivate));

	file_info_quark = g_quark_from_static_string ("tlite-subsystem-file-info");
}

static void
tlite_subsystem_init (TLiteSubsystem *subsystem)
{
	subsystem->priv = TLITE_SUBSYSTEM_GET_PRIVATE (subsystem);
}

static gint 
find_device (gconstpointer a, 
             gconstpointer b)
{
	GFile *file;
	file = tlite_ce_device_get_file ((TLiteCeDevice *)a);

	return g_file_equal (file, (GFile *)b)?0:1;
}

static void subsystem_read_line_cb(GObject *src,
                                   GAsyncResult *res,
                                   gpointer data)
{
	char *s, **ss;
    GError *error = NULL;
	gsize len;
	TLiteSubsystem *subsystem = data;
	TLiteSubsystemPrivate *priv;
	TLiteCeDevice *device;
	GFile *file;

	priv = TLITE_SUBSYSTEM_GET_PRIVATE (subsystem);

	s = g_data_input_stream_read_line_finish(G_DATA_INPUT_STREAM(src), res,
                                             &len, &error);
	/* two commands supported */
	ss = g_strsplit (s, " ", 0);

	/* TODO: check valid path here */
	g_printf ("%s:%s\n",ss[0], ss[1]);
	file = g_file_new_for_path (ss[1]);

	if (0 == g_strcmp0 ("mount-added", ss[0])) {
		/* create device and to subsystem and emit signal */
		device = tlite_ce_device_new (file);
		priv->devices = g_list_append (priv->devices, device);

		g_printf ("%s\n",__FUNCTION__);
		g_signal_emit (subsystem, signals[DEVICE_ADDED], 0, device);
	}

	if (0 == g_strcmp0 ("mount-removed", ss[0])) {
		GList *found;

		found = g_list_find_custom (priv->devices, file, find_device);

		if (found != NULL) {
			g_signal_emit (subsystem, signals[DEVICE_REMOVED], 0, found->data);
			priv->devices = g_list_delete_link (priv->devices, found);
		}
	}

	g_free(s);
	g_strfreev(ss);
}

static void
subsystem_uevent_cb (GUdevClient *client,
                     const gchar *action,
                     GUdevDevice *udevice,
                     TLiteSubsystem *subsystem)
{
	if (g_str_equal (g_udev_device_get_devtype(udevice), "usb_device")) {
		g_printf ("%s ID_SERIAL : %s\n", action, g_udev_device_get_property (udevice, "ID_SERIAL"));
	}

#if 0
		/* print all properties */
		gchar **list, **iter;
		list = g_udev_device_get_property_keys (udevice);
		for (iter = list; iter && *iter; iter++) {
			g_printf ("%s : %s\n", *iter, g_udev_device_get_property (udevice, *iter));
		}
#endif
}

static void
subsystem_mount_added_cb (GVolumeMonitor *volume_monitor,
                          GMount         *mount,
                          TLiteSubsystem *subsystem)
{
	TLiteSubsystemPrivate *priv;
	TLiteCeDevice *device;
	GFile *file;

	file = g_mount_get_default_location(mount);
	g_printf ("%s\n", g_file_get_path (file));

	priv = TLITE_SUBSYSTEM_GET_PRIVATE (subsystem);

	/* create device and to subsystem and emit signal */
	device = tlite_ce_device_new (file);
	priv->devices = g_list_append (priv->devices, device);

	g_signal_emit (subsystem, signals[DEVICE_ADDED], 0, device);
}

static void
subsystem_mount_removed_cb (GVolumeMonitor *volume_monitor,
                            GMount         *mount,
                            TLiteSubsystem *subsystem)
{
	TLiteSubsystemPrivate *priv;
	GFile *file;
	GList *found;

	priv = TLITE_SUBSYSTEM_GET_PRIVATE (subsystem);
	file = g_mount_get_default_location(mount);

	g_printf ("%s\n", g_file_get_path (file));

	found = g_list_find_custom (priv->devices, file, find_device);

	if (found != NULL) {
		g_signal_emit (subsystem, signals[DEVICE_REMOVED], 0, found->data);
		priv->devices = g_list_delete_link (priv->devices, found);
	}
}

TLiteSubsystem *
tlite_subsystem_new (void)
{
	TLiteSubsystem *subsystem;
	TLiteSubsystemPrivate *priv;
	const gchar *subsystems[] = {"usb", NULL};

	subsystem = g_object_new (TLITE_TYPE_SUBSYSTEM, NULL);
	priv = TLITE_SUBSYSTEM_GET_PRIVATE (subsystem);

	/* for non mountable devices, like mtp and iPod */
	priv->udev_client = g_udev_client_new (subsystems);
	g_signal_connect_object (priv->udev_client, "uevent",
	                  G_CALLBACK (subsystem_uevent_cb), subsystem, 0);

	/* for mountable devices, like USB sticks */
	priv->volume_monitor = g_volume_monitor_get ();
	g_signal_connect_object (priv->volume_monitor, "mount-added", 
	                         G_CALLBACK (subsystem_mount_added_cb), subsystem, 0);
	g_signal_connect_object (priv->volume_monitor, "mount-removed",
	                         G_CALLBACK (subsystem_mount_removed_cb), subsystem, 0);

	/* for testing */
	priv->input_stream = g_data_input_stream_new (g_unix_input_stream_new (STDIN_FILENO, TRUE));
	g_data_input_stream_read_line_async(priv->input_stream, G_PRIORITY_DEFAULT, NULL,
                                        subsystem_read_line_cb, subsystem);

	return subsystem;
}
