#include "class.r"
#include "student.r"
#include "student-priv.h"
#include "roster.r"

#include "error.h"

#include <config.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <glib/gstdio.h>

#define LAST "last_called"
#define EVEN "even"

/* Local Error Domain and Codes */
#define FAIRCALL_IO_ERROR faircall_io_error_quark()

/* XML name constants */

static gchar const *const restrict STUDENT = "student";
static gchar const *const restrict NAME = "name";
static gchar const *const restrict CALLED = "called";

static gchar const *const restrict CLASS = "class";
static gchar const *const restrict LAST = "last_called";
static gchar const *const restrict EVEN = "forced_even";

static gchar const *filename = NULL;

static GQuark
faircall_io_error_quark()
{
  return g_quark_from_static_string ("faircall-io-error-quark");
}

static void
cleanup (void)
{
  g_free ((gpointer) filename);
}

static const gchar *
faircall_io_get_filename()
{
  if (!filename)
    {
      filename = g_build_filename (g_get_user_config_dir(),
				    PACKAGE,
				    NULL); 
      atexit (cleanup);
    }

  return filename;
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
		       g_strerror (errno));
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
faircall_io_get_root (xmlDocPtr doc, GError **error)
{
  if (!doc)
    {
      g_warning ("There is not root node of a null doc.");
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

  return root;
}

static void
faircall_io_save_student (xmlNodePtr const restrict parent,
			  struct Student const *const restrict student)
{
  xmlNodePtr node = NULL;
  for (xmlNodePtr cur = parent->xmlChildrenNode;
       cur; cur = cur->next)
    {
      if (xmlStrcmp (cur->name, STUDENT) == 0)
	{
	  if (xmlStrcmp (student->name, xmlGetProp (cur, NAME)) == 0)
	    {
	      node = cur;
	      break;
	    }
	}
    }

  if (node == NULL)
    {
      node = xmlNewChild (parent, NULL, STUDENT, NULL);
      xmlNewProp (node, NAME, student->name);

      gchar *called_on = g_strdup_printf ("%u", student->called_on);
      xmlNewProp (node, CALLED,
		  (!called_on || !(*called_on)) ? "0" : called_on);
      g_free (called_on);
    }
  else
    {
      gchar *called_on = g_strdup_printf ("%u", student->called_on);
      xmlSetProp (node, CALLED,
		  (!called_on || !(*called_on)) ? "0" : called_on);
      g_free (called_on);
    }
}

static struct Student *
faircall_io_get_student (xmlNodePtr const restrict node,
			 GError **error)
{
  if (!node)
    {
      g_warning ("Cannot call faircall_io_get_student on null node.");
      return NULL;
    }
  if (xmlStrcmp (node->name, STUDENT) != 0)
    {
      g_warning ("Cannot call faircall_io_get_student on a non-student node.");
      return NULL;
    }

  xmlChar *name = xmlGetProp (node, NAME);
  if (!name)
    {
      g_set_error (error,
		   FAIRCALL_IO_ERROR,
		   XML_PARSE_ERROR,
		   "This student node does not have a name.");
      return NULL;
    }

  GError *tmp_error = NULL;
  struct Student *ret = faircall_student_new (name, &tmp_error);
  if (tmp_error)
    {
      g_propagate_error (error, tmp_error);
      faircall_student_delete (ret);
      return NULL;
    }

  xmlChar *called = xmlGetProp (node, CALLED);
  if (!called)
    {
      if (!name)
	{
	  g_set_error (error,
		       FAIRCALL_IO_ERROR,
		       XML_PARSE_ERROR,
		       "This student node does not have a called attribute.");
	  faircall_student_delete (ret);
	  return NULL;
	}
    }

  guint64 called_on = g_ascii_strtoull (called, NULL, 10);
  if (called_on > G_MAXUINT)
    {
      g_set_error (error,
		   FAIRCALL_IO_ERROR,
		   XML_PARSE_ERROR,
		   "This student has been called to many times to count.");
      called_on = G_MAXUINT;
    }
  ret->called_on = (guint) called_on;

  return ret;
}

gboolean
faircall_io_is_class (gchar const *const restrict name,
		      GError **error)
{
  GError *tmp_error = NULL;
  xmlDocPtr doc = faircall_io_open_doc (&tmp_error);
  if (!doc)
    goto error;

  xmlNodePtr root = faircall_io_get_root (doc, &tmp_error);
  if (!root)
    goto error;

  for (xmlNodePtr cur = root->xmlChildrenNode;
       cur; cur = cur->next)
    {
      if (xmlStrcmp (cur->name, CLASS) == 0)
	{
	  xmlChar *n = xmlGetProp (cur, NAME);
	  if (!n)
	    continue;

	  if (xmlStrcmp (n, name) == 0)
	    {
	      xmlFree (n);
	      xmlFreeDoc (doc);
	      return TRUE;
	    }

	  xmlFree (n);
	}
    }

  xmlFreeDoc (doc);
  return FALSE;

error:
  if (tmp_error)
    g_propagate_error (error, tmp_error);
  else
    g_set_error (error,
		 FAIRCALL_IO_ERROR,
		 UNKOWN_ERROR,
		 "An unkown error occured.");

  if (doc) xmlFreeDoc (doc);
  return FALSE;
}

struct Class *
faircall_io_get_class (gchar const *const restrict name,
		       GError **error)
{
  GError *tmp_error = NULL;
  struct Class *ret = faircall_class_new (name, &tmp_error);
  if (!ret)
    goto error;

  xmlDocPtr doc = faircall_io_open_doc (&tmp_error);
  if (!doc)
    goto error;

  xmlNodePtr root = faircall_io_get_root (doc, &tmp_error);
  if (!root)
    goto error;

  xmlNodePtr node = NULL;
  for (xmlNodePtr cur = root->xmlChildrenNode;
       cur; cur = cur->next)
    {
      if (xmlStrcmp (cur->name, CLASS) == 0)
	{
	  xmlChar *n = xmlGetProp (cur, NAME);
	  if (!n)
	    continue;

	  if (xmlStrcmp (n, name) == 0)
	    {
	      xmlFree (n);
	      node = cur;
	      break;
	    }

	  xmlFree (n);
	}
    }
  if (!node)
    goto notfound;

  xmlChar *last = xmlGetProp (node, LAST);
  if (!last) last = "";

  for (xmlNodePtr cur = node->xmlChildrenNode;
       cur; cur = cur->next)
    {
      if (xmlStrcmp (cur->name, STUDENT) == 0)
	{
	  struct Student *student =
	    faircall_io_get_student (cur, &tmp_error);
	  if (!student)
	    goto error;

	  faircall_roster_add_student (class->r, student);
	  if (0 == xmlStrcmp (last, student->name))
	    ret->last_called = student;
	}
    }

  xmlChar *even = xmlGetProp (node, EVEN);
  if (even)
    {
      faircall_class_set_forced_even (ret,
	(0 == xmlStrcmp (even, "true")));
      xmlFree (even);
    }

  xmlFreeDoc (doc);
  return ret;

error:
  if (tmp_error)
    g_propagate_error (error, tmp_error);
  else
    g_set_error (error,
		 FAIRCALL_IO_ERROR,
		 UNKOWN_ERROR,
		 "An unkown error occured.");

notfound:
  if (doc) xmlFreeDoc (doc);
  if (ret) faircall_class_delete (ret);

  return NULL;
}

gboolean
faircall_io_save_class (struct Class const *const class,
		      GError **error)
{
  return FALSE;
}

gchar **
faircall_io_saved_classes (GError **error)
{
  return NULL;
}


