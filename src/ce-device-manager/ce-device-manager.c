#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gio/gio.h>
#include <gudev/gudev.h>

#include "tlite-crawler-object.h"
#include "ce-device.h"
#include "ce-device-manager.h"

#define TLITE_CE_DEVICE_MANAGER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TLITE_TYPE_CE_DEVICE_MANAGER, TLiteCeDeviceManagerPrivate))

struct _TLiteCeDeviceManagerPrivate {
	GList *devices;			/* connected devices */
	GList *miners;			/* available miners */
	GList *crawlers;		/* available crawlers */

	/* subsystems */
	GVolumeMonitor *volume_monitor; /* valid only for the mounted devices */
	GUdevClient *udev_client;		/* more generic, not used currently */
	GDataInputStream *input_stream; /* for testing */

	/* Property values */
	gboolean auto_start;
};

static void ce_device_manager_initable_iface_init (GInitableIface *iface);

G_DEFINE_TYPE_WITH_CODE (TLiteCeDeviceManager, tlite_ce_device_manager, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (G_TYPE_INITABLE,
                                                ce_device_manager_initable_iface_init));

enum {
	PROP_0,
	PROP_AUTO_START
};

enum {
	CE_DEVICE_ADDED,
	CE_DEVICE_REMOVED,
	INDEXING_STARTED,
	INDEXING_PROGRESS,
	INDEXING_FINISHED,
	LAST_SIGNAL
};

static guint signals [LAST_SIGNAL] = { 0 };

static void
ce_device_manager_finalize (GObject *object)
{
	TLiteCeDeviceManagerPrivate *priv;

	priv = TLITE_CE_DEVICE_MANAGER_GET_PRIVATE (object);

	if (priv->volume_monitor) {
		g_object_unref (priv->volume_monitor);
	}

	if (priv->udev_client) {
		g_object_unref (priv->udev_client);
	}

	G_OBJECT_CLASS (tlite_ce_device_manager_parent_class)->finalize (object);
}

static void
tlite_ce_device_manager_class_init (TLiteCeDeviceManagerClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->finalize = ce_device_manager_finalize;

	signals [CE_DEVICE_ADDED] =
		g_signal_new ("ce-device-added",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TLiteCeDeviceManagerClass, ce_device_added),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__STRING,
		              G_TYPE_NONE, 1,
		              G_TYPE_STRING);

	signals [CE_DEVICE_REMOVED] =
		g_signal_new ("ce-device-removed",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TLiteCeDeviceManagerClass, ce_device_removed),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__STRING,
		              G_TYPE_NONE, 1,
		              G_TYPE_STRING);

	signals [INDEXING_STARTED] =
		g_signal_new ("indexing-started",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TLiteCeDeviceManagerClass, indexing_started),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__STRING,
		              G_TYPE_NONE, 1,
		              G_TYPE_STRING);

	signals [INDEXING_PROGRESS] =
		g_signal_new ("indexing-progress",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TLiteCeDeviceManagerClass, indexing_progress),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__STRING,
		              G_TYPE_NONE, 1,
		              G_TYPE_STRING);

	signals [INDEXING_FINISHED] =
		g_signal_new ("indexing-finished",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TLiteCeDeviceManagerClass, indexing_finished),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__STRING,
		              G_TYPE_NONE, 1,
		              G_TYPE_STRING);

	g_type_class_add_private (object_class, sizeof (TLiteCeDeviceManagerPrivate));
}


static void
tlite_ce_device_manager_init (TLiteCeDeviceManager *manager)
{
	manager->priv = TLITE_CE_DEVICE_MANAGER_GET_PRIVATE (manager);
}

static void
ce_device_manager_uevent_cb (GUdevClient *client,
                             const gchar *action,
                             GUdevDevice *udevice,
                             TLiteCeDeviceManager *manager)
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
ce_device_manager_found_cb (TLiteCrawler *crawler)
{
	TLiteCeDevice *device;
	GMount *mount;
	gchar  *path;

	mount = tlite_crawler_get_mount (crawler);

	/* media content found, create device and add it to the list */
	device = tlite_ce_device_new (mount);

	/* create miner and assign crawler to it */
	g_printf ("%s\n",__FUNCTION__);

}


static void
ce_device_manager_finished_cb (TLiteCrawler *crawler)
{
	g_printf ("%s\n",__FUNCTION__);
}


