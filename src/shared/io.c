#include "class-priv.h"
#include "student-priv.h"
#include "error.h"

#include <config.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <glib/gstdio.h>

#define CLASS "class"
#define STUDENT "student"
#define NAME "name"
#define LAST "last_called"
#define CALLED "called_on"
#define EVEN "even"

#define MUST "must"
#define MAY "may"
#define NOT "not"

/* Local Error Domain and Codes */
#define FAIRCALL_IO_ERROR faircall_io_error_quark()

static GQuark
faircall_io_error_quark()
{
  return g_quark_from_static_string ("faircall-io-error-quark");
}

static const gchar *
faircall_io_get_filename()
{
  return g_build_filename (g_get_user_config_dir(),
			   PACKAGE,
			   NULL); 
}

static xmlDocPtr
faircall_io_open_doc (GError **error)
{
  const gchar *filename = faircall_io_get_filename();
  FILE *file = g_fopen (filename, "r+");
  if (!file)
    {
      if (errno == ENOENT)
	{
	  xmlDocPtr doc = xmlNewDoc (BAD_CAST "1.0");
	  xmlNodePtr root = xmlNewNode (NULL, BAD_CAST "root");
	  xmlDocSetRootElement (doc, root);

	  return doc;
	}
      else
	{
	  g_set_error (error,
		       FAIRCALL_IO_ERROR,
		       g_file_error_from_errno (errno),
		       "%s",
		       strerror (errno));
	  return NULL;
	}
    }
  else
    {
      fclose (file);

      LIBXML_TEST_VERSION;

      xmlDocPtr doc = xmlParseFile (filename);
      if (!doc)
	g_set_error (error,
		     FAIRCALL_IO_ERROR,
		     XML_PARSE_ERROR,
		     "Could not parse file %s.",
		     filename);

      return doc;
    }
}

static xmlNodePtr
faircall_io_get_root (xmlDocPtr _doc, GError **error)
{
  GError *tmp_error = NULL;

    xmlDocPtr doc = _doc ? _doc : faircall_io_open_doc (&tmp_error);

  if (tmp_error)
    {
      g_propagate_error (error, tmp_error);
      return NULL;
    }

  xmlNodePtr root = xmlDocGetRootElement (doc);
  if (!root)
    {
      g_set_error (error,
		   FAIRCALL_IO_ERROR,
		   XML_PARSE_ERROR,
		   "Could not find the root element in doc.");

    }

  if (!_doc && doc)
    xmlFreeDoc (doc);

  return root;
}

static xmlNodePtr
faircall_io_get_class_node (xmlDocPtr _doc, const gchar *name, GError **error)
{
  GError *tmp_error = NULL;
  xmlNodePtr root = faircall_io_get_root (_doc, &tmp_error);
  if (tmp_error)
    {
      g_propagate_error (error, tmp_error);
      return NULL;
    }

  for (xmlNodePtr cur = root->xmlChildrenNode;
       cur != NULL; cur = cur->next)
    {
      if (xmlStrcmp (cur->name, CLASS) == 0)
	if (xmlStrcmp (xmlGetProp (cur, NAME), name) == 0)
	  return cur;
    }

  return NULL;
}

xmlNodePtr
faircall_io_new_student_node (gpointer data, gpointer user_data)
{
  struct Student *student = data;
  xmlNodePtr parent = user_data;

  xmlNodePtr ret = xmlNewChild (parent, NULL, STUDENT, NULL);

  xmlNewProp (ret, NAME, student->name);

  gchar *buf = g_malloc (snprintf (0, 0, "%d", student->called_on) + 1);
  sprintf (buf, "%d", student->called_on);

  xmlNewProp (ret, CALLED, buf);

  g_free (buf);
  return ret;
}

struct Student *
faircall_io_new_student_from_node (xmlNodePtr node, GError **error)
{

  if (!node)
    {
      g_warning ("Cannot call faircall_io_new_student_from_node "
		 " null node.");
      return NULL;
    }
  if (xmlStrcmp (node->name, STUDENT) != 0)
    {
      g_warning ("This is not a student node.");
      return NULL;
    }

  GError *tmp_error = NULL;
  struct Student *ret =
    faircall_student_new (xmlGetProp (node, NAME), &tmp_error);
  if (tmp_error)
    {
      g_propagate_error (error, tmp_error);
      return NULL;
    }
  ret->called_on = (guint) atoi (xmlGetProp (node, CALLED));

  return ret;
}


