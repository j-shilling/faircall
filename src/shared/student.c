#include "student-priv.h"
#include "error.h"

/* Local Error Domain and Codes */
#define FAIRCALL_STUDENT_ERROR faircall_student_error_quark()

static GQuark
faircall_student_error_quark()
{
  return g_quark_from_static_string ("faircall-student-error-quark");
}

struct Student *
faircall_student_new (gchar const *const restrict name,
		      GError **error)
{
  /* VALIDATE name */

  // Name must be present
  if (!name || !name[0])
    {
      g_set_error (error,
		   FAIRCALL_STUDENT_ERROR,
		   NO_NAME_ERROR,
		   "Every student must have a name.");
      return NULL;
    }

  // Name must be UTF8
  if (!g_utf8_validate (name, -1, NULL))
    {
      g_warning ("faircall_student_new was given a name "
		 "which was not a valid UTF8 string.");
      return NULL;
    }

  struct Student *ret =
    (struct Student *) g_malloc (sizeof (struct Student));

  ret->name = g_strdup (name);
  ret->called_on = 0;

  return ret;
}

void
faircall_student_delete (gpointer data)
{
  struct Student *student = data;

  g_free (student->name);
  g_free (student);
}


