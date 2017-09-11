#include "class.r"
#include "class-priv.h"
#include "student.r"
#include "student-priv.h"
#include "roster.r"
#include "roster-priv.h"

#include "error.h"

#include <config.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <glib/gstdio.h>

/* Local Error Domain and Codes */
#define FAIRCALL_IO_ERROR faircall_io_error_quark()

/* XML name constants */

static gchar const *const restrict STUDENT = "student";
static gchar const *const restrict NAME = "name";
static gchar const *const restrict CALLED = "called";
static gchar const *const restrict INDEX = "index";
static gchar const *const restrict MUST = "must_call";

static gchar const *const restrict ROSTER = "roster";
static gchar const *const restrict SIZE = "size";
static gchar const *const restrict NMEMB = "nmemb";

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
      //atexit (cleanup);
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
	  file = g_fopen (filename, "a");
	  if (!file)
	    goto error;
	  else
	    fclose (file);

	  xmlDocPtr doc = xmlNewDoc (BAD_CAST "1.0");
	  xmlNodePtr root = xmlNewNode (NULL, BAD_CAST "root");
	  xmlDocSetRootElement (doc, root);

	  if (0 > xmlSaveFile (filename, doc))
	    goto error;

	  return doc;
	}
      else
	{
	  goto error;
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

error:
  g_set_error (error,
	       FAIRCALL_IO_ERROR,
	       g_file_error_from_errno (errno),
	       "%s",
	       g_strerror (errno));
  return NULL;
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
			  guint const index,
			  gboolean const mustcall,
			  struct Student const *const restrict student)
{
  if (!parent)
    {
      g_warning ("Cannot save a student to a null parent node.");
      return;
    }

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

      gchar *i = g_strdup_printf ("%u", index);
      xmlNewProp (node, INDEX,
		  (!i || !(*i)) ? "?" : i);
      g_free (i);

      xmlNewProp (node, MUST,
		  mustcall ? "true" : "false");
    }
  else
    {
      gchar *called_on = g_strdup_printf ("%u", student->called_on);
      xmlSetProp (node, CALLED,
		  (!called_on || !(*called_on)) ? "0" : called_on);
      g_free (called_on);

      gchar *i = g_strdup_printf ("%u", index);
      xmlSetProp (node, INDEX,
		  (!i || !(*i)) ? "?" : i);
      g_free (i);

      xmlSetProp (node, MUST,
		  mustcall ? "true" : "false");
    }
}

static struct Student *
faircall_io_get_student (xmlNodePtr const restrict node,
			 gint *const restrict index,
			 gboolean *const restrict mustcall,
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
      g_set_error (error,
		   FAIRCALL_IO_ERROR,
		   XML_PARSE_ERROR,
		   "This student node does not have a called attribute.");
      faircall_student_delete (ret);
      return NULL;
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

  xmlChar *_mustcall = xmlGetProp (node, MUST);
  if (_mustcall)
    *mustcall = (0 == xmlStrcmp (_mustcall, "true"));
  else
    *mustcall = FALSE;
  xmlFree (_mustcall);

  xmlChar *i = xmlGetProp (node, INDEX);
  if (!i || (0 == xmlStrcmp (i, "?")))
    {
      g_set_error (error,
		   FAIRCALL_IO_ERROR,
		   XML_PARSE_ERROR,
		   "This student does not have an index.");
      *index = -1;
    }
  else
    {
      gint64 _index = g_ascii_strtoll (i, NULL, 10);

      if (_index > G_MAXINT)
	{
	  g_set_error (error,
		       FAIRCALL_IO_ERROR,
		       XML_PARSE_ERROR,
		       "This student has an index too high to count.");
	  _index = G_MAXINT;
	}

      *index = (gint) _index;
     }
  xmlFree (i);

  return ret;
}

static void
faircall_io_save_roster (xmlNodePtr const restrict parent,
			 struct Roster const *const restrict r)
{
  if (!parent)
    {
      g_warning ("Cannot save a roster to a null parent node.");
      return;
    }

  xmlNodePtr node = NULL;
  for (xmlNodePtr cur = parent->xmlChildrenNode;
       cur; cur = cur->next)
    {
      if (xmlStrcmp (cur->name, ROSTER) == 0)
	{
	  node = cur;
	  break;
	}
    }
  
