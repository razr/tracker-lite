#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gio/gio.h>

#include "options.h"
#include "ce-device-manager.h"

static TLiteCeDeviceManager * manager;

static void activate(GApplication *app)
{
	manager = tlite_ce_device_manager_new();
}

int main(int argc, char * argv[])
{
	GApplication *app;
	int status;

	app = g_application_new("com.windriver.tracker-lite",
	                        G_APPLICATION_FLAGS_NONE);

//	g_signal_connect(app, "command-line", G_CALLBACK(command_line), NULL);

	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

	g_application_hold(app);
	status = g_application_run(app, argc, argv);

	g_object_unref(app);

	return status;
}
