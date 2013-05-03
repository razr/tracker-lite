#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gio/gio.h>

#include "tlite-subsystem-object.h"
#include "tlite-crawler-object.h"
#include "tlite-miner-object.h"
#include "ce-device.h"
#include "ce-device-manager.h"

#define TLITE_CE_DEVICE_MANAGER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TLITE_TYPE_CE_DEVICE_MANAGER, TLiteCeDeviceManagerPrivate))

struct _TLiteCeDeviceManagerPrivate {
	GList *devices;			/* connected devices */
	GList *miners;			/* available miners */
	GList *crawlers;		/* available crawlers */
	GList *subsystems;		/* available subsystems */

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
ce_device_manager_found_cb (TLiteCrawler *crawler,
                            TLiteCeDeviceManager *manager)
{
	TLiteCeDeviceManagerPrivate *priv;
	TLiteCeDevice *device;
	TLiteMiner *miner;

	g_printf ("%s\n",__FUNCTION__);
	g_signal_emit (manager, signals[CE_DEVICE_ADDED], 0);

	priv = TLITE_CE_DEVICE_MANAGER_GET_PRIVATE (manager);

	device = tlite_crawler_get_device (crawler);
	priv->devices = g_list_append (priv->devices, device);

	miner = tlite_miner_new ();
	priv->miners = g_list_append (priv->miners, miner);
	tlite_miner_start (miner, crawler);
}


static void
ce_device_manager_finished_cb (TLiteCrawler *crawler,
                               gint scanned_files,
                               TLiteCeDeviceManager *manager)
{
	g_printf ("%s %d\n",__FUNCTION__, scanned_files);
}

static void
ce_device_manager_device_added_cb (TLiteSubsystem *subsystem,
                                   TLiteCeDevice *device,
                                   TLiteCeDeviceManager *manager)
{
	TLiteCrawler *crawler;
	TLiteCeDeviceManagerPrivate *priv;

	g_printf ("%s\n",__FUNCTION__);

	priv = TLITE_CE_DEVICE_MANAGER_GET_PRIVATE (manager);

	/* check, probably it is created already, but not used */
	crawler = tlite_crawler_new();
	priv->crawlers = g_list_append (priv->crawlers, crawler);

	g_signal_connect_object (crawler, "found",
					             G_CALLBACK (ce_device_manager_found_cb), manager, 0);
	g_signal_connect_object (crawler, "finished",
	              				 G_CALLBACK (ce_device_manager_finished_cb), manager, 0);
	tlite_crawler_start (crawler, device);
}

static void
ce_device_manager_device_removed_cb (TLiteSubsystem *subsystem,
                                   TLiteCeDevice *device)
{
	/* stop crawler, if running */
	/* stop miner, if running */
	/* remove device if indexed */
}

static gboolean
ce_device_manager_initable_init (GInitable     *initable,
                                 GCancellable  *cancellable,
                                 GError       **error)
{
	TLiteCeDeviceManager *manager;
	TLiteCeDeviceManagerPrivate *priv;
	TLiteSubsystem *subsystem;
	GError *inner_error = NULL;

	manager = TLITE_CE_DEVICE_MANAGER (initable);
	priv = TLITE_CE_DEVICE_MANAGER_GET_PRIVATE (manager);

	/* connect to subsystems */
	subsystem = tlite_subsystem_new ();
	priv->subsystems = g_list_append (priv->subsystems, subsystem);

	g_signal_connect_object (subsystem, "device-added",
	                  G_CALLBACK (ce_device_manager_device_added_cb), manager, 0);

	g_signal_connect_object (subsystem, "device-removed",
	                  G_CALLBACK (ce_device_manager_device_removed_cb), manager, 0);
	
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
