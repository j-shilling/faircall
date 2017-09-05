#include "class-priv.h"
#include "student-priv.h"
#include "io.h"

#include <stdlib.h>
#include <math.h>

/* Local Error Domain and Codes */
#define FAIRCALL_CLASS_ERROR faircall_class_error_quark()

static GQuark
faircall_class_error_quark()
{
  return g_quark_from_static_string ("faircall-class-error-quark");
}

/* Used to check if a student name is unique within a 
   class */
static gint
faircall_find_student_by_name (gconstpointer a, gconstpointer b)
{
  gchar const *const x =
    g_utf8_normalize (((struct Student *)a)->name, -1, G_NORMALIZE_DEFAULT);
  gchar const *const y =
    g_utf8_normalize ((gchar *)b, -1, G_NORMALIZE_DEFAULT);

  gint ret = g_strcmp0 (x, y);

  g_free ((gpointer)x);
  g_free ((gpointer)y);

  return ret;
}

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

static void
faircall_class_can_call (struct Class *const restrict class,
			 struct Student const *const restrict student)
{
  if (!class || !student)
    {
      g_warning ("Cannot call faircall_class_can_call with null params "
		 "{class = %p, student = %p}.",
		 class, student);
      return;
    }

  class->can_call = g_list_prepend (class->can_call, (gpointer)student);
  class->can_call_length ++;
}

static void
faircall_class_cant_call (struct Class *const restrict class,
			 struct Student const *const restrict student)
{
  if (!class || !student)
    {
      g_warning ("Cannot call faircall_class_cant_call with null params "
		 "{class = %p, student = %p}.",
		 class, student);
      return;
    }

  class->cant_call = g_list_prepend (class->cant_call, (gpointer)student);
}

static void
faircall_class_must_call (struct Class *const restrict class,
			 struct Student const *const restrict student)
{
  if (!class || !student)
    {
      g_warning ("Cannot call faircall_class_must_call with null params "
		 "{class = %p, student = %p}.",
		 class, student);
      return;
    }

  g_queue_push_tail (class->must_call, (gpointer)student);
}

/* Passed to g_thread_new to update a class after
   a student has been called */
static gpointer
faircall_update_class (gpointer data)
{
  if (!data)
    {
      g_warning ("faircall_update_call called with "
		 "data == NULL");
      goto cleanup;
    }

  struct Class *const class = data;

  g_mutex_lock (&class->m);
  struct Student const *const student = class->last_called;

  /* Primary mode alows for any student to be called
     except for class->last called */
  if (!class->forced_even)
    {
      // Remove anyone who couldn't be called before
      class->can_call = g_list_concat (class->can_call, class->cant_call);
      class->can_call_length += g_list_length (class->cant_call);
      g_list_free (class->cant_call);
      class->cant_call = NULL;
      // If there is only one student than we are done
      if (class->can_call_length <= 1)
	goto cleanup;

      // Check fairness
      gdouble *calls = g_malloc (sizeof (gdouble) * class->can_call_length);
      guint i = 0;
      for (GList *cur = class->can_call; cur != NULL; cur = cur->next)
	{
	  struct Student *item = cur->data;
	  if (!item)
	    {
	      g_warning ("Somehow null data ended up in can_call");
	      goto cleanup;
	    }


	  calls[i] = (gdouble) item->called_on;
	  i++;
	}

      qsort (calls, class->can_call_length,
	     sizeof (gdouble), faircall_gdouble_cmp);
      gdouble q1 = faircall_median (calls, class->can_call_length / 2);
      gdouble q3 = (class->can_call_length % 2) == 0 ?
	faircall_median (calls + (class->can_call_length / 2), class->can_call_length / 2)
	: faircall_median (calls + ((class->can_call_length / 2) + 1), class->can_call_length / 2);
      gdouble iqr = q3 - q1;
      gdouble min = q1 - (iqr * 1.5);
      gdouble max = q3 - (iqr * 1.5);

      // student->called_on should try to fall between min and max
      GList *cur = class->can_call;
      while (cur)
	{
	  struct Student *item = cur->data;
	  if (!item)
	    {
	      g_warning ("Somehow null data ended up in can_call");
	      goto cleanup;
	    }

	  guint called_on = item->called_on;
	  if (called_on < min && cur->data != student)
	    {
	      // this student must be called
	      faircall_class_must_call (class, cur->data);

	      GList *temp = cur->next;
	      class->can_call = g_list_remove_link (class->can_call, cur);
	      class->can_call_length --;
	      g_list_free_1 (cur);
	      cur = temp;
	    }
	  else if (called_on > max)
	    {
	      // this student must not be called
	      faircall_class_cant_call (class, cur->data);

	      GList *temp = cur->next;
	      class->can_call = g_list_remove_link (class->can_call, cur);
	      class->can_call_length --;
	      g_list_free_1 (cur);
	      cur = temp;
	    }
	  else
	    {
	      cur = cur->next;
	    }
	}
    }

  // Students should not be called twice in a row
  if (student)
    {
      GList *l = g_list_find (class->can_call, student);
      if (l)
	{
	  class->can_call = g_list_remove_link (
			      class->can_call,
			      l);
	  class->can_call_length --;
	  faircall_class_cant_call (class, l->data);
	  g_list_free_1 (l);
	}
    }

  /* Secondary mode tries to ensure that all students are called
     an equal number of times. */

  // If we are out of students to call, repopulate the can_call list
  if (class->forced_even
      && class->can_call_length <= 1
      && class->cant_call)
    {
      GList *list = NULL;
      guint min =
	((struct Student *)class->cant_call->data)->called_on;

      for (GList *cur = class->cant_call; cur != NULL; cur = cur->next)
	{
	  if (((struct Student *)cur->data)->called_on < min) 
	    {
	      min = ((struct Student *)cur->data)->called_on;
	      g_list_free (list);
	      list = g_list_prepend (NULL, cur->data);
	    }
	  else if (((struct Student *)cur->data)->called_on == min)
	    {
	      list = g_list_prepend (list, cur->data);
	    }
	}

      class->can_call = g_list_concat (class->can_call, list);
      class->can_call_length += g_list_length (list);
      g_list_free (list);
    }

cleanup:
  g_mutex_unlock (&class->m);
  g_thread_unref (g_thread_self());
  return NULL;
}

