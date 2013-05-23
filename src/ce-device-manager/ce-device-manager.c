#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gio/gio.h>

#include "tlite-subsystem-object.h"
#include "tlite-crawler-object.h"
#include "tlite-miner-object.h"
#include "tlite-store-object.h"
#include "ce-device.h"
#include "ce-device-manager.h"

#define TLITE_CE_DEVICE_MANAGER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TLITE_TYPE_CE_DEVICE_MANAGER, TLiteCeDeviceManagerPrivate))

#define TLITE_CE_DEVICE_MANAGER_DBUS_INTERFACE   "org.freedesktop.TLite.CeDeviceManager"
#define TLITE_CE_DEVICE_MANAGER_DBUS_NAME_PREFIX "org.freedesktop.TLite.CeDeviceManager"
#define TLITE_CE_DEVICE_MANAGER_DBUS_PATH_PREFIX "/org/freedesktop/TLite/CeDeviceManager"

/* Introspection data for the service we are exporting */
static const gchar introspection_xml[] =
  "<node>"
  "  <interface name='org.freedesktop.TLite.CeDeviceManager'>"
  "    <signal name='CeDeviceAdded' />"
  "    <signal name='CeDeviceRemoved' />"
  "    <signal name='DirScanStarted' />"
  "    <signal name='DirScanFinished' />"
  "    <signal name='FileScanStarted' />"
  "    <signal name='FileScanProgress' />"
  "    <signal name='FileScanFinished' />"
  "    <signal name='IndexingStarted' />"
  "    <signal name='IndexingProgress' />"
  "    <signal name='IndexingFinished' />"
  "    <signal name='StoreStarted' />"
  "    <signal name='StoreProgress' />"
  "    <signal name='StoreFinished' />"
  "  </interface>"
  "</node>";

struct _TLiteCeDeviceManagerPrivate {
	GList *devices;			/* connected devices */
	GList *miners;			/* available miners */
	GList *crawlers;		/* available crawlers */
	GList *subsystems;		/* available subsystems */
	GList *stores;			/* available stores */

	GDBusConnection *dbus;
	GDBusNodeInfo *introspection_data;
	guint registration_id;

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
	DIR_SCAN_STARTED,
	DIR_SCAN_FINISHED,
	FILE_SCAN_STARTED,
	FILE_SCAN_PROGRESS,
	FILE_SCAN_FINISHED,
	INDEXING_STARTED,
	INDEXING_PROGRESS,
	INDEXING_FINISHED,
	STORE_STARTED,
	STORE_PROGRESS,
	STORE_FINISHED,
	LAST_SIGNAL
};

static guint signals [LAST_SIGNAL] = { 0 };

static void
ce_device_manager_finalize (GObject *object)
{
	TLiteCeDeviceManagerPrivate *priv;

	priv = TLITE_CE_DEVICE_MANAGER_GET_PRIVATE (object);


	if (priv->registration_id != 0) {
		g_dbus_connection_unregister_object (priv->dbus,
		                                     priv->registration_id);
	}

	if (priv->introspection_data) {
		g_dbus_node_info_unref (priv->introspection_data);
	}

	if (priv->dbus) {
		g_object_unref (priv->dbus);
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

	signals [DIR_SCAN_STARTED] =
		g_signal_new ("dir-scan-started",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TLiteCeDeviceManagerClass, dir_scan_started),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__STRING,
		              G_TYPE_NONE, 1,
		              G_TYPE_STRING);

	signals [DIR_SCAN_FINISHED] =
		g_signal_new ("dir-scan-finished",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TLiteCeDeviceManagerClass, dir_scan_finished),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__STRING,
		              G_TYPE_NONE, 1,
		              G_TYPE_STRING);

	signals [FILE_SCAN_STARTED] =
		g_signal_new ("file-scan-started",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TLiteCeDeviceManagerClass, file_scan_started),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__STRING,
		              G_TYPE_NONE, 1,
		              G_TYPE_STRING);

	signals [FILE_SCAN_PROGRESS] =
		g_signal_new ("file-scan-progress",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TLiteCeDeviceManagerClass, file_scan_progress),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__STRING,
		              G_TYPE_NONE, 1,
		              G_TYPE_STRING);

	signals [FILE_SCAN_FINISHED] =
		g_signal_new ("file-scan-finished",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TLiteCeDeviceManagerClass, file_scan_finished),
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

	signals [STORE_STARTED] =
		g_signal_new ("store-started",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TLiteCeDeviceManagerClass, store_started),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__STRING,
		              G_TYPE_NONE, 1,
		              G_TYPE_STRING);

	signals [STORE_PROGRESS] =
		g_signal_new ("store-progress",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TLiteCeDeviceManagerClass, store_progress),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__STRING,
		              G_TYPE_NONE, 1,
		              G_TYPE_STRING);

	signals [STORE_FINISHED] =
		g_signal_new ("store-finished",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TLiteCeDeviceManagerClass, store_finished),
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
ce_device_manager_store_finished_cb (TLiteStore *store,
                                     gint stored_files,
                        			 TLiteCeDeviceManager *manager)
{
	g_printf ("%s %d\n",__FUNCTION__, stored_files);	
}

