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

#include <sys/stat.h>

#include <libxml/parser.h>

#include "io.h"
#include "list.h"
#include "strdup.h"

#include "config.h"

/************************************************************************
 * MACROS FOR STRING CONSTANTS                                          *
 ************************************************************************/
#define ARGSTR    "?1mxa:f:i:d:"

/************************************************************************
 * EXTERNAL VARIABLES                                                   *
 ************************************************************************/
extern int errno;

extern int optind;
extern char *optarg;

/************************************************************************
 * STATIC FUNCTIONS                                                     *
 ************************************************************************/

static void on_kill (int sig);
static List *get_list();
static void usage();

static void print_stats (char *name, unsigned int index, bool is_last_called,
				unsigned int called, unsigned int slots,
				void *data);

/************************************************************************
 * GLOBAL VARIABLES                                                     *
 ************************************************************************/
List *list = NULL;
char *class = NULL;

bool machine = false;

/************************************************************************
 * MAIN FUNCTION                                                        *
 ************************************************************************/
int
main (int argc, char** argv)
{

  bool run = true;
  bool loop = true;

  /*
   * Check for args which need ot be processed first
   */
  int c;
  while ( (c = getopt (argc, argv, ARGSTR)) != -1 )
    {
      switch (c)
      {
        case 'm':
          machine = true;
          continue;

        case 'd':
          io_set_filename (optarg);
          continue;

        case '1':
          loop = false;
          continue;

        default:
          continue;
      }
    } 
  
  if (optind < argc)
    {
      class = strdup(argv[optind]);
    }

  // Reset getopt
  optind = 0;

  /*
   * Check for other args
   */
  while ( (c = getopt(argc, argv, ARGSTR)) != -1 )
    {
      switch (c)
      {
      case 'f':
        {
          FILE *file = fopen(optarg, "r");
          char line[256];

          if (!file)
            {
              fprintf(stderr, "WARNING: Could not open \"%s\" -- %s\n",
                  optarg, strerror(errno));
            }
          else
          {
            while (fgets(line, sizeof(line), file))
              {
        	for (int i = 0; i < strlen (line); i++)
        	    {
        	      if (line[i] == '\n')
        	        {
        		  line[i] = '\0';
        	          break;
        	        }
        	    }

        	list_add (get_list(), line, 0, 1);
        	io_save_list (get_list());
              }
            fclose(file);
          }
        }

        run = false;
        continue;

      case 'a':
        {
          list_add (get_list(), optarg, 0, 1);
          io_save_list(get_list());
        }

        run = false;
        continue;

      case 'i':
	  if (class)
	    {
	      char *name = strdup(optarg);
	      list_for_each (get_list(), print_stats, name);
	      free (name);
	    }
	  else
	    {
	      class = strdup(optarg);

	      list_for_each (get_list(), print_stats, NULL);
	    }

          run = false;
          continue;

      case '?':
        usage();
        return EXIT_SUCCESS;

      case '1':
        continue;

      case 'm':
        continue;

      case 'd':
        continue;

      default:
	usage();
        return EXIT_FAILURE;
    }
  }


  if (run)
    {
      signal (SIGTERM, on_kill);
      signal (SIGINT, on_kill);
      signal (SIGQUIT, on_kill);
      signal (SIGHUP, on_kill);

      do
        {
          if (loop)
            {
              printf ("Press enter: ");
              getchar();
            }
      
          char *name = list_call_next (get_list());

          if (name == NULL)
            {
              fprintf (stderr, "ERROR: list did not return a student.\n");
              return EXIT_FAILURE;
            }

          printf ("\n%s\n\n", name);

        }
      while (loop);

      on_kill(0);
    }

  return EXIT_SUCCESS;
}

/************************************************************************
 * OTHER FUNCTIONS                                                      *
 ************************************************************************/

static void
on_kill (int sig)
{
  io_save_list (get_list());
  exit (EXIT_SUCCESS);
}

static List *
get_list () {
  if (!list)
    {
      if (!class)
            {
              fprintf (stderr, "ERROR: no class name provided\n");
              exit (EXIT_FAILURE);
            }

      list = io_load_list(class);

      if (!list)
	{
	  list = list_new(class);
	}
    }

  return list;
}

static void
print_stats (char *name, unsigned int index, bool is_last_called,
				unsigned int called, unsigned int slots,
				void *data)
{
  double odds = list_get_odds (get_list(), index);
  char *that = (char *) data;

  if (data)
    if (strcmp (name, that) != 0)
      return;

  if (machine)
    {
      printf ("%s:%d:%f\n", name, called, odds);
    }
  else
    {
      printf ("%s\ncalled: %d\nodds: %3.2f%%\n",
	      name, called, 100 * odds);
    }
}

static void
usage()
{
  printf ("%s [mode] [class]\n", PACKAGE);
}
