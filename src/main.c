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

#include "student.h"
#include "list.h"

#include "config.h"

/************************************************************************
 * OPTIONAL DEFINITIONS OF NON-POSIX FUCTIONS                           *                                                       *
 ************************************************************************/

#ifdef  HAVE_ASPRINTF

extern int asprintf (char ** restrict  __ptr,
	       	     const char * restrict  __fmt, ...);

#else  /* don't have asprintf */

#include <stdarg.h>

int
asprintf (char ** restrict __ptr,
          const char * restrict __fmt, ...)
{
  int size = 0;
  va_list args;

  va_start (args, __fmt);

  size = vsnprintf (NULL, size, __fmt, args);

  if (size < 0) { return -1; }

  *__ptr = (char *) malloc (size + 1);

  if (__ptr == NULL) { return -1; }

  size = vsprintf (*__ptr, __fmt, args);

  va_end (args);
  return size;
}

#endif /* HAVE_ASPRINTF */

#ifdef  HAVE_STRDUP

extern char *strdup (const char *str);

#else  /* don't have strdup */

char *
strdup (const char *str)
{
  char *ret = NULL;

  if (asprintf (&ret, "%s", str) == -1)
    return NULL;

  return ret;
}

#endif  /* HAVE_STRDUP */

/************************************************************************
 * MACROS FOR STRING CONSTANTS                                          *
 ************************************************************************/
#define ARGSTR    "?1mxa:f:i:d:"

#define DOCNAME   "roster"

#define CLASS		"class"
#define TYPE      "student"
#define NAME      "name"
#define CALLED    "called"
#define INDEX     "index"

/************************************************************************
 * FAIL IF THE USER HASN'T GIVEN A CLASS NAME                           *
 ************************************************************************/
#define NEEDS_CLASS_NAME                                      \
  do                                                          \
    {                                                         \
    if (!class)                                               \
      {                                                       \
        fprintf (stderr, "ERROR: no class name provided\n");  \
        exit (EXIT_FAILURE);                                  \
      }                                                       \
    }                                                         \
  while (0)

/************************************************************************
 * EXTERNAL VARIABLES                                                   *
 ************************************************************************/
extern int errno;

extern int optind;
extern char *optarg;

/************************************************************************
 * STATIC FUNCTIONS                                                     *
 ************************************************************************/
/*/ create a new student in doc
static int add_student (char * name, char * class);
// save new data to doc
static void on_kill (int sig);
// open the doc
static xmlDocPtr open_doc();
// get the node for the current class
static xmlNodePtr get_class (xmlDocPtr doc);
// get the root of the doc
static xmlNodePtr get_root (xmlDocPtr doc);
// get student from xmlNodePtr
static student_t *parse_student (xmlNodePtr node);
// print student info
static void print_stats (student_t *cur);
// get path to doc
static char *get_doc_name ();
// read the doc and build list
static list_t *get_list();
// show usage instructions
static void usage();*/

/************************************************************************
 * GLOBAL VARIABLES                                                     *
 ************************************************************************/
char *file = NULL;
List *list = NULL;
char *class = NULL;

unsigned int last_index = 0;

bool machine = false;

/************************************************************************
 * MAIN FUNCTION                                                        *
 ************************************************************************/
