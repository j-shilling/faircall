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
#include "class.h"

/************************************************************************
 * STATIC FUNCTION DECLARATIONS                                         *
 ************************************************************************/
static void
print_hello (GtkWidget *widget, gpointer data);

static void
open_dialog (gpointer callback_data, guint callback_action,
	     GtkWidget *menu_item);
static void
about_dialog (gpointer callback_data, guint callback_action,
	      GtkWidget *menu_item);

/************************************************************************
 * GLOBAL VARIABLES                                                     *
 ************************************************************************/
static GtkItemFactoryEntry menu_items[] =
  {
  /* Path, Accelerator, callback, callback_action, item_type */
    { "/_File", NULL, NULL, 0, "<Branch>" },
    { "/File/_New", "<control>N", print_hello, 0, "<StockItem>",
    GTK_STOCK_NEW },
    { "/File/_Open", "<control>O", open_dialog, 1, "<StockItem>",
    GTK_STOCK_OPEN },
    { "/File/_Save", "<control>S", print_hello, 0, "<StockItem>",
    GTK_STOCK_SAVE },
    { "/File/sep1", NULL, NULL, 0, "<Separator>" },
    { "/File/_Quit", "<control>Q", gtk_main_quit, 0, "<StockItem>",
    GTK_STOCK_QUIT },

    { "/_Help", NULL, NULL, 0, "<Branch>" },
    { "/Help/_About", NULL, about_dialog, 1, "<StockItem>", GTK_STOCK_ABOUT } };

static gint nmenu_items = sizeof(menu_items) / sizeof(menu_items[0]);

static class_t **classes;

/************************************************************************
 * MAIN FUNCTION                                                        *
 ************************************************************************/

int
main (int argc, char *argv[])
{
  classes = get_class_list();

  GtkWidget *window;
  GtkWidget *main_vbox;
  GtkWidget *menubar;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  gtk_window_set_title (GTK_WINDOW(window), PACKAGE_NAME);
  gtk_widget_set_size_request (GTK_WIDGET(window), 600, 600);

  g_signal_connect(window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

  main_vbox = gtk_vbox_new (FALSE, 1);
  gtk_container_set_border_width (GTK_CONTAINER(main_vbox), 1);
  gtk_container_add (GTK_CONTAINER(window), main_vbox);

  GtkItemFactory *item_factory;
  GtkAccelGroup *accel_group;

  accel_group = gtk_accel_group_new ();

  item_factory = gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>",
				       accel_group);

  gtk_item_factory_create_items (item_factory, nmenu_items, menu_items, window);

  gtk_window_add_accel_group (GTK_WINDOW(window), accel_group);

  menubar = gtk_item_factory_get_widget (item_factory, "<main>");

  gtk_box_pack_start (GTK_BOX(main_vbox), menubar, FALSE, TRUE, 0);

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
}

static void
open_dialog (gpointer callback_data, guint callback_action,
	     GtkWidget *menu_item)
{
  GtkWidget *dialog = gtk_dialog_new_with_buttons (
      "Open Class", GTK_WINDOW(callback_data),
      GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
      GTK_STOCK_CANCEL,
      GTK_RESPONSE_REJECT,
      GTK_STOCK_OPEN,
      GTK_RESPONSE_ACCEPT,
      NULL);

  enum
  {
    CLASS_ID, NAME_COLUMN, SIZE_COLUMN, LAST_CALLED_COLUMN, N_COLUMNS
  };

  GtkListStore *store = gtk_list_store_new (N_COLUMNS,
					    G_TYPE_INT,
					    G_TYPE_STRING,
					    G_TYPE_INT,
					    G_TYPE_STRING);

  GtkTreeIter iter;
  int i = 0;

  while (classes[i] != NULL)
    {
      gtk_list_store_append (store, &iter);
      gtk_list_store_set (store, &iter, CLASS_ID, i, NAME_COLUMN,
			  class_get_name (classes[i]), SIZE_COLUMN,
			  class_get_size (classes[i]), LAST_CALLED_COLUMN,
			  class_get_last_called (classes[i]), -1);
      i++;
    }

  GtkWidget *tree = gtk_tree_view_new_with_model (GTK_TREE_MODEL(store));

  GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
  GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes (
      "Class", renderer, "text", NAME_COLUMN,
      NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW(tree), column);

  column = gtk_tree_view_column_new_with_attributes ("Size", renderer, "text",
						     SIZE_COLUMN,
						     NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW(tree), column);

  column = gtk_tree_view_column_new_with_attributes ("Last Called", renderer,
						     "text", LAST_CALLED_COLUMN,
						     NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW(tree), column);

  GtkWidget *content_area = gtk_dialog_get_content_area (GTK_DIALOG(dialog));
  gtk_box_pack_end (GTK_BOX(content_area), tree,
		      TRUE, TRUE, 0);

  gtk_widget_show_all (GTK_WIDGET(dialog));

  gint result = gtk_dialog_run (GTK_DIALOG (dialog));

  switch (result)
  {
    case GTK_RESPONSE_ACCEPT:
      g_message ("Open clicked\n");
      break;
    case GTK_RESPONSE_REJECT:
      break;
    default:
      break;
  }

  gtk_widget_hide (dialog);
}

static void
about_dialog (gpointer callback_data, guint callback_action,
	      GtkWidget *menu_item)
{
  static GtkWidget *dialog = NULL;

  if (!dialog)
    {
      const gchar *auth[] =
	{ "Jake Shilling",
	NULL };

      dialog = gtk_about_dialog_new ();
      gtk_window_set_transient_for (GTK_WINDOW(dialog),
				    GTK_WINDOW(callback_data));
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
