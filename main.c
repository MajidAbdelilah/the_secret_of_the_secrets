#include <gtk/gtk.h>
#include "sos.h"

int main(int arg_num, char **args)
{
	GtkApplication *app;
	int status;
	app = gtk_application_new("org.gtk4.exapmles", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "activate", G_CALLBACK(activate_callback), NULL);
	status = g_application_run(G_APPLICATION(app), arg_num, args);
	g_object_unref(app);
	return status;
}