/* Functions */

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

  struct Class *ret =
    (struct Class *) g_malloc (sizeof (struct Class));

  ret->name = g_strdup (name);
  ret->forced_even = FALSE;
  ret->can_call = NULL;
  ret->can_call_length = 0;
  ret->cant_call = NULL;
  ret->must_call = g_queue_new();
  ret->last_called = NULL;

  g_mutex_init (&ret->m);

  return ret;
}

void
faircall_class_delete (gpointer data)
{
  struct Class *class = data;

  g_mutex_lock (&class->m); // don't free while class is being edited

  g_free (class->name);
  g_list_free_full (class->can_call, faircall_student_delete);
  g_list_free_full (class->cant_call, faircall_student_delete);
  g_queue_free_full (class->must_call, faircall_student_delete);
  g_mutex_clear (&class->m);
  g_free (class);
}

void
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

  if (g_list_find_custom (class->can_call,
			  student->name,
			  faircall_find_student_by_name)
      || g_list_find_custom (class->cant_call,
			     student->name,
			     faircall_find_student_by_name))
    {
      g_set_error (error,
		   FAIRCALL_CLASS_ERROR,
		   NAME_NOT_UNIQUE_ERROR,
		   "There is alread a %s in class %s.",
		   student->name, class->name);
      return;
    }

  g_mutex_lock (&class->m);
  faircall_class_can_call (class, student);
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

gchar *
faircall_class_call_student (struct Class *const restrict class)
{
  if (!class)
    {
      g_warning ("Cannot call faircall_class_call_student on a null class.");
      return NULL;
    }

  g_mutex_lock (&class->m);
  struct Student *ret = NULL;
  // If there are students who must be called, call them
  if (!g_queue_is_empty (class->must_call))
    {
      ret = g_queue_pop_head (class->must_call);
    }
  // Call a random student
  else
    {
      if (class->can_call_length <= 0)
	{
	  g_warning ("We don't have anyone to call on.");
	  goto cleanup;
	}

      gint32 i = g_random_int_range (0, class->can_call_length);
      GList *node = g_list_nth (class->can_call, i);
      if (!node)
	{
	  g_warning ("Somehow g_list_nth returned null. "
		     "class->can_call_length = %d, i = %d, "
		     "g_list_length(class->can_call) = %d",
		     class->can_call_length, i, 
		     g_list_length(class->can_call));
	  goto cleanup;
	}
      ret = node->data;
    }

cleanup:
  if (ret)
    {
      ret->called_on ++;
      class->last_last_called = class->last_called;
      class->last_called = ret;
    }
  g_mutex_unlock (&class->m);

  // Update the class in the background
  g_thread_new ("Update Class",
		faircall_update_class,
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

  GList *list = g_list_find (class->cant_call, student);
  if (list)
    {
      class->cant_call = g_list_remove_link (class->cant_call, list);
      g_list_free_1 (list);

      faircall_class_can_call (class, student);
    }
  class->last_called = class->last_last_called;
  class->last_last_called = NULL;
  g_mutex_unlock (&class->m);
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

  GList *list = g_list_find (class->cant_call, student);
  if (list)
    {
      class->cant_call = g_list_remove_link (class->cant_call, list);
      g_list_free_1 (list);
    }
  list = g_list_find (class->can_call, student);
  if (list)
    {
      class->can_call = g_list_remove_link (class->can_call, list);
      class->can_call_length --;
      g_list_free_1 (list);
    }

  faircall_student_delete (class->last_called);
  class->last_called = class->last_last_called;
  class->last_last_called = NULL;
  g_mutex_unlock (&class->m);
}
