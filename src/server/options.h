#ifndef OPTIONS_H
#define OPTIONS_H

#include <glib.h>
#include <gio/gio.h>

G_BEGIN_DECLS

struct options {
	gchar * deviceId;
	gchar * devicePath;	
};

int
command_line (GApplication            *application,
              GApplicationCommandLine *cmdline);

G_END_DECLS

#endif /* OPTIONS_H */
