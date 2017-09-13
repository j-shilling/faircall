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

#include "class.r"
#include "class-priv.h"
#include "student.r"
#include "student-priv.h"
#include "roster.r"
#include "roster-priv.h"

#include "io.h"
#include "error.h"
#include "compare.h"

#include <stdlib.h>
#include <math.h>

/* Local Error Domain and Codes */
#define FAIRCALL_CLASS_ERROR faircall_class_error_quark()

static GQuark
faircall_class_error_quark()
{
  return g_quark_from_static_string ("faircall-class-error-quark");
}

/************************************************************************
  LOCAL FUNCTIONS TO CALCULATE OUTLIERS
************************************************************************/

/* Passed to qsort */
static int
faircall_gdouble_cmp (const void *a, const void *b)
{
  return (*(gdouble *)a - *(gdouble *)b);
}

/* Return the median of vals */
static gdouble
faircall_median (gdouble *vals, guint len)
{
  return (len % 2) == 0 ?
    (vals[len / 2] + vals[(len / 2) - 1]) / 2
    : vals[len / 2];
}

/* Return -1 if called_on is low, 0 if fine, 1 if too high */
static gint
faircall_outlier (struct Class *class, struct Student *student)
{
  if (!class || !student)
    {
      g_warning ("Called faircall_outlier with null parameters.");
      return 0;
    }

  if (class->r->size <= 1)
    return 0;

  gdouble *counts = g_malloc (sizeof (gdouble) * class->r->size);
  for (int i = 0; i < class->r->size; i ++)
    counts[i] = (gdouble) class->r->arr[i]->called_on;

  qsort (counts, class->r->size, sizeof (gdouble), faircall_gdouble_cmp);
  gdouble q1 = faircall_median (counts, class->r->size / 2);
  gdouble q3 = (class->r->size % 2) == 0 ?
    faircall_median (counts + (class->r->size / 2), class->r->size / 2)
    : faircall_median (counts + ((class->r->size / 2) + 1), class->r->size / 2);
  g_free (counts);

  gdouble shift = (q3 - q1) * 1.5;
  gdouble max = q3 + shift;
  gdouble min = q3 - shift;

  if (student->called_on < min)
    return -1;
  else if (student->called_on > max)
    return 1;
  else
    return 0;
}

/************************************************************************
  CONSTRUCT AND DESTROY
*************************************************************************/

struct Class *
faircall_class_alloc (void)
{
  struct Class *ret =
    (struct Class *) g_malloc (sizeof (struct Class));
  if (!ret)
    return NULL;

  ret->name = NULL;
  ret->forced_even = FALSE;
  ret->r = faircall_roster_new();
  ret->last_called = NULL;
  ret->last_last_called = NULL;

  g_mutex_init (&ret->m);

  return ret;
}

struct Class *
faircall_class_new (gchar const *const restrict name,
		    GError **error)
{
  /* VALIDATE name */

  // Name must be present
  if (!name || !name[0])
    {
      g_set_error (error,
		   FAIRCALL_CLASS_ERROR,
		   NO_NAME_ERROR,
		   "Every class must have a name.");
      return NULL;
    }

  // Name must be UTF8
  if (!g_utf8_validate (name, -1, NULL))
    {
      g_warning ("faircall_class_new was given a name "
		 "which was not a valid UTF8 string.");
      return NULL;
    }

  // Name must be unique
  GError *tmp_error = NULL;
  if (faircall_io_is_class (name, &tmp_error))
    {
      if (tmp_error)
	g_propagate_error (error, tmp_error);
      else
	g_set_error (error,
		     FAIRCALL_CLASS_ERROR,
		     NAME_NOT_UNIQUE_ERROR,
		     "There is alread a class \"%s\".",
		     name);
      return NULL;
    }

  struct Class *ret = faircall_class_alloc ();

  ret->name = g_strdup (name);
  return ret;
}

