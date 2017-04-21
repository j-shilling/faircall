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
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include <argp.h>

#include <sys/stat.h>

#include <libxml/parser.h>

#include "io.h"
#include "list.h"
#include "strdup.h"

#include "../config.h"

/************************************************************************
 * GLOBAL VARIABLES                                                     *
 ************************************************************************/
static List *list = NULL;
static int exit_status = EXIT_SUCCESS;

static struct {
  int show_help;
  int show_version;
  int machine_readable;

  int run_count;

  char *class;

  char **names_to_add;
  char *stats_to_print;
} args = {
    .show_help = 0,
    .show_version = 0,
    .machine_readable = 0,

    .run_count = -1,

    .class = NULL,
    .names_to_add = NULL,
    .stats_to_print = NULL
};

/************************************************************************
 * STATIC FUNCTIONS                                                     *
 ************************************************************************/

static void
queue_name_to_add (char *name);
static void
on_kill (int sig);
static List *
get_list ();

static void
print_stats (char *name, unsigned int index, bool is_last_called,
	     unsigned int called, unsigned int slots, void *data);

/************************************************************************
 * CMD LINE OPTIONS                                                     *
 ************************************************************************/

const char *argp_program_version = PACKAGE_STRING;
const char *argp_program_bug_address = PACKAGE_BUGREPORT;

static const char doc[] =
    "faircall -- call on students fairly";
static const char args_doc[] =
    "[class name]";

static struct argp_option options[] =
      {
	{ "machine", 'm', 0, 0, "Produce machine readable output" },
	{ "count", 'c', "int", 0, "Call a certain number of names" },
	{ "add", 'a', "name", 0,
	    "Add a student to [class name]. Create the class if necessary" },
	    { "import", 'f', "path/to/file", 0,
		"Add all names in a file to [class name]. Create the class if necessary" },
	    { "info", 'i', "name", 0,
		"Show info for [name] in [class name]. If no name is given, show information for everyone in [class name]" },
	    { "roster", 'r', "path/to/file", 0,
		"Use the given file as the class roster" },
	    { 0 } };

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  switch (key) {
    case 'm':
      args.machine_readable = 1;
      break;

    case 'c':
      args.run_count = arg ? atoi (arg) : -1;
      break;

    case 'r':
      io_set_filename (arg);
      break;

    case 'a':
      {
	queue_name_to_add (arg);
      }

      args.run_count = 0;
      break;

    case 'i':
      {
	args.stats_to_print = arg;
      }

      args.run_count = 0;
      break;

    case 'f':
      {
	FILE *file = fopen (arg, "r");
	char line[256];

	if (!file)
	  {
	    fprintf (stderr, "WARNING: Could not open \"%s\" -- %s\n",
		     arg, strerror(errno));
	  }
	else
	  {
	    while (fgets (line, sizeof(line), file))
	      {
		for (int i = 0; i < strlen (line); i++)
		  {
		    if (line[i] == '\n')
		      {
			line[i] = '\0';
			break;
		      }
		  }

		queue_name_to_add (line);
	      }

	    fclose (file);
	  }
      }

      args.run_count = 0;
      break;

    case ARGP_KEY_NO_ARGS:
      if (args.stats_to_print == NULL)
	argp_usage (state);

      break;

    case ARGP_KEY_ARG:
      args.class = arg;
      state->next = state->argc;
      break;

    default:
      return ARGP_ERR_UNKNOWN;
  }

  return 0;
}


static struct argp argp = {
    options, parse_opt, args_doc, doc
};

/************************************************************************
 * MAIN FUNCTION                                                        *
 ************************************************************************/
int
main (int argc, char** argv)
{

  argp_parse (&argp, argc, argv, 0, 0, 0);

  if (args.names_to_add)
      {
        int i = 0;
        while (args.names_to_add[i])
  	{
  	  list_add (get_list(), args.names_to_add[i], 0, 1);
  	  i++;
  	}

        io_save_list (get_list());
      }

    if (args.stats_to_print)
      {
        if (args.class)
  	{
  	  list_for_each (get_list(), print_stats, args.stats_to_print);
  	}
        else
  	{
  	  args.class = args.stats_to_print;
  	  list_for_each (get_list(), print_stats, NULL);
  	}
      }

  if (args.run_count == 0)
    exit (exit_status);

  signal (SIGTERM, on_kill);
  signal (SIGINT, on_kill);
  signal (SIGQUIT, on_kill);
  signal (SIGHUP, on_kill);

  int count = 0;

  while (1)
    {
      if (args.run_count < 0)
	{
	  printf ("Press enter: ");
	  getchar ();
	}

      char *name = list_call_next (get_list ());

      if (name == NULL)
	{
	  fprintf (stderr, "ERROR: list did not return a student.\n");
	  return EXIT_FAILURE;
	}

      printf ("\n%s\n\n", name);

      count++;

      if ((count >= args.run_count) && (args.run_count > 0))
	{
	  on_kill (0);
	}

    }

  return exit_status;
}

/************************************************************************
 * OTHER FUNCTIONS                                                      *
 ************************************************************************/

static void
on_kill (int sig)
{
  io_save_list (get_list ());
  exit (exit_status);
}

static List *
get_list ()
{
  if (!list)
    {
      if (!args.class)
	{
	  fprintf (stderr, "ERROR: no class name provided\n");
	  exit (EXIT_FAILURE);
	}

      list = io_load_list (args.class);

      if (!list)
	{
	  list = list_new (args.class);
	}
    }

  return list;
}

static void
queue_name_to_add (char *name)
{
  if (!args.names_to_add) {
      args.names_to_add = (char **) malloc (sizeof (char *));
      args.names_to_add[0] = 0;
  }

  int length = 0;
  while (args.names_to_add[length])
    {
      length++;
    }

  char ** new = (char **) malloc (sizeof (char *) * (length + 2));

  for (int i = 0; i < length; i++) {
      new[i] = args.names_to_add[i];
  }

  new[length] = strdup(name);
  new[length+1] = 0;

  free (args.names_to_add);
  args.names_to_add = new;
}

static void
print_stats (char *name, unsigned int index, bool is_last_called,
	     unsigned int called, unsigned int slots, void *data)
{
  double odds = list_get_odds (get_list (), index);
  char *that = (char *) data;

  if (data)
    if (strcmp (name, that) != 0)
      return;

  if (args.machine_readable)
    {
      printf ("%s:%d:%f\n", name, called, odds);
    }
  else
    {
      printf ("%s\ncalled: %d\nodds: %3.2f%%\n", name, called, 100 * odds);
    }
}