int
main (int argc, char** argv)
{

 /* bool run = true;
  bool loop = true;


  int c;
  while ( (c = getopt (argc, argv, ARGSTR)) != -1 )
    {
      switch (c)
      {
        case 'm':
          machine = true;
          continue;

        case 'd':
          file = strdup (optarg);
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

  optind = 0;


  while ( (c = getopt(argc, argv, ARGSTR)) != -1 )
    {
      switch (c)
      {
      case 'f':
        {
          NEEDS_CLASS_NAME;

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
                add_student (line, class);
              }
            fclose(file);
          }
        }

        run = false;
        continue;

      case 'a':
        {
          NEEDS_CLASS_NAME;

          add_student (optarg, class);
        }

        run = false;
        continue;

      case 'i':
        {
          bool all = (class == NULL);

          if (all)
            class = optarg;

          xmlDocPtr doc = open_doc();
          xmlNodePtr class_node = get_class(doc);

          char *li = xmlGetProp (class_node, CALLED);
          last_index = atoi (li);

          if (!all)
            {
              xmlNodePtr cur = class_node->xmlChildrenNode;
              while (cur != NULL)
                {
                  if (!xmlStrcmp (cur->name, TYPE))
                    {
                      if (!xmlStrcmp (optarg, xmlGetProp(cur, NAME)))
                        {
                          student_t *student = parse_student (cur);
                          print_stats (student);
                          free_student (student);
                        }

                    }

                  cur = cur->next;
                }
            }
          else
            {
              xmlNodePtr cur = class_node->xmlChildrenNode;
              while (cur != NULL)
                {
                  if (!xmlStrcmp (cur->name, TYPE))
                    {
                      student_t *student = parse_student (cur);
                      print_stats (student);
                      free_student (student);
                    }

                    cur = cur->next;
                }
            }

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
      NEEDS_CLASS_NAME;



      signal (SIGTERM, on_kill);
      signal (SIGINT, on_kill);
      signal (SIGQUIT, on_kill);
      signal (SIGHUP, on_kill);



      xmlDocPtr doc = open_doc();
      xmlNodePtr class_node = get_class (doc);

      if (class_node == NULL)
        {
          fprintf (stderr, "ERROR: no such class \'%s\'\n", class);
          return EXIT_FAILURE;
        }


      list = get_list();

      if (!list)
        {
          fprintf (stderr, "ERROR: class \'%s\' is empty\n", class);
          return EXIT_FAILURE;
        }


      student_t *student = NULL;
      do
        {
          if (loop)
            {
              printf ("Press enter: ");
              getchar();
            }
      
          last_index = call_student (list, last_index, &student);

          if (student == NULL)
            {
              fprintf (stderr, "ERROR: index %d did not return a student.\n", last_index);
              return EXIT_FAILURE;
            }

          printf ("\n%s\n\n", student->name);

        }
      while (loop);

      on_kill(0);
    }

  return EXIT_SUCCESS;*/

  printf("Hello World\n");
}

/************************************************************************
 * OTHER FUNCTIONS                                                      *
 ************************************************************************/
/*
static int
add_student (char * name, char *class)
{
  for (int i = 0; i < strlen (name); i++)
    {
      if (name[i] == '\n')
        {
          name[i] = '\0';
          break;
        }
    }

  xmlDocPtr doc = open_doc();
*/
  /*
   * Create doc and root
   */
/*
  if (doc == NULL)
    {
      doc = xmlNewDoc (BAD_CAST "1.0");
      xmlNodePtr root = xmlNewNode (NULL, BAD_CAST "root");  
      xmlDocSetRootElement (doc, root);
    }/*

  /*
   * Find or create class
   */
 /*
  xmlNodePtr class_node = get_class(doc);

  if (class_node == NULL)
    {
      class_node = xmlNewChild (get_root (doc), NULL, CLASS, NULL);
      xmlNewProp (class_node, NAME, class);
      xmlNewProp (class_node, CALLED, "0");
    }
*/
  /*
   * Check if student already exists in class
   */
 /* xmlNodePtr cur = class_node->xmlChildrenNode;
  student_t *last = NULL;

  while (cur != NULL)
    {
      if (!xmlStrcmp (cur->name, TYPE))
        {
          last = parse_student (cur);

          if (!xmlStrcmp (cur->name, name))
            {
              fprintf (stderr, "WARNING: class %s already has a student named %s\n", class, name);
              return EXIT_FAILURE;
            }

	  free_student (last);
        }

        cur = cur->next;
    }
*/
  /*
   * Add student to class
   */
/*
  student_t *student = new_student (name);
  xmlNodePtr node = xmlNewChild (class_node, NULL, TYPE, NULL);

  xmlNewProp (node, NAME, name);
  char buf[256];
  sprintf (buf, "%d", last ? (last->max_index + 1) : 0);
  xmlNewProp(node, INDEX, buf);
  xmlNewProp(node, CALLED, "0"); 

  free_student (student);
  xmlSaveFormatFile (get_doc_name(), doc, 0);
  xmlFreeDoc (doc);

  return EXIT_SUCCESS;
}

static void
on_kill (int sig)
{/*
  /*
   * Update saved info
   */
