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

#include <glib.h>
#include <gtk/gtk.h>

#include "../config.h"

#include "io.h"
#include "asprintf.h"
#include "strdup.h"

enum ListItemCols
{
  NAME_COLUMN, CALLED_COLUMN, N_COLUMNS
};

/************************************************************************
 * STATIC FUNCTION DECLARATIONS                                         *
 ************************************************************************/
static void
print_hello (GtkWidget *widget, gpointer data);

static void
show_list (GtkContainer *content_area, List *list);

static void
about_dialog (GtkWidget *widget, gpointer data);

static void
file_dialog (GtkWidget *widget, gpointer data);

static void
show_name (GtkButton *button, gpointer func_data);

static void
set_absent (GtkButton *button, gpointer func_data);

static void
open_class (GtkWidget *widget, gpointer data);

static void
append_item_to_store (char *name, unsigned int index, bool is_last_called,
		      unsigned int called, unsigned int slots, void *data);

/************************************************************************
 * GLOBAL VARIABLES                                                     *
 ************************************************************************/
static char **classes;

/************************************************************************
 * MAIN FUNCTION                                                        *
 ************************************************************************/

int
main (int argc, char *argv[])
{
  classes = io_get_available_classes ();

  GtkWidget *window;
  GtkWidget *main_vbox, *content_area;
  GtkWidget *menubar;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  gtk_window_set_title (GTK_WINDOW(window), PACKAGE_NAME);
  gtk_widget_set_size_request (GTK_WIDGET(window), 400, 400);

  g_signal_connect(window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

  main_vbox = gtk_vbox_new (FALSE, 1);
  gtk_container_set_border_width (GTK_CONTAINER(main_vbox), 1);
  gtk_container_add (GTK_CONTAINER(window), main_vbox);

  content_area = gtk_vbox_new (FALSE, 0);

  {
    menubar = gtk_menu_bar_new ();

    GtkWidget *file_menu = gtk_menu_new ();

    GtkWidget *open_item = gtk_menu_item_new_with_label ("Open");
    GtkWidget *save_item = gtk_menu_item_new_with_label ("Save");
    GtkWidget *quit_item = gtk_menu_item_new_with_label ("Quit");

    GtkWidget *classes_item = gtk_menu_item_new_with_label ("Classes");

    gtk_menu_shell_append (GTK_MENU_SHELL(file_menu), open_item);
    gtk_menu_shell_append (GTK_MENU_SHELL(file_menu), classes_item);
    gtk_menu_shell_append (GTK_MENU_SHELL(file_menu), save_item);
    gtk_menu_shell_append (GTK_MENU_SHELL(file_menu), quit_item);

    g_signal_connect (open_item, "activate",
  			   G_CALLBACK (file_dialog),
  			   (gpointer ) window);
    g_signal_connect (save_item, "activate",
  			   G_CALLBACK (print_hello),
  			   (gpointer ) "file.save");
    g_signal_connect (quit_item, "activate", G_CALLBACK (gtk_main_quit),
  			   (gpointer ) "file.quit");

    gtk_widget_show (open_item);
    gtk_widget_show (save_item);
    gtk_widget_show (quit_item);

    GtkWidget *classes_menu = gtk_menu_new ();
    for (int i = 0; classes[i] != NULL; i++)
      {
	gpointer *args = (gpointer *) malloc (sizeof (gpointer) * 2);
	args[0] = (gpointer) content_area;
	args[1] = (gpointer) classes[i];

        GtkWidget *class_item = gtk_menu_item_new_with_label (classes[i]);
        gtk_menu_shell_append (GTK_MENU_SHELL(classes_menu), class_item);
        g_signal_connect (class_item, "activate", G_CALLBACK (open_class), args);
        gtk_widget_show (class_item);
      }

    gtk_widget_show (classes_item);
    gtk_menu_item_set_submenu (GTK_MENU_ITEM(classes_item), classes_menu);

    GtkWidget *file_item = gtk_menu_item_new_with_label ("File");
    gtk_widget_show (file_item);

    gtk_menu_item_set_submenu (GTK_MENU_ITEM(file_item), file_menu);

    gtk_menu_bar_append(GTK_MENU_BAR (menubar), file_item);
  }

  gtk_box_pack_start (GTK_BOX(main_vbox), menubar, FALSE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX(main_vbox), content_area, TRUE, TRUE, 0);

  gtk_widget_show_all (window);

  gtk_main ();

  return 0;
}

/************************************************************************
 * FUNCTION IMPLEMENTATIONS                                             *
 ************************************************************************/

static void
print_hello (GtkWidget *widget, gpointer data)
{
  g_message ("Hello, World!\n");
  g_print ("\"%s\"\n", (gchar *) data);
}

static void
about_dialog (GtkWidget *widget, gpointer data)
{
  static GtkWidget *dialog = NULL;

  if (!dialog)
    {
      const gchar *auth[] =
	{ "Jake Shilling",
	NULL };

      dialog = gtk_about_dialog_new ();

      GtkWidget *window = ((GtkWidget *) data);
      gtk_window_set_transient_for (GTK_WINDOW(dialog), GTK_WINDOW(window));
      gtk_about_dialog_set_program_name (GTK_ABOUT_DIALOG(dialog),
      PACKAGE_NAME);
      gtk_about_dialog_set_version (GTK_ABOUT_DIALOG(dialog),
      PACKAGE_VERSION);
      gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG(dialog),
				      "Copyright (c) 2017");
      gtk_about_dialog_set_website (GTK_ABOUT_DIALOG(dialog),
				    "https://github.com/j-shilling/faircall");
      gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG(dialog), auth);

      gtk_widget_show_all (dialog);
    }

  gtk_dialog_run (GTK_DIALOG(dialog));
  gtk_widget_hide (dialog);
}