  if (NULL == node)
    {
      node = xmlNewChild (parent, NULL, ROSTER, NULL);

      gchar *size = g_strdup_printf ("%lu", r->size);
      xmlNewProp (node, SIZE,
		  (size && *size) ? size : "");
      g_free (size);

      gchar *nmemb = g_strdup_printf ("%u", r->nmemb);
      xmlNewProp (node, NMEMB,
		  (nmemb && *nmemb) ? nmemb : "");
      g_free (nmemb);
    }
  else
    {
      gchar *size = g_strdup_printf ("%lu", r->size);
      xmlSetProp (node, SIZE,
		  (size && *size) ? size : "");
      g_free (size);

      gchar *nmemb = g_strdup_printf ("%u", r->nmemb);
      xmlSetProp (node, NMEMB,
		  (nmemb && *nmemb) ? nmemb : "");
      g_free (nmemb);
    }

  for (int i = 0; i < r->size; i++)
    faircall_io_save_student (node,
			      i,
			      g_queue_find (r->must_call, r->arr[i]) != NULL,
			      r->arr[i]);
}

static struct Roster *
faircall_io_get_roster (xmlNodePtr const restrict node,
			GError **error)
{
  if (!node)
    {
      g_warning ("Cannot get roster from a null node");
      return NULL;
    }
  if (0 != xmlStrcmp (node->name, ROSTER))
    {
      g_warning ("Cannot get roster from a non-roster node");
      return NULL;
    }

  xmlChar *_size = xmlGetProp (node, SIZE);
  if (!_size || !(*_size))
    {
      g_set_error (error,
		   FAIRCALL_IO_ERROR,
		   XML_PARSE_ERROR,
		   "This roster does not have a size.");
      return NULL;
    }
  xmlChar *_nmemb = xmlGetProp (node, NMEMB);
  if (!_nmemb || !(*_nmemb))
    {
      g_set_error (error,
		   FAIRCALL_IO_ERROR,
		   XML_PARSE_ERROR,
		   "This roster does not have an nmemb.");
      return NULL;
    }
  guint64 size = g_ascii_strtoull (_size, NULL, 10);
  guint64 nmemb = g_ascii_strtoull (_nmemb, NULL, 10);
  xmlFree (_size);
  xmlFree (_nmemb);

  if (nmemb > G_MAXUINT)
    {
      g_warning ("nmemb is to high to count.");
      nmemb = G_MAXUINT;
    }
  if (nmemb > size)
    {
      g_set_error (error,
		   FAIRCALL_IO_ERROR,
		   XML_PARSE_ERROR,
		   "This roster has more members than it can hold.");
      return NULL;
    }

  struct Roster *ret = faircall_roster_new();
  ret->size = (gsize) size;
  ret->nmemb = (guint) nmemb;
  ret->arr = g_malloc0 (sizeof (struct Student *) * ret->size);

  GQueue *noindex = g_queue_new();
  for (xmlNodePtr cur = node->xmlChildrenNode;
       cur; cur = cur->next)
    {
      if (0 == xmlStrcmp (cur->name, STUDENT))
	{
	  GError *tmp_error = NULL;
	  gint index = -1;
	  gboolean mustcall = FALSE;
	  struct Student *student = faircall_io_get_student (cur,
							     &index,
							     &mustcall,
							     &tmp_error);

	  if (tmp_error)
	    {
	      g_propagate_error (error, tmp_error);
	      faircall_roster_delete (ret);
	      return NULL;
	    }
	  if (!student)
	    {
	      g_warning ("faircall_io_get_student returned null.");
	      faircall_roster_delete (ret);
	      return NULL;
	    }

	  if (index >= ret->size)
	    {
	      g_warning ("faircall_io_get_student gave an invalid index.");
	      faircall_roster_delete (ret);
	      return NULL;
	    }

	  if (index < 0)
	    g_queue_push_head (noindex, student);
	  else
	    ret->arr[index] = student;

	  if (mustcall)
	    g_queue_push_head (ret->must_call, student);
	}
    }

  for (int i = 0; i < ret->size; i++)
    if (ret->arr[i] == NULL)
      for (int j = i; j < ret->size; j++)
	ret->arr[j] = j < (ret->size - 1) ? ret->arr[j+1] : 0;

  while (!g_queue_is_empty (noindex))
    faircall_roster_add_student (ret, g_queue_pop_head (noindex));
  g_queue_free (noindex);

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
  if (!name || !name[0] || !g_utf8_validate (name, -1, NULL))
    {
      g_set_error (error,
		   FAIRCALL_IO_ERROR,
		   NO_NAME_ERROR,
		   "\"%s\" is not a valid class name.",
		   name);
      return NULL;
    }

  GError *tmp_error = NULL;

  struct Class *ret = NULL;
  struct Student *last_called = NULL;
  struct Roster *roster = NULL;
  gboolean forced_even = FALSE;

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
    if (xmlStrcmp (cur->name, ROSTER) == 0)
      roster = faircall_io_get_roster (cur, &tmp_error);

  if (!roster)
    goto error;

  for (int i = 0; i < roster->size; i++)
    if (roster->arr[i]
	&& 0 == xmlStrcmp (last, roster->arr[i]->name))
      last_called = roster->arr[i];
  if (last) xmlFree (last);

  xmlChar *even = xmlGetProp (node, EVEN);
  if (even)
    {
      forced_even = (0 == xmlStrcmp (even, "true"));
      xmlFree (even);
    }

  xmlFreeDoc (doc);

  ret = g_malloc (sizeof (struct Class));
  ret->name = g_strdup (name);
  ret->forced_even = forced_even;
  ret->r = roster;
  ret->last_called = last_called;
  g_mutex_init (&ret->m);

  g_thread_new ("Update Class",
		faircall_class_update,
		ret);

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
  if (!class)
    {
      g_warning ("Cannot save a null class.");
      return TRUE;
    }

  GError *tmp_error = NULL;
  gboolean ret = FALSE;
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
      if (0 == xmlStrcmp (cur->name, CLASS))
	{
	  xmlChar *name = xmlGetProp (cur, NAME);
	  if (!name)
	    continue;

	  if (0 == xmlStrcmp (name, class->name))
	    {
	      xmlFree(name);
	      node = cur;
	      break;
	    }

	  xmlFree (name);
	}
    }

  if (!node)
    {
      node = xmlNewChild (root, NULL, CLASS, NULL);
      xmlNewProp (node, NAME, class->name);
      xmlNewProp (node, EVEN, class->forced_even ? "true" : "false");
      xmlNewProp (node, LAST, class->last_called ?
		  class->last_called->name : "");
    }
  else
    {
      xmlSetProp (node, NAME, class->name);
      xmlSetProp (node, EVEN, class->forced_even ? "true" : "false");
      xmlSetProp (node, LAST, class->last_called ?
		  class->last_called->name : "");
    }
      
  faircall_io_save_roster (node, class->r);  

