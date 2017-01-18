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

#include <stdlib.h>
#include <errno.h>

#include <unistd.h>

#include <sys/stat.h>

#include <libxml/parser.h>

#include "io.h"
#include "asprintf.h"
#include "class.h"
#include "student.h"
#include "list.h"

#define CLASS	"class"
#define STUDENT "student"
#define NAME    "name"
#define CALLED  "called"
#define INDEX   "index"
#define SLOTS   "index"

extern int errno;

char *file = NULL;
const char *DOCNAME = "roster";

static char *
get_doc_name ()
{
  if (!file)
    {
      char *dir = NULL;
      if (asprintf (&dir, "%s/.%s", getenv ("HOME"), PACKAGE_NAME) == -1)
	{
	  errno = ASPRINTF_FAILED;
	  return NULL;
	}

      if (mkdir (dir, S_IRWXU) != 0)
	{
	  if (errno != EEXIST)
	    {
	      errno = MKDIR_FAILED;
	      return NULL;
	    }
	}

      if (asprintf (&file, "%s/%s", dir, DOCNAME) == -1)
	{
	  errno = ASPRINTF_FAILED;
	  return NULL;
	}
    }

  return file;
}

static xmlDocPtr
open_doc ()
{

  if (access (get_doc_name (), F_OK) != 0)
    {
      errno = ACCESS_FAILED;
      return NULL;
    }
  else
    {
      LIBXML_TEST_VERSION
      ;

      xmlDocPtr doc = xmlParseFile (get_doc_name ());

      if (doc == NULL)
	{
	  errno = PARSE_FAILED;
	  return NULL;
	}

      return doc;
    }
}

static xmlNodePtr
get_root (xmlDocPtr doc)
{
  xmlNodePtr root = xmlDocGetRootElement (doc);

  if (root == NULL)
    {
      errno = ROOT_FAILED;
      return NULL;
    }

  return root;
}

static xmlNodePtr
get_class (xmlDocPtr doc, char *class)
{
  xmlNodePtr cur = get_root (doc)->xmlChildrenNode;

  while (cur != NULL)
    {
      if (!xmlStrcmp (cur->name, CLASS))
	if (!xmlStrcmp (class, xmlGetProp(cur, NAME)))
	  return cur;

      cur = cur->next;
    }

  return NULL;
}

static student_t *
parse_student (xmlNodePtr node)
{
  student_t *student = new_student (xmlGetProp (node, NAME));

  student->times_called_on = atoi (xmlGetProp (node, CALLED));

  student->max_index = atoi (xmlGetProp (node, INDEX));

  return student;
}

static class_t *
parse_class (xmlNodePtr node)
{
  char *name = xmlGetProp (node, NAME);
  unsigned int index = atoi (xmlGetProp (node, CALLED));
  unsigned int size = 0;
  char *last = NULL;

  xmlNodePtr cur = node->xmlChildrenNode;
  while (cur != NULL)
    {
      if (!xmlStrcmp (cur->name, STUDENT))
	{
	  size++;
	  unsigned int i = atoi (xmlGetProp (cur, INDEX));
	  if (i == index)
	    last = xmlGetProp (cur, NAME);

	  cur = cur->next;
	}
    }

  return new_class (name, size, last, index);
}

void
set_file (char *name)
{
  if (file)
    free (file);
  file = name;
}

class_t **
get_class_list ()
{
  xmlDocPtr doc = open_doc ();
  if (!doc)
    return NULL;

  xmlNodePtr root = get_root (doc);
  if (!root)
    return NULL;

  int i = 0;

  xmlNodePtr cur = root->xmlChildrenNode;
  while (cur != NULL)
    {
      if (!xmlStrcmp (cur->name, CLASS))
	i++;

      cur = cur->next;
    }

  class_t **ret = (class_t **) malloc (sizeof(class_t *) * (i+1));
  i = 0;

  cur = root->xmlChildrenNode;
  while (cur != NULL)
    {
      if (!xmlStrcmp (cur->name, CLASS))
	{
	  ret[i] = parse_class (cur);
	  i++;
	}

      cur = cur->next;
    }

  ret[i] = NULL;

  xmlFreeDoc (doc);

  return ret;
}

list_t *
get_student_list (char *class)
{
  list_t *ret = list_new (class);

  xmlDocPtr doc = open_doc ();
    if (!doc)
      return NULL;

  xmlNodePtr class_node = get_class (doc, class);

  if (!class_node)
    {
      errno = NOSUCHCLASS;
      return NULL;
    }

  xmlNodePtr cur = class_node->xmlChildrenNode;
  while (cur != NULL)
    {
      if (!xmlStrcmp (cur->name, STUDENT))
	{
	  list_add (ret, xmlGetProp(cur, NAME),
		    atoi (xmlGetProp(cur, CALLED)),
		    atoi (xmlGetProp(cur, SLOTS)));
	}

      cur = cur->next;
    }

  xmlFreeDoc (doc);

  return ret;
}