/*  xmlDocPtr doc = open_doc();
  list_node_t *l = get_list();

  while (l != NULL)
    {
      student_t *val = list_get_item(l);

      xmlNodePtr class_node = get_class (doc);

      char buf[256];
      sprintf (buf, "%i", last_index);
      xmlSetProp (class_node, CALLED, buf);

      xmlNodePtr cur = get_class(doc)->xmlChildrenNode;
      while (cur != NULL)
        {
          if (!xmlStrcmp (cur->name, TYPE))
            {
              if (!xmlStrcmp (xmlGetProp (cur, NAME), val->name))
                {
                  char buf[256];
                  sprintf(buf, "%d", val->times_called_on);
                  xmlSetProp (cur, CALLED, buf);
                  sprintf(buf, "%d", val->max_index);
                  xmlSetProp (cur, INDEX, buf);
                  break;
                }
            }

          cur = cur->next;
        }

        l = list_get_item(l);
    }

  free_list_node (l);
  xmlSaveFormatFile (get_doc_name(), doc, 0);
  xmlFreeDoc (doc);
  exit (EXIT_SUCCESS);
}

static xmlDocPtr
open_doc()
{

  if (access (get_doc_name(), F_OK) != 0)
    {
      return NULL;
    }
  else
    {
      LIBXML_TEST_VERSION;

      xmlDocPtr doc = xmlParseFile (get_doc_name());

      if (doc == NULL)
        {
          fprintf (stderr, "Failed to parse xml document: %s\n", get_doc_name());
          exit (EXIT_FAILURE);
        }
    
      return doc;
    }
}

static xmlNodePtr
get_class (xmlDocPtr doc)
{
  xmlNodePtr cur = get_root(doc)->xmlChildrenNode;

  while (cur != NULL)
    {
      if (!xmlStrcmp (cur->name, CLASS))
	if (!xmlStrcmp (class, xmlGetProp(cur, NAME)))
	  return cur;

      cur = cur->next;
    }

    return NULL;
}

static xmlNodePtr
get_root (xmlDocPtr doc)
{
  xmlNodePtr root = xmlDocGetRootElement(doc);

  if (root == NULL)
    {
      fprintf (stderr, "Failed to get the root element of xml document: %s\n", get_doc_name());
      xmlFreeDoc(doc);
      exit (EXIT_FAILURE);
    }

  return root;
}

static student_t *
parse_student (xmlNodePtr node)
{
  student_t *student = new_student (xmlGetProp (node, NAME));

  student->times_called_on = atoi (xmlGetProp (node, CALLED));

  student->max_index = atoi (xmlGetProp (node, INDEX));

  return student;
}

static void
print_stats (student_t *cur)
{
  double odds = (100 * get_odds (get_list(), cur, last_index));

  if (machine)
    {
      printf ("%s:%i:%3.2f\n", cur->name, cur->times_called_on, odds);
    }
  else
    {
      printf ("name: %s\n", cur->name);
      printf ("times called on: %d\n", cur->times_called_on);
      printf ("odds of getting called next: %3.2f %%\n", odds);
    }

}

static char *
get_doc_name ()
{
  if (!file)
    {
      char *dir = NULL;
      if (asprintf (&dir, "%s/.%s", getenv ("HOME"), PACKAGE_NAME) == -1)
        {
          fprintf (stderr,
            "ERROR: could not allocate space for the dir name.\n");
            exit (EXIT_FAILURE);
        }

      if (mkdir (dir, S_IRWXU) != 0)
        {
          if (errno != EEXIST)
            {
              fprintf (stderr, "ERROR: Could not create dir %s\n%s\n",
                dir, strerror (errno));
              exit (EXIT_FAILURE);
            }
        }

      if (asprintf (&file, "%s/%s", dir, DOCNAME) == -1)
        {
          fprintf (stderr,
            "ERROR: Could not allocate memmory to the file name.\n");
          exit(EXIT_FAILURE);
        }
    }

  return file;
}

static list_t *
get_list () {
  if (!list)
    {
      xmlDocPtr doc = open_doc();
      xmlNodePtr class_node = get_class(doc);

      xmlNodePtr cur = class_node->xmlChildrenNode;
      while (cur != NULL)
        {
          if (!xmlStrcmp (cur->name, TYPE))
            {
              student_t *student = parse_student (cur);
              int i = student->max_index;

              if (!list)
                list = new_list_node (student);
              else
                add (list, student);

              if (i != 0)
                student->max_index = i;
            }

          cur = cur->next;
        }

        last_index = atoi(xmlGetProp (class_node, CALLED));

        xmlFreeDoc (doc);
    }

  return list;

}

static void
usage()
{
  printf ("%s [mode] [class]\n", PACKAGE);
}*/
