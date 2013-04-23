#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gio/gio.h>

#include "options.h"

static struct options cmd_options;

static GOptionEntry options[] =
{
	{ "device-id",     'i', 0, G_OPTION_ARG_STRING, &cmd_options.deviceId, "deviceID ", NULL },
	{ "device-path",   'p', 0, G_OPTION_ARG_STRING, &cmd_options.devicePath, "devicePath ", NULL },
	{ NULL }
};

int
command_line(GApplication            *application,
             GApplicationCommandLine *cmdline)
{
  gchar **argv;
  gint argc;
  gint i;

  argv = g_application_command_line_get_arguments(cmdline, &argc);

  for(i = 0; i < argc; i++)
    g_print ("handling argument %s remotely\n", argv[i]);

  g_strfreev(argv);

  g_application_register(application, NULL, NULL);

  return 0;
}
