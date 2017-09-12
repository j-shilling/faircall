%{
  #include <stdlib.h>
  #include <glib.h>
  #include <glib/gprintf.h>

  #include <shared/faircall.h>

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
		  if (!faircall_set_forced_even ($<boolean>2, &error));
		    {
		      g_printf ("Could update class. %s\n", error->message);
		      g_error_free (error);
		    }
		}
;

info:
    INFO	{
		  GError *error = NULL;
		  if (!faircall_info (NULL, &error));
		    {
		      g_printf ("Could update class. %s\n", error->message);
		      g_error_free (error);
		    }
		}

 | INFO NAME	{
		  GError *error = NULL;
		  if (!faircall_info ($<string>2, &error));
		    {
		      g_printf ("Could update class. %s\n", error->message);
		      g_error_free (error);
		    }
		  g_free ($<string>2);
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
    HELP	{
		  gchar **msg = get_help_message (0);
		  for (gchar **line = msg; line && *line; line++)
		    g_printf ("%s", *line);
		}
;
%%

void
yyerror (const char *s)
{
  g_printf ("%s\n", s);
}

static gchar *help[] =
{
"\nAvailable Commands:\n",
"\n",
"(q)uit                 Exit the program.\n",
"\n",
"(l)ist                 List saved classes.\n",
"(o)pen                 Open a saved class.\n",
"(c)lose                Close an opened class.\n",
"(i)nfo                 Show infomation about a class or student.\n"
"(e)ven                 Configure a class to call students exactly the same number of times.\n"
"\n",
"call                   Call a student.\n",
"(u)ndo                 Undo the last call.\n",
"(a)bsent               Undo the last call and mark student as absent.\n",
"\n",
"For more information about a command, type \"(h)elp command\"\n",
0
};

gchar **
get_help_message (guint const i)
{
  switch (i)
    {
      default: return help;
    }
}
