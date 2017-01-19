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
#include <errno.h>

#include <unistd.h>

#include <sys/stat.h>

#include <libxml/parser.h>

#include "io.h"
#include "io-priv.h"
#include "asprintf.h"
#include "strdup.h"
#include "class.h"
#include "student.h"
#include "list.h"

/************************************************************************
 * FILE ACCESS FUNCTIONS                                                *
 ************************************************************************/

io_err
io_set_filename (char *name)
{
  if (file)
    free (file);

  file = NULL;

  if (access (name, F_OK) != 0)
      {
        if (errno != ENOENT)
  	{
  	     return ACCESS_FAILED;
  	    }
      }

  file = strdup (name);
  return SUCCESS;
}

io_err
io_save_list_item (List *list, unsigned int index)
{
  xmlDocPtr doc = open_doc();

  if (!doc)
    {
      return errno == ACCESS_FAILED ? ACCESS_FAILED : PARSE_FAILED;
    }

  xmlNodePtr root = get_root(doc);

  if (!root)
    {
      return ROOT_FAILED;
    }

  char *class_name = list_get_class_name (list);
  xmlNodePtr class_node = get_class (doc, class_name);

  if (!class_node)
    {
      class_node = xmlNewChild (get_root (doc), NULL, CLASS, NULL);
      xmlNewProp (class_node, NAME, class_name);
      xmlNewProp (class_node, CALLED, "0");
    }

  char *last_called = NULL;
  asprintf (&last_called, "%d", list_get_last_called (list));
  xmlSetProp (class_node, CALLED, last_called);

  save_item (list_get_name (list, index), index, FALSE,
	     list_get_times_called_on (list, index),
	     list_get_slots (list, index),
	     class_node);

  xmlSaveFormatFile (get_doc_name(), doc, 0);
  xmlFreeDoc (doc);

  return SUCCESS;
}

io_err
io_save_list (List *list)
{
  xmlDocPtr doc = open_doc();

  if (!doc)
    {
      return errno == ACCESS_FAILED ? ACCESS_FAILED : PARSE_FAILED;
    }

  xmlNodePtr root = get_root(doc);

  if (!root)
    {
      return ROOT_FAILED;
    }

  char *class_name = list_get_class_name (list);
  xmlNodePtr class_node = get_class (doc, class_name);

  if (!class_node)
    {
      class_node = xmlNewChild (get_root (doc), NULL, CLASS, NULL);
      xmlNewProp (class_node, NAME, class_name);
      xmlNewProp (class_node, CALLED, "0");
    }

  char *last_called = NULL;
  asprintf (&last_called, "%d", list_get_last_called (list));
  xmlSetProp (class_node, CALLED, last_called);

  list_for_each (list, save_item, class_node);

  xmlSaveFormatFile (get_doc_name(), doc, 0);
  xmlFreeDoc (doc);

  return SUCCESS;
}

List *
io_load_list (char *class_name)
{
  List *ret = NULL;

  xmlDocPtr doc = open_doc();
  if (doc)
  {
    xmlNodePtr class_node = get_class (doc, class_name);
    if (class_node)
      {
	ret = list_new (class_name);

	xmlNodePtr cur = class_node->xmlChildrenNode;
	while (cur)
	  {
	    if (!xmlStrcmp (cur->name, STUDENT))
	      {
		list_add (ret,
			  xmlGetProp (cur, NAME),
			  atoi (xmlGetProp (cur, CALLED)),
			  atoi (xmlGetProp (cur, SLOTS)));
	      }

	    cur = cur->next;
	  }
      }
  }

  return ret;
}

/************************************************************************
 * STATIC FUNCTIONS DEFINITIONS                                         *
 ************************************************************************/

static void
save_item (char *name, unsigned int index, bool is_last_called,
			unsigned int called, unsigned int slots,
			void *data)
{
  xmlNodePtr class_node = (xmlNodePtr) data;

  xmlNodePtr cur = class_node->xmlChildrenNode;
  xmlNodePtr item_node = NULL;
  while (cur)
    {
      if (!xmlStrcmp (cur->name, STUDENT))
	if (!xmlStrcmp (name, xmlGetProp (cur, NAME)))
	  {
	    item_node = cur;
	    break;
	  }

      cur = cur->next;
    }

  if (!item_node)
    {
      item_node = xmlNewChild (class_node, NULL, STUDENT, NULL);
      xmlNewProp (item_node, NAME, name);
      xmlNewProp (item_node, SLOTS, "0");
      xmlNewProp (item_node, CALLED, "0");
    }

  char *item_slots = NULL;
  asprintf (&item_slots, "%d", slots);
  char *item_called = NULL;
  asprintf (&item_called, "%d", called);

  xmlSetProp (item_node, SLOTS, item_slots);
  xmlSetProp (item_node, CALLED, item_called);
}

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
  xmlDocPtr doc = NULL;

  if (access (get_doc_name (), F_OK) != 0)
    {
      if (errno == ENOENT)
	{
	      doc = xmlNewDoc (BAD_CAST "1.0");
	      xmlNodePtr root = xmlNewNode (NULL, BAD_CAST "root");
	      xmlDocSetRootElement (doc, root);

	      return doc;
	    }
      else
	{

      errno = ACCESS_FAILED;
	}
    }
  else
    {
      LIBXML_TEST_VERSION;

      doc = xmlParseFile (get_doc_name ());

      if (doc == NULL)
	{
	  errno = PARSE_FAILED;
	}
    }

  return doc;
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

List *
get_student_list (char *class)
{
  List *ret = list_new (class);

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



#ifdef __TEST_SAVE_LIST_ITEM


int
main (int argc, char **argv)
{
  io_set_filename ("test.xml");
  List *list = list_new ("class1");

  list_add (list, argv[1], atoi (argv[2]), atoi (argv[3]));
  io_err ret = io_save_list_item (list, 0);

  if (ret != SUCCESS)
    fprintf (stderr, "io_save_list_item retured %d\n", ret);

  return ret == SUCCESS;
}


#endif

#ifdef __TEST_LOAD_LIST

static void
print_info (char *name, unsigned int index, bool is_last_called,
			unsigned int called, unsigned int slots,
			void *data)
{
  printf ("%s: %d %d\n", name, called, slots);
}

int
main (int argc, char **argv)
{
  io_set_filename (argv[1]);

  List *list = io_load_list (argv[2]);
  list_for_each (list, print_info, NULL);
}
#endif
