/*
 *    This file is part of faircall.
 *
 *    faircall is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    faircall is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with faircall.  If not, see <http://www.gnu.org/licenses/>.
 */

/************************************************************************
 * INCLUDE FILES                                                        *
 ************************************************************************/

#include <stdlib.h>

#include <gtk/gtk.h>

#include "../config.h"

/************************************************************************
 * MAIN and ACTIVATE                                                    *
 ************************************************************************/

static void
start_up (GtkApplication* app,
	  gpointer        user_data)
{

}

static void
activate (GtkApplication* app,
          gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *pane;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Faircall");
  gtk_window_set_default_size (GTK_WINDOW (window), 600, 500);

  pane = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_container_add (GTK_CONTAINER(window), pane);

  gtk_paned_add1 (GTK_PANED (pane), gtk_label_new ("one"));
  gtk_paned_add2 (GTK_PANED (pane), gtk_label_new ("two"));

  gtk_widget_show_all (window);
}

int
main (int    argc,
      char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("com.shilling.faircall", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  g_signal_connect (app, "start_up", G_CALLBACK (start_up), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