static void
ce_device_manager_crawler_found_cb (TLiteCrawler *crawler,
                                    TLiteCeDeviceManager *manager)
{
	TLiteCeDeviceManagerPrivate *priv;
	TLiteCeDevice *device;
	TLiteMiner *miner;
	TLiteStore *store;

	g_printf ("%s\n",__FUNCTION__);
	priv = TLITE_CE_DEVICE_MANAGER_GET_PRIVATE (manager);

	g_signal_emit (manager, signals[CE_DEVICE_ADDED], 0);

	g_dbus_connection_emit_signal (priv->dbus,
			                       NULL,
			                       TLITE_CE_DEVICE_MANAGER_DBUS_PATH_PREFIX,
			                       TLITE_CE_DEVICE_MANAGER_DBUS_INTERFACE,
			                       "CeDeviceAdded",
			                       NULL,
			                       NULL);

	device = tlite_crawler_get_device (crawler);
	priv->devices = g_list_append (priv->devices, device);

	miner = tlite_miner_new ();
	priv->miners = g_list_append (priv->miners, miner);

	store = tlite_store_new (device);
	priv->stores = g_list_append (priv->stores, store);

	tlite_store_create_db (device);

	g_signal_connect_object (store, "finished",
	              		     G_CALLBACK (ce_device_manager_store_finished_cb), manager, 0);

	tlite_store_start (store, miner);
	tlite_miner_start (miner, crawler);
}


static void
ce_device_manager_crawler_finished_cb (TLiteCrawler *crawler,
                                       gint scanned_files,
                                       TLiteCeDeviceManager *manager)
{
	TLiteCeDeviceManagerPrivate *priv = TLITE_CE_DEVICE_MANAGER_GET_PRIVATE (manager);
	g_printf ("%s %d\n",__FUNCTION__, scanned_files);

	g_signal_emit (manager, signals[FILE_SCAN_FINISHED], 0);
	g_dbus_connection_emit_signal (priv->dbus,
			                       NULL,
			                       TLITE_CE_DEVICE_MANAGER_DBUS_PATH_PREFIX,
			                       TLITE_CE_DEVICE_MANAGER_DBUS_INTERFACE,
			                       "FileScanFinished",
			                       NULL,
			                       NULL);

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
					             G_CALLBACK (ce_device_manager_crawler_found_cb), manager, 0);
	g_signal_connect_object (crawler, "finished",
	              				 G_CALLBACK (ce_device_manager_crawler_finished_cb), manager, 0);
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
	GDBusInterfaceVTable interface_vtable = {
		NULL,
		NULL,
		NULL
	};

	manager = TLITE_CE_DEVICE_MANAGER (initable);
	priv = TLITE_CE_DEVICE_MANAGER_GET_PRIVATE (manager);

	/* connect to subsystems */
	subsystem = tlite_subsystem_new ();
	priv->subsystems = g_list_append (priv->subsystems, subsystem);

	g_signal_connect_object (subsystem, "device-added",
	                  G_CALLBACK (ce_device_manager_device_added_cb), manager, 0);

	g_signal_connect_object (subsystem, "device-removed",
	                  G_CALLBACK (ce_device_manager_device_removed_cb), manager, 0);
	
	/* Try to get DBus connection... */
	priv->dbus = g_bus_get_sync (G_BUS_TYPE_SESSION, NULL, &inner_error);
	if (!priv->dbus) {
		g_propagate_error (error, inner_error);
		return FALSE;
	}

	/* Setup introspection data */
	priv->introspection_data = g_dbus_node_info_new_for_xml (introspection_xml, &inner_error);
	if (!priv->introspection_data) {
		g_propagate_error (error, inner_error);
		return FALSE;
	}

	priv->registration_id =
		g_dbus_connection_register_object (priv->dbus,
		                                   TLITE_CE_DEVICE_MANAGER_DBUS_PATH_PREFIX,
	                                       priv->introspection_data->interfaces[0],
	                                       &interface_vtable,
	                                       manager,
	                                       NULL,
		                                   &inner_error);

	if (inner_error) {
		g_propagate_error (error, inner_error);
		g_prefix_error (error,
		                "Could not register the D-Bus object '%s'. ",
		                TLITE_CE_DEVICE_MANAGER_DBUS_PATH_PREFIX);
		return FALSE;
	}

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