gboolean
faircall_io_save_class(struct Class *class, GError **error)
{
  if (!class)
    {
      g_warning ("Cannot save a null class");
      return FALSE;
    }

  g_mutex_lock (&class->m);

  GError *tmp_error = NULL;
  xmlDocPtr doc = faircall_io_open_doc (&tmp_error);

  if (tmp_error)
    {
      g_propagate_error (error, tmp_error);
      return FALSE;
    }

  xmlNodePtr class_node = faircall_io_get_class_node (doc,
						      class->name,
						      &tmp_error);
  if (tmp_error)
    {
      g_propagate_error (error, tmp_error);
      return FALSE;
    }

  if (!class_node)
    {
      class_node = xmlNewChild (faircall_io_get_root (doc, &tmp_error),
				NULL, CLASS, NULL);
      xmlNewProp (class_node, NAME, class->name);
    }

  xmlSetProp (class_node, LAST, 
	      class->last_called ?
		class->last_called->name : "");

  xmlNodePtr cur = class_node->xmlChildrenNode;
  while (cur)
    {
      xmlNodePtr tmp = cur->next;
      xmlUnlinkNode (cur);
      xmlFreeNode (cur);
      cur = tmp;
    }

  xmlNodePtr must_call = xmlNewChild (class_node, NULL, MUST, NULL);
  xmlNodePtr may_call = xmlNewChild (class_node, NULL, MAY, NULL);
  xmlNodePtr no_call = xmlNewChild (class_node, NULL, NOT, NULL);

  g_queue_foreach (class->must_call, (GFunc)faircall_io_new_student_node, must_call);
  g_list_foreach (class->can_call, (GFunc)faircall_io_new_student_node, may_call);
  g_list_foreach (class->cant_call, (GFunc)faircall_io_new_student_node, no_call);

  xmlSaveFormatFile (faircall_io_get_filename(), doc, 0);
  xmlFreeDoc (doc);
  g_mutex_unlock (&class->m);

  return TRUE;
}

gboolean
faircall_io_is_class (gchar const *const restrict name,
		      GError **error)
{
  xmlDocPtr doc = faircall_io_open_doc(error);
  if (*error)
    return FALSE;

  gboolean ret = NULL != faircall_io_get_class_node (doc, name, error);
  xmlFreeDoc (doc);
  return ret;
}

struct Class *
faircall_io_get_class (gchar const *const restrict name,
		       GError **error)
{
  xmlDocPtr doc = NULL;
  struct Class *ret = NULL;

  if (!name)
    {
      g_warning ("Cannot call faircall_io_get_class with a null name.");
      goto cleanup;
    }

  GError *tmp_error = NULL;
  doc = faircall_io_open_doc (&tmp_error);
  if (tmp_error)
    {
      g_propagate_error (error, tmp_error);
      goto cleanup;
    }

  xmlNodePtr class_node = faircall_io_get_class_node (doc, name, &tmp_error);
  if (tmp_error)
    {
      g_propagate_error (error, tmp_error);
      xmlFreeDoc (doc);
      goto cleanup;
    }

  ret = (struct Class *) g_malloc (sizeof (struct Class));
  ret->name = g_strdup (xmlGetProp (class_node, NAME));
  ret->last_called = NULL;
  ret->last_last_called = NULL;
  ret->can_call = NULL;
  ret->can_call_length = 0;
  ret->cant_call = NULL;
  ret->must_call = g_queue_new();
  ret->forced_even = 
    (0 == xmlStrcmp (xmlGetProp (class_node, EVEN), "true"));
  
  g_mutex_init (&ret->m);

  for (xmlNodePtr cur = class_node->xmlChildrenNode;
       cur != NULL; cur = cur->next)
    {
      for (xmlNodePtr student_node = cur->xmlChildrenNode;
	   student_node != NULL; student_node = student_node->next)
	{
	  struct Student *student = 
	    faircall_io_new_student_from_node (student_node, &tmp_error);
	  if (tmp_error)
	    {
	      g_propagate_error (error, tmp_error);
	      goto cleanup;
	    }

	  if (0 == xmlStrcmp (xmlGetProp (class_node, LAST), student->name))
	    ret->last_called = student;

	  if (xmlStrcmp (cur->name, MAY) == 0)
	    ret->can_call = g_list_prepend (ret->can_call, student);
	  else if (xmlStrcmp (cur->name, NOT) == 0)
	    ret->cant_call = g_list_prepend (ret->cant_call, student);
	  else if (xmlStrcmp (cur->name, MUST) == 0)
	    g_queue_push_head (ret->must_call, student);
	  else
	    {
	      g_set_error (error,
			   FAIRCALL_IO_ERROR,
			   XML_PARSE_ERROR,
			   "A child of the class not has an invalid name %s.",
			   cur->name);
	      goto cleanup;
	    }
	}
    }

cleanup:
  if (doc)
    xmlFreeDoc (doc);
  return ret;
}