static void
append_item_to_store (char *name, unsigned int index, bool is_last_called,
		      unsigned int called, unsigned int slots, void *data)
{
  GtkListStore *store = GTK_LIST_STORE(data);
  GtkTreeIter iter;

  gtk_list_store_append (store, &iter);
  gtk_list_store_set (store, &iter, NAME_COLUMN, name, CALLED_COLUMN, called,
		      -1);
}

static void
show_list (GtkContainer *content_area, List *list)
{

  /*
   * Clear content_area
   */
    {
      GList *children, *iter;

      children = gtk_container_get_children (GTK_CONTAINER(content_area));
      for (iter = children; iter != NULL; iter = g_list_next(iter))
	gtk_widget_destroy (GTK_WIDGET(iter->data));
      g_list_free (children);
    }

  /*
   * Create a list of names
   */

  GtkListStore *store = gtk_list_store_new (N_COLUMNS,
  G_TYPE_STRING,
					    G_TYPE_INT);

  list_for_each (list, append_item_to_store, store);

  GtkTreeModel *model = GTK_TREE_MODEL(store);
  GtkWidget *tree = gtk_tree_view_new_with_model (model);

  GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
  GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes (
      "Name", renderer, "text", NAME_COLUMN,
      NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW(tree), column);

  column = gtk_tree_view_column_new_with_attributes ("Called", renderer, "text",
						     CALLED_COLUMN,
						     NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW(tree), column);

  gtk_container_add (GTK_CONTAINER(content_area), tree);
  gtk_widget_show_all (GTK_WIDGET(content_area));
}

static void
open_class (GtkWidget *widget, gpointer data)
{
  gpointer *args = (gpointer *) data;
  GtkWidget *content_area = GTK_WIDGET(args[0]);
  const gchar *class = (const gchar *) args[1];
    {
      /*
       * Clear content_area
       */

      GList *children, *iter;

      children = gtk_container_get_children (GTK_CONTAINER(content_area));
      for (iter = children; iter != NULL; iter = g_list_next(iter))
	gtk_widget_destroy (GTK_WIDGET(iter->data));
      g_list_free (children);
    }

  List *list = io_load_list (class);

  GtkWidget *select_area, *buttons_area;
  GtkWidget *list_area, *name_area;

  select_area = gtk_hbox_new (FALSE, 0);

  list_area = gtk_scrolled_window_new (NULL, NULL);
  show_list (GTK_CONTAINER(list_area), list);

    {
      /*
       * Set up name area
       */

      name_area = gtk_vbox_new (FALSE, 0);

      GtkWidget *label = gtk_label_new (list_get_last_called (list));

      gtk_box_pack_start (GTK_BOX(name_area), label, TRUE, TRUE, 0);

      buttons_area = gtk_hbox_new (FALSE, 0);

      GtkWidget *next = gtk_button_new_with_label ("Next");

      gpointer *callback_data = (gpointer *) malloc (sizeof(gpointer) * 3);
      callback_data[0] = (gpointer) list;
      callback_data[1] = (gpointer) label;
      callback_data[2] = (gpointer) list_area;

      gtk_signal_connect(GTK_OBJECT (next), "clicked",
			 GTK_SIGNAL_FUNC (show_name), callback_data);

      GtkWidget *absent = gtk_button_new_with_label ("Absent");

      gtk_signal_connect(GTK_OBJECT (absent), "clicked",
			 GTK_SIGNAL_FUNC (set_absent), callback_data);

      gtk_box_pack_start (GTK_BOX(buttons_area), absent, TRUE,
      TRUE,
			  0);

      gtk_box_pack_start (GTK_BOX(buttons_area), next, TRUE,
      TRUE,
			  0);

      gtk_box_pack_start (GTK_BOX(name_area), buttons_area, TRUE, TRUE, 0);
    }

  gtk_box_pack_start (GTK_BOX(select_area), list_area, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX(select_area), name_area, TRUE, TRUE, 0);

  gtk_box_pack_start (GTK_BOX(content_area), select_area, TRUE, TRUE, 0);

  gtk_widget_show_all (GTK_WIDGET(content_area));
}

static void
show_name (GtkButton *button, gpointer func_data)
{
  gpointer *callback_data = (gpointer *) func_data;

  List *list = (List *) callback_data[0];
  GtkWidget *label = (GtkWidget *) callback_data[1];
  GtkContainer *list_area = (GtkContainer *) callback_data[2];

  gtk_label_set_text (GTK_LABEL(label), list_call_next (list));
  io_save_list (list);
  show_list (list_area, list);
}

static void
set_absent (GtkButton *button, gpointer func_data)
{

}

static void
file_dialog (GtkWidget *widget, gpointer data)
{
  GtkWidget *dialog;
  dialog = gtk_file_chooser_dialog_new ("Open File",
  				      GTK_WIDGET (data),
  				      GTK_FILE_CHOOSER_ACTION_OPEN,
  				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
  				      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
  				      NULL);
  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
      char *filename;
      filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
      io_set_filename (filename);
      g_free (filename);
    }
  gtk_widget_destroy (dialog);
}
