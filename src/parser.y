%{
  #include <stdlib.h>
  #include <glib.h>
  #include <glib/gprintf.h>

  #include <shared/faircall.h>

  int yylex (void);
  void yyerror (const char *s);
%}

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

quit:
    QUIT	{ exit (EXIT_SUCCESS); }
;

open:
    OPEN NAME	{
		  gboolean new = FALSE;
		  GError *error = NULL;
		  if (!faircall_open_class ($<string>2, &new &error))
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
		  if (!faircall_list (&error))
		    {
		      g_printf ("Error listing: %s\n", error->message);
		      g_error_free (error);
		    }
		}
;
%%

void
yyerror (const char *s)
{
  g_printf ("%s\n", s);
}