void
faircall_class_delete (struct Class *class)
{
  if (!class)
    return; 

  g_mutex_lock (&class->m); // don't free while class is being edited

  g_free (class->name);
  faircall_roster_delete (class->r);
  g_mutex_unlock (&class->m);
  g_mutex_clear (&class->m);
  g_free (class);
}
/************************************************************************
  GET INFORMATION AND DATA
*************************************************************************/

gchar *
faircall_class_name (struct Class const *const restrict class)
{
  return class ? class->name : NULL;
}

gchar **
faircall_class_info (struct Class const *const restrict class,
		     GError **error)
{
  if (!class)
    {
      g_warning ("called faircall_class_info with null class");
      return NULL;
    }

  gchar **ret = g_malloc (sizeof (gchar *) * (class->r->size + 3));
  ret[0] = g_strdup_printf ("%s: %lu students, forced even: %s\n",
			    faircall_class_name (class),
			    faircall_roster_length (class->r),
			    class->forced_even ? "true" : "false");
  ret[1] = g_strdup ("\n");

  for (int i = 0; i < class->r->size; i++)
    {
      struct Student *student = class->r->arr[i];
      guint outlier = faircall_outlier ((struct Class *)class, student);

      ret[i+2] = g_strdup_printf ("%s  %u %s\n",
			 student->name,
			 student->called_on,
			 outlier == 0 ? "" : outlier > 0 ? "[too high]" : "[too low]");
    }

  ret[class->r->size + 2] = 0;
  qsort (ret + 2, class->r->size, sizeof (gchar *), faircall_strcmp);

  return ret;
}

struct Student *
faircall_class_get_student (struct Class const *const restrict class,
			    gchar const *const restrict name,
			    GError **error)
{
  if (!class)
    {
      g_warning ("faircall_class_get_student called on null class");
      return NULL;
    }

  for (int i = 0; i < class->r->size; i ++)
    if (faircall_student_cmp_str (&class->r->arr[i], &name) == 0)
      return class->r->arr[i];

  g_set_error (error,
	       FAIRCALL_CLASS_ERROR,
	       NO_SUCH_NAME_ERROR,
	       "There is not student named \"%s\" in class \"%s\"",
	       name, class->name);

  return NULL;
}

gchar **
faircall_class_list (struct Class const *const restrict class,
	 	     GError **error)
{
  if (!class)
    {
      g_warning ("faircall_class_list called on a null class.");
      return NULL;
    }

  size_t size = faircall_roster_length (class->r);
  gchar **ret =
    g_malloc (sizeof (gchar *) * (size + 1));

  for (int i = 0; i < class->r->size; i++)
    ret[i] = g_strdup_printf ("%s\n",
			      class->r->arr[i]->name);

  ret[size] = 0;

  qsort (ret, size, sizeof (gchar *), faircall_strcmp);
  return ret;
}

/************************************************************************
  GET INFORMATION AND DATA
*************************************************************************/

static void
faircall_class_add_student (struct Class *const restrict class,
			    struct Student const *const restrict student,
			    GError **error)
{
  if (!class || !student)
    {
      g_warning ("Cannot call faircall_class_add_student with null parameters: "
		 "{ class = %p, student = %p }",
		 class, student);
      return;
    }

  if (faircall_roster_is_student (class->r, student->name))
    {
      g_set_error (error,
		   FAIRCALL_CLASS_ERROR,
		   NAME_NOT_UNIQUE_ERROR,
		   "There is alread a %s in class %s.",
		   student->name, class->name);
      return;
    }

  g_mutex_lock (&class->m);
  faircall_roster_add_student (class->r, student);
  g_mutex_unlock (&class->m);
}

void
faircall_class_new_student (struct Class *const restrict class,
			    gchar const *const restrict name,
			    GError **error)
{
  if (!class)
  {
    g_warning ("Cannot call faircall_class_new_student on a null class");
    return;
  }

  GError *tmp_error = NULL;
  struct Student const *const student =
    faircall_student_new (name, &tmp_error);
  if (tmp_error)
    {
      g_propagate_error (error, tmp_error);
      return;
    }

  faircall_class_add_student (class, student, &tmp_error);
  if (tmp_error)
    {
      g_propagate_error (error, tmp_error);
      return;
    }
}

