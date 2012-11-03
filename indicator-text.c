/*
 * Author: Michael Egli <eglimi@gmail.com>
 */

#include <stdlib.h>
#include <stdio.h>
#include <glib.h>
#include <libappindicator/app-indicator.h>

AppIndicator* indicator = NULL;
GSocket* socket = NULL;
GSocket* client = NULL;

static gboolean udp_socket = FALSE;
static GOptionEntry options[] = 
{
	{"udp-socket", 'u', 0, G_OPTION_ARG_NONE, &udp_socket, "Use IPV4 UDP socket to listen for input. Defaults to FALSE."},
	{NULL}
};

gboolean input_handler(GIOChannel* source, GIOCondition condition, gpointer data)
{
	GError* error = NULL;
	char *b;
	gsize term = 0;

	GIOStatus status;
	do
	{
		status = g_io_channel_read_line( source, &b, NULL, &term, &error );
	} while (status == G_IO_STATUS_AGAIN);

	if(status == G_IO_STATUS_ERROR) 
	{ 
		g_error("Could not read any data: %s\n", error->message);
		return TRUE;
	}
	
	if(b != NULL)
	{
		b[term] = '\0'; // truncate line terminator
		app_indicator_set_label(indicator, b, NULL);
		g_free(b);
	}

	return TRUE;
}

void start_socket()
{
	GError* error = NULL;
	socket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, G_SOCKET_PROTOCOL_UDP, &error);
	if(socket == NULL)
	{
		g_error("Error creating unix socket: %s\n", error->message);
		exit(1);
	}

	GSocketAddress* addr = g_inet_socket_address_new(g_inet_address_new_any(G_SOCKET_FAMILY_IPV4), 3434);
	if(!g_socket_bind(socket, addr, TRUE, &error))
	{
		g_error("Could not bind socket: %s\n", error->message);
		exit(1);
	}
}

int main(int argc, char** argv)
{
	// Option parsing
	GError* error = NULL;
	GOptionContext* context = g_option_context_new ("- text indicator");
	g_option_context_add_main_entries (context, options, NULL);
	g_option_context_add_group (context, gtk_get_option_group (TRUE));
	if(!g_option_context_parse (context, &argc, &argv, &error))
	{
		g_error("option parsing failed: %s\n", error->message);
		exit(1);
	}

	// Handle stdin
	GIOChannel* in_stdin = g_io_channel_unix_new(0); // STDIN for reading
	g_io_add_watch(in_stdin, G_IO_IN, input_handler, NULL);		// watch for input on STDIN

	// Handle unix sockets
	if(udp_socket == TRUE)
	{
		start_socket();
		GIOChannel* in_socket = g_io_channel_unix_new(g_socket_get_fd(socket)); // channel from socket
		g_io_add_watch(in_socket, G_IO_IN, input_handler, NULL);		// watch for input on socket
	}

	// Gtk setup
	gtk_init (&argc, &argv);

	GtkWidget* indicator_menu = gtk_menu_new();

	GtkWidget *sep = gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(indicator_menu), sep);

	GtkWidget* quit_item = gtk_menu_item_new_with_label("Quit");
	gtk_menu_shell_append(GTK_MENU_SHELL(indicator_menu), quit_item);
	g_signal_connect(quit_item, "activate", G_CALLBACK (gtk_main_quit), NULL);

	gtk_widget_show_all(indicator_menu);

	// AppIndicator setup
	indicator = app_indicator_new ("indicator-text", "", APP_INDICATOR_CATEGORY_SYSTEM_SERVICES);
	app_indicator_set_status(indicator, APP_INDICATOR_STATUS_ACTIVE);
	app_indicator_set_label(indicator, "indicator-text", NULL);
	app_indicator_set_menu(indicator, GTK_MENU (indicator_menu));

	// Main loop
	gtk_main ();

	// Cleanup
	if(socket != NULL)
	{
		g_object_unref(socket);
	}

	return 0;
}
