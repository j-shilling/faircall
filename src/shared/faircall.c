#include <glib.h>

#include "class.r"
#include "student.r"
#include "class-priv.h"
#include "roster-priv.h"
#include "io.h"
#include "error.h"

gboolean faircall_open_class (gchar const *const name,
			      gboolean *const restrict new,
			      GError **error);gboolean faircall_close_class (GError **error);
gboolean faircall_add_student (gchar const *const name, GError **error);
gboolean faircall_del_student (gchar const *const name, GError **error);
gboolean faircall_undo_call (GError **error);
gboolean faircall_mark_absent (GError **error);
gboolean faircall_set_forced_even (gboolean const value, GError **error);

gchar *  faircall_call_student (GError **error);
gchar ** faircall_call_n_students (guint n, GError **error);

gchar ** faircall_info (GError **error);

static struct Class *class = NULL;

#define FAIRCALL_ERROR faircall_error_quark()

static GQuark
faircall_error_quark()
{
  return g_quark_from_static_string ("faircall-error-quark");
}

gboolean
faircall_open_class (gchar const *const name,
		     gboolean *const restrict new,
		     GError **error)
{
  GError *tmp_error = NULL;
  if (class)
    {
      faircall_close_class (&tmp_error);
      if (tmp_error);
	{
	  g_propagate_error (error, tmp_error);
	  return FALSE;
	}
    }

  *new = FALSE;
  if (!faircall_io_is_class (name, &tmp_error))
    {
      if (!tmp_error)
	class = faircall_class_new (name, &tmp_error);

      if (tmp_error)
	{
	  g_propagate_error (error, tmp_error);
	  return FALSE;
	}

      *new = TRUE;
      return TRUE;
    }
 
  class = faircall_io_get_class (name, &tmp_error);
  if (tmp_error)
    {
      g_propagate_error (error, tmp_error);
      return FALSE;
    }

  return (class != NULL);
}

gboolean
faircall_close_class (GError **error)
{
  if (!class)
    return FALSE;

  gboolean ret = faircall_io_save_class (class, error);
  if (ret)
    class = NULL;

  return ret;
}

gboolean
faircall_add_student (gchar const *const name, GError **error)
{
  if (!class)
    {
      g_set_error (error,
		   FAIRCALL_ERROR,
		   NO_OPEN_CLASS_ERROR,
		   "There is not class to add to.");
      return FALSE;
    }

  GError *tmp_error = NULL;
  faircall_class_new_student (class, name, &tmp_error);
  if (tmp_error)
    {
      g_propagate_error (error, tmp_error);
      return FALSE;
    }

  return TRUE;
}

gboolean
faircall_del_student (gchar const *const name, GError **error)
{
  if (!class)
    {
      g_set_error (error,
		   FAIRCALL_ERROR,
		   NO_OPEN_CLASS_ERROR,
		   "There is not class to delete from.");
      return FALSE;
    }

  GError *tmp_error = NULL;
  faircall_class_del_student (class, name, &tmp_error);
  if (tmp_error)
    {
      g_propagate_error (error, tmp_error);
      return FALSE;
    }

  return TRUE;
}

gchar *
faircall_call_student (GError **error)
{
  if (!class)
    {
      g_set_error (error,
		   FAIRCALL_ERROR,
		   NO_OPEN_CLASS_ERROR,
		   "There is not class to delete from.");
      return NULL;
    }

  return faircall_class_call_student (class); 
}

gboolean
faircall_undo_call (GError **error)
{
  if (!class)
    {
      g_set_error (error,
		   FAIRCALL_ERROR,
		   NO_OPEN_CLASS_ERROR,
		   "There is not class to delete from.");
      return FALSE;
    }

  faircall_class_uncall_student (class);
  return TRUE;
}

gboolean
faircall_mark_absent (GError **error)
{
   if (!class)
    {
      g_set_error (error,
		   FAIRCALL_ERROR,
		   NO_OPEN_CLASS_ERROR,
		   "There is not class to delete from.");
      return FALSE;
    }

   faircall_class_absent_student (class);
   return TRUE;
}


gchar **
faircall_call_n_students (guint n, GError **error)
{
  if (!class)
    {
      g_set_error (error,
		   FAIRCALL_ERROR,
		   NO_OPEN_CLASS_ERROR,
		   "There is not class to delete from.");
      return NULL;
    }

  return faircall_class_call_n_students (class, n, error);
}

gchar **
faircall_info (GError **error)
{
  if (!class)
    {
      g_set_error (error,
		   FAIRCALL_ERROR,
		   NO_OPEN_CLASS_ERROR,
		   "There is not class to delete from.");
      return NULL;
    }

  gchar **ret = g_malloc0 (sizeof (gchar *) * (faircall_roster_length (class->r) + 3));
  ret[0] = g_strdup_printf ("%s - Forced Event: %s - Students: %lu\n",
			    class->name, class->forced_even ? "true" : "false",
			    faircall_roster_length (class->r));
  ret[1] = g_strdup ("\n");

  struct Student **students = faircall_roster_as_array (class->r);
  for (int i = 0; students[i]; i ++)
    {
      ret[i + 2] = g_strdup_printf ("%s called %u times\n",
				    students[i]->name,
				    students[i]->called_on);
    }

  return ret;
}

gboolean
faircall_set_forced_even (gboolean const value, GError **error)
{
  if (!class)
    {
      g_set_error (error,
		   FAIRCALL_ERROR,
		   NO_OPEN_CLASS_ERROR,
		   "There is not class to delete from.");
      return FALSE;
    }

  faircall_class_set_forced_even (class, value);
  return TRUE;
}
