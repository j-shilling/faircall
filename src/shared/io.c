#include "class.r"
#include "student.r"
#include "roster.r"

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

gboolean
faircall_io_is_class (gchar const *const restrict name,
		      GError **error)
{
  return FALSE;
}

struct Class *
faircall_io_get_class (gchar const *const restrict name,
		       GError **error)
{
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


