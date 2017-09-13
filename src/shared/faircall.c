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

#include <glib.h>

#include "faircall.h"
#include "class-priv.h"
#include "roster-priv.h"
#include "io.h"
#include "error.h"
#include "compare.h"

#include <stdlib.h>
#include <string.h>

static struct Class *class = NULL;

#define FAIRCALL_ERROR faircall_error_quark()

static GQuark
faircall_error_quark()
{
  return g_quark_from_static_string ("faircall-error-quark");
}

gboolean
faircall_open_class (gchar const *const name,
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

  if (!faircall_io_is_class (name, &tmp_error))
    {
      if (!tmp_error)
	class = faircall_class_new (name, &tmp_error);

      if (tmp_error)
	{
	  g_propagate_error (error, tmp_error);
	  return FALSE;
	}

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
    return TRUE;

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
  GError *tmp_error = NULL;
  if (faircall_io_is_class (name, &tmp_error))
    {
      if (tmp_error)
	{
	  g_propagate_error (error, tmp_error);
	  return FALSE;
	}

      return faircall_io_del_class (name, error);
    }

  if (!class)
    {
      g_set_error (error,
		   FAIRCALL_ERROR,
		   NO_OPEN_CLASS_ERROR,
		   "There is not class to delete from.");
      return FALSE;
    }

  faircall_class_del_student (class, name, &tmp_error);
  if (tmp_error)
    {
      g_propagate_error (error, tmp_error);
      return FALSE;
    }
  faircall_io_del_student (faircall_class_name(class), name, &tmp_error);
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
faircall_info (gchar const *const restrict name, GError **error)
{
  if (NULL == name)
    {
      if (!class)
	{
	  g_set_error (error,
		       FAIRCALL_ERROR,
		       NO_OPEN_CLASS_ERROR,
		       "There is no open class to examine.");
	  return NULL;
	}

      return faircall_class_info (class, error);
    }
  
  GError *tmp_error = NULL;
  if (faircall_io_is_class (name, &tmp_error))
    {
      struct Class *other = faircall_io_get_class (name, &tmp_error);
      if (tmp_error)
	{
	  g_propagate_error (error, tmp_error);
	  return NULL;
	}

      return faircall_class_info (other, error);
    }
  else if (tmp_error)
    {
      g_propagate_error (error, tmp_error);
      return NULL;
    }

  if (!class)
    {
      g_set_error (error,
		   FAIRCALL_ERROR,
		   NO_OPEN_CLASS_ERROR,
		   "There is no open class to examine.");
      return NULL;
    }

  struct Student *student =
    faircall_class_get_student (class, name, &tmp_error);
  if (tmp_error)
    {
      g_propagate_error (error, tmp_error);
      return NULL;
    }

  return faircall_student_info (student, error);
}

gboolean
faircall_set_forced_even (gboolean const value, GError **error)
{
  if (!class)
    {
      g_set_error (error,
		   FAIRCALL_ERROR,
		   NO_OPEN_CLASS_ERROR,
		   "There is no class edit.");
      return FALSE;
    }

  faircall_class_set_forced_even (class, value);
  return TRUE;
}

gboolean
faircall_call_student_by_name (gchar const *const restrict name,
			       GError **error)
{
  if (!class)
    {
      g_set_error (error,
		   FAIRCALL_ERROR,
		   NO_OPEN_CLASS_ERROR,
		   "There is no class call from.");
      return FALSE;
    }

  return faircall_class_call_student_by_name (class, name, error);
}

gchar **
faircall_list (GError **error)
{
  if (class)
    return faircall_class_list (class, error);
  else
    return faircall_io_saved_classes (error);
}

gchar *
faircall_get_open_class_name(void)
{
  return class ? faircall_class_name (class) : "";
}
