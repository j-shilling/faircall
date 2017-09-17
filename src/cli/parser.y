%{
  #include <stdlib.h>
  #include <glib.h>
  #include <glib/gprintf.h>

  #include <faircall.h>
  #include "help.h"

  int yylex (void);
  void yyerror (const char *s);

  gchar **get_help_message (guint const i);
%}

%define parse.error verbose
%union
{
  gchar *string;
  gboolean boolean;
  guint integer;
}

%token ADD
%token DELETE
%token INFO
%token LIST
%token OPEN
%token CLOSE
%token UNDO
%token ABSENT
%token EVEN
%token CALL
%token HELP
%token QUIT

%token NAME
%token BOOL
%token INT

%%

cmd:
   quit
|  open
|  add
|  delete
|  close
|  even
|  info
|  call
|  undo
|  absent
|  list
|  help
;

quit:
    QUIT	{ exit (EXIT_SUCCESS); }
;

open:
    OPEN NAME	{
		  GError *error = NULL;
		  if (!faircall_open_class ($<string>2, &error))
		    {
		      g_printf ("Could not open class \"%s\". %s\n",
		        $<string>2, error->message);
		      g_error_free (error);
		    }
		  g_free ($<string>2);
		}
;

add: ADD NAME	{
		  GError *error = NULL;
		  if (!faircall_add_student ($<string>2, &error))
		    {
		      g_printf ("Could not add student \"%s\". %s\n",
		        $<string>2, error->message);
		      g_error_free (error);
		    }
		  g_free ($<string>2);
		}
;

delete:
   DELETE NAME	{
		  GError *error = NULL;
		  if (!faircall_del_student ($<string>2, &error))
		    {
		      g_printf ("Could not delete student \"%s\". %s\n",
		        $<string>2, error->message);
		      g_error_free (error);
		    }
		  g_free ($<string>2);
		}
;

close:
     CLOSE	{
		  GError *error = NULL;
		  if (!faircall_close_class (&error))
		    {
		      g_printf ("Could not close class. %s\n", error->message);
		      g_error_free (error);
		    }
		}
;

even:
    EVEN BOOL	{
		  GError *error = NULL;
		  if (!faircall_set_forced_even ($<boolean>2, &error))
		    {
		      g_printf ("Could update class. %s\n", error->message);
		      g_error_free (error);
		    }
		}
;

info:
    INFO	{
		  GError *error = NULL;
		  gchar **info = faircall_info (NULL, &error);
		  if (error)
		    {
		      g_printf ("Could update class. %s\n", error->message);
		      g_error_free (error);
		    }

                  for (gchar **line = info; line && *line; line++)
                    g_printf ("%s", *line);
		  g_strfreev (info);
		}

 | INFO NAME	{
		  GError *error = NULL;
		  gchar **info = faircall_info ($<string>2, &error);
		  if (error)
		    {
		      g_printf ("Could update class. %s\n", error->message);
		      g_error_free (error);
		    }
		  g_free ($<string>2);

                  for (gchar **line = info; line && *line; line++)
                    g_printf ("%s", *line);
		  g_strfreev (info);
		}
;

call:
    CALL	{
		  GError *error = NULL;
		  gchar *name = faircall_call_student (&error);
		  if (error)
		    g_printf ("Error calling student: %s\n",
		              error->message);
		  if (!name)
		    g_printf ("Could not call student.\n");
		  else
		    g_printf ("%s\n", name);

		  g_free (name);
		  if (error) g_error_free (error);
		}
 |  CALL NAME	{
		  GError *error = NULL;
		  faircall_call_student_by_name ($<string>2, &error);
		  if (error)
		    g_printf ("Error calling student: %s\n",
		              error->message);
		  if (error) g_error_free (error);
		  g_free ($<string>2);
		}
 |  CALL INT	{
		  GError *error = NULL;
		  faircall_call_n_students ($<integer>2, &error);
		  if (error)
		    g_printf ("Error calling students: %s\n",
		              error->message);
		  if (error) g_error_free (error);
		}
;

undo:
    UNDO	{
		  GError *error = NULL;
		  if (!faircall_undo_call (&error))
		    {
		      g_printf ("Error undoing call. %s\n",
		        error->message);
		      g_error_free (error);
		    }
		}
;

absent:
      ABSENT	{
		  GError *error = NULL;
		  if (!faircall_mark_absent (&error))
		    {
		      g_printf ("Error marking absence. %s\n",
		        error->message);
		      g_error_free (error);
		    }
		}
;

list:
    LIST	{
		  GError *error = NULL;
		  gchar **list = faircall_list (&error);
		  if (error)
		    {
		      g_printf ("Error listing: %s\n", error->message);
		      g_error_free (error);
		    }
		  for (gchar **line = list;
		       line && *line;
		       line ++)
		    g_printf ("%s", *line);

		  if (list) g_strfreev (list);
		}
;

help:
    HELP	{ faircall_print_help_menu (HELP_M); }
|   HELP HELP	{ faircall_print_help_menu (HELP_M); }
|   HELP ADD	{ faircall_print_help_menu (ADD_M); }
|   HELP DELETE	{ faircall_print_help_menu (DELETE_M); }
|   HELP INFO	{ faircall_print_help_menu (INFO_M); }
|   HELP OPEN	{ faircall_print_help_menu (OPEN_M); }
|   HELP CLOSE	{ faircall_print_help_menu (CLOSE_M); }
|   HELP UNDO	{ faircall_print_help_menu (UNDO_M); }
|   HELP ABSENT	{ faircall_print_help_menu (ABSENT_M); }
|   HELP EVEN	{ faircall_print_help_menu (EVEN_M); }
|   HELP CALL	{ faircall_print_help_menu (CALL_M); }
|   HELP QUIT	{ faircall_print_help_menu (QUIT_M); }
|   HELP LIST	{ faircall_print_help_menu (LIST_M); }
;
%%

void
yyerror (const char *s)
{
  g_printf ("%s\n", s);
}
