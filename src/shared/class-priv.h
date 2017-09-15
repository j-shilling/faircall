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

#ifndef __CLASS_PRIV_H__
#define __CLASS_PRIV_H__

#include <glib.h>

struct Class;

/************************************************************************
  CONSTRUCT AND DESTROY
*************************************************************************/

/* Allocate and quickly initialize memory for a Class. This skips some
   of the checking of the safer faircall_class_new */
struct Class *faircall_class_alloc (void);

/* Create a new instance of class. Returns NULL on failure and sets
   *error */
struct Class *faircall_class_new (gchar const *const restrict name,
				  GError **error);

/* Frees memory used by a Class */
void faircall_class_delete (struct Class *class);

/************************************************************************
  GET INFORMATION AND DATA
*************************************************************************/

/* Returns the name of the class or NULL on error */
gchar * faircall_class_name (struct Class const *const restrict class);

/* Returns an array of strings explaining the current state of the class.
   On failure, NULL is returned and *error is set. */
gchar **faircall_class_info (struct Class const *const restrict class,
			     GError **error);

/* Returns the data associated with a student of a given name. Returns
   NULL on failure and sets *error */
struct Student *faircall_class_get_student (struct Class const *const restrict class,
				            gchar const *const name,
					    GError **error);

/* Returns an array of the names of students in this class. On failure
   NULL is returned and *error is set */
gchar **faircall_class_list (struct Class const *const restrict class,
			     GError **error);

/************************************************************************
  EDIT
*************************************************************************/

/* Creates a new student and adds him to this class. Sets *error on
   failure */
void faircall_class_new_student (struct Class *const restrict class,
			         gchar const *const restrict name,
			         GError **error);

/* Removes a given student from the class. Sets *error on failure */
void faircall_class_del_student (struct Class *const restrict class,
			         gchar const *const restrict name,
			         GError **error);

/* Configures whether this class should call each student exactly the
   same number of times. */
void faircall_class_set_forced_even (struct Class *const restrict class,
				     gboolean const val);

/* Checks the times each student has been called for fairness and makes
   any necessary changes. This is ment to be run in its own GThread.
   Always returns NULL. */
gpointer faircall_class_update (gpointer data);

/************************************************************************
  CALLS AND UNDOS
*************************************************************************/

/* Returns the name of the nexted called student. */
gchar * faircall_class_call_student (struct Class *const restrict class);

/* Undoes the effects of calling the last student. */
void faircall_class_uncall_student (struct Class *const restrict class);

/* Calls faircall_class_uncall_student then removes the last called
   student from the class. */
void faircall_class_absent_student (struct Class *const restrict class);

/* Calls the given number of students, ensuring the same student is not
   called twice, and returns an array of their names. Returns NULL on
   failure and sets *error */
gchar ** faircall_class_call_n_students (struct Class *const restrict class,
				guint const n,
				GError **error);

/* Increments the number of times the given student has been called. Returns
   FALSE on error and sets *error */
gboolean faircall_class_call_student_by_name (struct Class *const restrict class,
					      gchar const *const restrict name,
					      GError **error);

#endif /* __CLASS_PRIV_H__ */