success:
  ret = TRUE;
  if (0 > xmlSaveFile (faircall_io_get_filename(), doc))
    goto error;

  goto cleanup;
  
error:
  ret = FALSE;
  if (tmp_error)
    g_propagate_error (error, tmp_error);
  else
    g_set_error (error,
		 FAIRCALL_IO_ERROR,
		 UNKOWN_ERROR,
		 "An unkown error occured.");

cleanup:
  if (doc) xmlFreeDoc (doc);

  return ret;
}

gchar **
faircall_io_saved_classes (GError **error)
{
  GError *tmp_error = NULL;
  gchar **ret = NULL;

  xmlDocPtr doc = faircall_io_open_doc (&tmp_error);
  if (!doc)
    goto error;

  xmlNodePtr root = faircall_io_get_root (doc, &tmp_error);
  if (!root)
    goto error;

  ret = g_malloc0 (sizeof (gchar *) * (xmlChildElementCount (root) + 1));

  guint i = 0;
  for (xmlNodePtr cur = root->xmlChildrenNode;
       cur; cur = cur->next)
    {
      if (0 == xmlStrcmp (cur->name, CLASS))
	{
	  xmlChar *name = xmlGetProp (cur, NAME);
	  if (name)
	    {
	      ret[i] = (gchar *) name;
	      i++;
	    }
	}
    }

  xmlFreeDoc (doc);
  return ret;

error:
  if (doc) xmlFreeDoc (doc);
  if (ret) g_strfreev (ret);
  ret = g_malloc (sizeof (gchar *));
  ret[0] = 0;

  if (tmp_error)
    g_propagate_error (error, tmp_error);
  else
    g_set_error (error,
		 FAIRCALL_IO_ERROR,
		 UNKOWN_ERROR,
		 "An unkown error occured.");

  return ret;
}
