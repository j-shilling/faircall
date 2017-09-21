#include <stdlib.h>
#include <stdio.h>
#include <locale.h>

#include <glib.h>
#include <glib/gprintf.h>

#include <readline/readline.h>
#include <readline/history.h>

#include <faircall.h>
#include <parser.h>
#include <config.h>

struct yy_buffer_state;
typedef struct yy_buffer_state *YY_BUFFER_STATE;
YY_BUFFER_STATE yy_scan_string(const char *);
void yy_delete_buffer( YY_BUFFER_STATE buffer );

static gchar **names_to_add = NULL;
static gchar **names_to_del = NULL;
static gchar **class_names = NULL;

static gint count = -1;
static gboolean show_info = FALSE;

static GOptionEntry entries[] =
{
  {"add", 'a', 0, G_OPTION_ARG_STRING_ARRAY, &names_to_add, "Add a student to a class", "[name]"},
  {"remove", 'r', 0, G_OPTION_ARG_STRING_ARRAY, &names_to_del, "Remove a student from a class", "[name]"},
  {"calls", 'c', 0, G_OPTION_ARG_INT, &count, "Call the specified number of students.", "[INT]"},
  {"info", 'i', 0, G_OPTION_ARG_NONE, &show_info, "Print infomation of a given class", NULL},
  {G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_STRING_ARRAY, &class_names, "The name of the class to operate on.", NULL},

  { NULL }
};

static void cleanup(void);
static void showinfo(void);

static char **name_completion (const char *, int, int);
static char *name_generator (const char *, int);

static gchar **class = NULL;
static gchar **students = NULL;
static gchar *commands[] =
{
  "quit",
  "list",
  "open",
  "close",
  "info",
  "even",
  "add",
  "delete",
  "call",
  "undo",
  "absent",
  "help",
  NULL
};
static gchar **symbols = NULL;

int
main (int argc, char *argv[])
{
  setlocale (LC_ALL, "");
  atexit(cleanup);

  GError *error = NULL;
  gchar **args = NULL;
  GOptionContext *context = g_option_context_new ("[class]");
  g_option_context_set_summary (context, "Call on students fairly.");
  g_option_context_add_main_entries (context, entries, NULL);

#ifdef G_OS_WIN32
  args = g_win32_get_command_line ();
#else
  args = g_strdupv (argv);
#endif

  if (!g_option_context_parse (context, &argc, &args, &error))
    {
      g_printf ("Could not parse command options: %s\n", error->message);
      exit (EXIT_FAILURE);
    }
  g_strfreev (args);
  g_option_context_free (context);

  if (class_names && class_names[0])
    if (!faircall_open_class (class_names[0], &error))
	{
	  g_printf ("Could not open class \"%s\". %s\n",
		    class_names[0],
		    error ? error->message : "" );
	  exit (EXIT_FAILURE);
	}
  g_strfreev (class_names);

  if (show_info)
    atexit (showinfo);

  if (names_to_add)
    for (gchar **name = names_to_add; *name; name++)
      {
	faircall_add_student (*name, &error);
	if (error)
	  {
	    g_printf ("Error adding \"%s\": %s\n",
		      *name, error->message);
	    error = NULL;
	  }
      }
  g_strfreev (names_to_add);
  if (names_to_del)
    for (gchar **name = names_to_del; *name; name++)
      {
	faircall_del_student (*name, &error);
	if (error)
	  {
	    g_printf ("Error removing \"%s\": %s\n",
		      *name, error->message);
	    error = NULL;
	  }
      }
  g_strfreev (names_to_del);

  if (count > 0)
    {
      gchar **names = faircall_call_n_students (count, &error);
      if (error)
	{
	  g_printf ("Error calling students: %s\n", error->message);
	  error = NULL;
	}

      if (!names)
	{
	  g_printf ("No students were called.\n");
	  exit (EXIT_FAILURE);
	}

      for (gchar **name = names; *name; name++)
	g_printf ("%s\n", *name);

      g_strfreev (names);
      exit (EXIT_SUCCESS);
    }

  rl_attempted_completion_function = name_completion;
  while (TRUE)
    {
      static gchar const *const call = "call";
      gchar const *const prompt = g_strdup_printf ("%s:%s> ",
						   PACKAGE,
						   faircall_get_open_class_name());
      gchar *input = readline (prompt);
      g_free ((gpointer)prompt);
      if (input && *input)
	add_history (input);

      void *buf = yy_scan_string ((input && *input) ? input : call);
      yyparse();

      yy_delete_buffer (buf);
      g_free (input);
    }
}

static void
cleanup (void)
{

  GError *error = NULL;
  faircall_close_class (&error);
  if (error)
    g_warning ("%s\n", error->message);
}

static void
showinfo (void)
{
  GError *error = NULL;
  gchar **info = faircall_info (NULL, &error);
  if (error)
    g_printf ("Error getting class info: %s\n",
	      error->message);
  error = NULL;

  if (info)
    {
      for (gchar **line = info; *line; line++)
	g_printf ("%s", *line);
    }

  g_strfreev (info);
}

static char **
name_completion (const char *text, int start, int en)
{
  rl_attempted_completion_over = 1;
  
  if (start > 1)
    symbols = faircall_list (NULL);

  char **ret = rl_completion_matches (text, name_generator);

  g_strfreev (symbols);
  symbols = NULL;
  return ret;
}

static char *
name_generator (const char *text, int state)
{
  static int list_index, len;
  char *line;

  if (!state)
    {
      list_index = 0;
      len = strlen (text);
    }

  char **list = symbols ? symbols : commands;

  while ((line = list[list_index++]))
    {
      if (strncmp (line, text, len) == 0)
	{
	  return g_strdup (line);
	}
    }

  return NULL;
}
