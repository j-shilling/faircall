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

#include "student.r"
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
faircall_student_delete (struct Student *student)
{
  if (!student)
    return;

  g_free (student->name);
  g_free (student);
}

gchar **
faircall_student_info (struct Student const *const restrict student,
			       GError **error)
{
  if (!student)
    {
      g_warning ("faircall_student_info called with null student.");
      return NULL;
    }

  gchar **ret = g_malloc (2 * sizeof (gchar *));

  ret[0] = g_strdup_printf ("%s called %u times\n",
			    student->name,
			    student->called_on);
  ret[1] = 0;

  return ret;
}