static void
ce_device_manager_mount_added_cb (GVolumeMonitor *volume_monitor,
                                  GMount         *mount,
                                  TLiteCeDeviceManager *manager)
{
	TLiteCrawler *crawler;
	TLiteCeDeviceManagerPrivate *priv;
	GError error;

	g_printf ("%s\n", g_file_get_path (g_mount_get_default_location(mount)));
	priv = TLITE_CE_DEVICE_MANAGER_GET_PRIVATE (manager);

	/* start crawler and wait for signal */
	crawler = tlite_crawler_new();
	priv->crawlers = g_list_append (priv->crawlers, crawler);

	g_signal_connect_object (crawler, "found",
	                  G_CALLBACK (ce_device_manager_found_cb), manager, 0);
	g_signal_connect_object (crawler, "finished",
	                  G_CALLBACK (ce_device_manager_finished_cb), manager, 0);

	/* TODO: replace mount with a usb id, probably with something even more generic */
	tlite_crawler_start(crawler, mount);

}

static void
ce_device_manager_mount_removed_cb (GVolumeMonitor *volume_monitor,
                                    GMount         *mount,
                                    TLiteCeDeviceManager *manager)
{
	g_printf ("%s\n", g_file_get_path (g_mount_get_default_location(mount)));

	/* TODO: remove devices from the list */
}

static void ce_device_manager_read_line_cb(GObject *src,
                                           GAsyncResult *res,
                                           gpointer data)
{
	char *s, **ss;
    GError *error = NULL;
	gsize len;
	TLiteCeDeviceManager *manager = data;
	TLiteCeDeviceManagerPrivate *priv;

	priv = TLITE_CE_DEVICE_MANAGER_GET_PRIVATE (manager);

	s = g_data_input_stream_read_line_finish(G_DATA_INPUT_STREAM(src), res,
                                             &len, &error);
	/* two commands supported */
	ss = g_strsplit (s, " ", 0);

	g_printf ("%s:%s\n",ss[0], ss[1]);

	if (0 == g_strcmp0 ("mount-added", ss[0])) {
		GFile *file;
		GMount *mount;
		TLiteCrawler *crawler;

		file = g_file_new_for_path (ss[1]);
		mount = g_file_find_enclosing_mount (file, NULL, &error);

		/* start crawler and wait for signal */
		crawler = tlite_crawler_new();
		priv->crawlers = g_list_append (priv->crawlers, crawler);

		g_signal_connect_object (crawler, "found",
					             G_CALLBACK (ce_device_manager_found_cb), manager, 0);
		g_signal_connect_object (crawler, "finished",
	              				 G_CALLBACK (ce_device_manager_finished_cb), manager, 0);

		/* TODO: replace mount with a usb id, probably with something even more generic */
		tlite_crawler_start(crawler, mount);
	}

	g_free(s);
	g_strfreev(ss);
}

static gboolean
ce_device_manager_initable_init (GInitable     *initable,
                                 GCancellable  *cancellable,
                                 GError       **error)
{
	TLiteCeDeviceManager *manager;
	TLiteCeDeviceManagerPrivate *priv;
	const gchar *subsystems[] = {"usb", NULL};
	GError *inner_error = NULL;

	manager = TLITE_CE_DEVICE_MANAGER (initable);
	priv = TLITE_CE_DEVICE_MANAGER_GET_PRIVATE (manager);

	/* for non mountable devices, like mtp and iPod */
	priv->udev_client = g_udev_client_new (subsystems);
	g_signal_connect_object (priv->udev_client, "uevent",
	                  G_CALLBACK (ce_device_manager_uevent_cb), manager, 0);

	/* for mountable devices, like USB sticks */
	priv->volume_monitor = g_volume_monitor_get ();
	g_signal_connect_object (priv->volume_monitor, "mount-added", 
	                         G_CALLBACK (ce_device_manager_mount_added_cb), manager, 0);
	g_signal_connect_object (priv->volume_monitor, "mount-removed",
	                         G_CALLBACK (ce_device_manager_mount_removed_cb), manager, 0);

	/* for mountable devices, like USB sticks */
	priv->input_stream = G_INPUT_STREAM (g_data_input_stream_new (g_unix_input_stream_new(STDIN_FILENO, FALSE)));
	g_data_input_stream_read_line_async(priv->input_stream, G_PRIORITY_DEFAULT, NULL,
                                       ce_device_manager_read_line_cb, manager);
	
	return TRUE;
}


static void
ce_device_manager_initable_iface_init (GInitableIface *iface)
{
	iface->init = ce_device_manager_initable_init;
}


TLiteCeDeviceManager *
tlite_ce_device_manager_new (void)
{
	GError *inner_error = NULL;
	TLiteCeDeviceManager *manager;

	manager = g_initable_new (TLITE_TYPE_CE_DEVICE_MANAGER,
	                          NULL,
	                          &inner_error,
	                          NULL);
	if (!manager) {
		g_critical ("Couldn't create new TLiteCeDeviceManager: '%s'",
		            inner_error ? inner_error->message : "unknown error");
		g_clear_error (&inner_error);
	}

	return manager;
}