void
faircall_class_del_student (struct Class *const restrict class,
			    gchar const *const restrict name,
			    GError **error)
{
  if (!class || !name)
    {
      g_warning ("Cannot call faircall_class_del_student with null parameters "
		 "class = %p, name = %p", class, name);
      return;
    }

  g_mutex_lock (&class->m);
  GError *tmp_error = NULL;
  struct Student * student =
    faircall_class_get_student (class, name, &tmp_error);
  if (tmp_error)
    g_propagate_error (error, tmp_error);
  else
    faircall_roster_del_student (class->r, student);
  g_mutex_unlock (&class->m);
}

void
faircall_class_set_forced_even (struct Class *const restrict class,
				gboolean const val)
{
  if (!class)
    return;

  g_mutex_lock (&class->m);
  class->forced_even = val;
  g_mutex_unlock (&class->m);
}

gpointer
faircall_class_update (gpointer data)
{
  if (!data)
    {
      g_warning ("faircall_update_call called with "
		 "data == NULL");
      goto cleanup;
    }

  struct Class *const class = data;
  g_mutex_trylock (&class->m);

  struct Student const *const student = class->last_called;

  /* Primary mode alows for any student to be called
     except for class->last called */
  if (!class->forced_even)
    {
      faircall_roster_set_may_call_anyone (class->r);
      // If there is only one student than we are done
      if (faircall_roster_length (class->r) <= 1)
	goto cleanup;

      // Check fairness
      struct Student **students = faircall_roster_as_array (class->r);
      gsize len = faircall_roster_length (class->r);

      gdouble *calls = g_malloc (sizeof (gdouble) * len);
      for (int i = 0; i < len; i ++)
	calls[i] = (gdouble) students[i]->called_on;

      qsort (calls, len,
	     sizeof (gdouble), faircall_gdouble_cmp);
      gdouble q1 = faircall_median (calls, len / 2);
      gdouble q3 = (len % 2) == 0 ?
	faircall_median (calls + (len / 2), len / 2)
	: faircall_median (calls + ((len / 2) + 1), len / 2);
      g_free (calls);

      gdouble iqr = q3 - q1;
      gdouble min = q1 - (iqr * 1.5);
      gdouble max = q3 + (iqr * 1.5);

      min = min<0 ? 0 : min;
      max = max<0 ? 0 : max;

      // student->called_on should try to fall between min and max
      for (int i = 0; i < (len >= 5 ? (len * .8) : len); i ++)
	{
	  struct Student *item = students[i];
	  if (!item)
	    {
	      g_warning ("Somehow null data ended up in the roster");
	      goto cleanup;
	    }
	  if (item == student)
	    continue;

	  guint called_on = item->called_on;
	  if (called_on < min)
	    {
	      // this student must be called
	      faircall_roster_set_must_call (class->r, item);
	    }
	  else if (called_on > max)
	    {
	      // this student must not be called
	      faircall_roster_set_cant_call (class->r, item);
	    }
	}

      g_free (students);

    if (faircall_roster_could_call (class->r) <= 1)
	faircall_roster_set_may_call_anyone (class->r);
    }

  // Students should not be called twice in a row
  if (student)
    faircall_roster_set_cant_call (class->r, student);

  /* Secondary mode tries to ensure that all students are called
     an equal number of times. */

  // If we are out of students to call, repopulate the can_call list
  if (class->forced_even &&
      faircall_roster_could_call (class->r) == 0)
    {
      gsize len = faircall_roster_length (class->r);
      struct Student **students = faircall_roster_as_array(class->r);
      guint min = students[0] ? students[0]->called_on : 0;
      faircall_roster_set_cant_call_anyone (class->r);
      for (int i = 0; i < len; i++)
	{
	  if (students[i]->called_on < min)
	    {
	      faircall_roster_set_cant_call_anyone (class->r);
	      min = students[i]->called_on;
	    }

	  if (students[i]->called_on == min)
	    faircall_roster_set_may_call (class->r, students[i]);
	}
      g_free (students);
   }

cleanup:
  g_mutex_unlock (&class->m);
  g_thread_unref (g_thread_self());
  return NULL;
}

