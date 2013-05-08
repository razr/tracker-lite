#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib.h>
#include <gio/gio.h>

#include "options.h"
#include "ce-device-manager.h"

static TLiteCeDeviceManager *manager;
static GMainLoop *main_loop;

int main(int argc, char *argv[])
{

	main_loop = g_main_loop_new (NULL, FALSE);
	manager = tlite_ce_device_manager_new ();

	g_main_loop_run (main_loop);

	g_object_unref (manager);
	g_main_loop_unref (main_loop);

	return 0;
}
