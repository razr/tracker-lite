#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib.h>
#include <gio/gio.h>

#include "options.h"
#include "ce-device-manager.h"

static TLiteCeDeviceManager *manager;
static GMainLoop *main_loop;

static GOptionEntry options[] =
{
	{ NULL }
};

int main(int argc, char *argv[])
{
	GOptionContext *context;
	GError *error = NULL;
	gboolean retval;

	context = g_option_context_new ("- indexer of multimedia devices");
	g_option_context_set_summary (context,
                                "Example: to index directory, use:\n"
                                "\n"
                                "  ./tlite\n"
                                "  mount-added <dirname>");
	retval = g_option_context_parse (context, &argc, &argv, &error);
	g_option_context_free (context);

	main_loop = g_main_loop_new (NULL, FALSE);
	manager = tlite_ce_device_manager_new ();

	g_main_loop_run (main_loop);

	g_object_unref (manager);
	g_main_loop_unref (main_loop);

	return 0;
}