/************************************************************************
  CALLS AND UNDOS
*************************************************************************/

gchar *
faircall_class_call_student (struct Class *const restrict class)
{
  if (!class)
    {
      g_warning ("Cannot call faircall_class_call_student on a null class.");
      return NULL;
    }

  g_mutex_lock (&class->m);
  struct Student *ret = faircall_roster_call_student (class->r);

  if (ret)
    {
      ret->called_on ++;
      class->last_last_called = class->last_called;
      class->last_called = ret;
    }

  // Update the class in the background
  g_thread_new ("Update Class",
		faircall_class_update,
		class);

  return ret ? g_strdup (ret->name) : NULL;
}

/* If the called student was in the bathroom, undo the last call */
void
faircall_class_uncall_student (struct Class *const restrict class)
{
  if (!class)
    {
      g_warning ("Cannot call faircall_class_uncall_student on a null class.");
      return;
    }

  g_mutex_lock (&class->m);
  if (!class->last_called) // nothing to do
    {
      g_mutex_unlock (&class->m);
      return;
    }

  struct Student *const student = class->last_called;
  student->called_on --;

  faircall_roster_set_may_call (class->r, student);
  class->last_called = class->last_last_called;
  class->last_last_called = NULL;

  g_thread_new ("Update Class",
		faircall_class_update,
		class);
}

/* If student is absent, undo call and remove them from the class */
void
faircall_class_absent_student (struct Class *const restrict class)
{
  if (!class)
    {
      g_warning ("Cannot call faircall_class_uncall_student on a null class.");
      return;
    }

  g_mutex_lock (&class->m);
  struct Student *const student = class->last_called;
  student->called_on --;

  faircall_roster_del_student (class->r, student);
  class->last_called = class->last_last_called;
  class->last_last_called = NULL;

  g_thread_new ("Update Class",
		faircall_class_update,
		class);
}

gchar **
faircall_class_call_n_students (struct Class *const restrict class,
				guint const n,
				GError **error)
{
  if (!class)
    {
      g_warning ("Cannot call faircall_class_call_n_students on a "
		 "null class.");
      return NULL;
    }
  if (n > faircall_roster_length(class->r))
    {
      g_set_error (error,
		   FAIRCALL_CLASS_ERROR,
		   NOT_ENOUGH_STUDENTS_ERROR,
		   "Cannot call %u students from a class of %lu",
		   n, faircall_roster_length(class->r));
      return NULL;
    }

  g_mutex_lock (&class->m);
  gchar **ret = g_malloc ((n + 1) * sizeof (gchar *));
  for (int i = 0; i < n; i++)
    {
      struct Student *const restrict student =
	faircall_roster_call_student (class->r);
      if (!student)
	{
	  g_warning ("faircall_roster_call_student returned null.");
	  i--;
	  continue;
	}	  

      student->called_on++;
      faircall_roster_set_cant_call (class->r, student);
      ret[i] = g_strdup (student->name);
    }
  ret[n] = 0;

  g_thread_new ("Update Class",
		faircall_class_update,
		class);

  return ret;
}

gboolean
faircall_class_call_student_by_name (struct Class *const restrict class,
				     gchar const *const restrict name,
			  	     GError **error)
{
  GError *tmp_error = NULL;
  struct Student *student =
    faircall_class_get_student (class, name, &tmp_error);
  if (tmp_error)
    {
      g_propagate_error (error, tmp_error);
      return FALSE;
    }

  student->called_on ++;
  return TRUE;
}
