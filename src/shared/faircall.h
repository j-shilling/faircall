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

/* This file declares functions for working with the main functionality of
   Faircall. Both the CLI and GUI versions use these functions to perform
   the major tasks of the application. */

#ifndef __FAIRCALL_H__
#define __FAIRCALL_H__

#include <glib.h> /* gchar,
		     gboolean,
		     GError */

#include "error.h" /* Also define error codes */

/*************************************************************************
  OPEN AND CLOSE CLASSES
*************************************************************************/

/* Open a class for editing. If no class with the given name is saved,
   then a new class will be created. Function returns TRUE when the class
   was successfully opened. If an error occurs *error is set and FALSE
   is returned */
gboolean faircall_open_class (gchar const *const name, GError **error);

/* Closes the currently open class. Returns TRUE on success. Of failure
   FALSE is returned and *error is set. */
gboolean faircall_close_class (GError **error);

/*************************************************************************
  EDIT CLASSES
*************************************************************************/

/* Add a new student to the open class. The name must be unique within
   this class. On success, TRUE is returned; on failure, FALSE is returned
   and *error is set. */
gboolean faircall_add_student (gchar const *const name, GError **error);

/* Removes a student from the open class and deletes him from the save
   file. On success, TRUE is returned; on failure, FALSE is returned
   and *error is set. */
gboolean faircall_del_student (gchar const *const name, GError **error);

/* Undoes the effect of the last call. On success, TRUE is returned; on
   failure, FALSE is returned and *error is set. */
gboolean faircall_undo_call (GError **error);

/* Removes the last called student from the open class, but does not delete
   them from the save file. They will not be called again until the class
   is closed and re-opened. On success, TRUE is returned; on failure, FALSE
   is returned and *error is set. */
gboolean faircall_mark_absent (GError **error);

/* Configures whether the open class sured ensure that all students are
   called exactly the same number of times. On success, TRUE is returned;
   on failure, FALSE is returned and *error is set. */
gboolean faircall_set_forced_even (gboolean const value, GError **error);

/*************************************************************************
  CALL ON STUDENTS
*************************************************************************/

/* Call the next student. Returns the name of the called student or, on
   failure, returns NULL and sets *error */
gchar *  faircall_call_student (GError **error);

/* Call the given number of students, ensuring that the same student is
   not called twice. Returns and array of names or, on failure, returns
   NULL and sets *error */
gchar ** faircall_call_n_students (guint n, GError **error);

/* Increments the number of times the given student has been called. On
   success, TRUE is returned; on failure, FALSE is returned and *error is
   set. */
gboolean faircall_call_student_by_name (gchar const *const restrict name,
					GError **error);

/*************************************************************************
  GET INFOMATION TEXT
*************************************************************************/

/* Get an array of lines to be desplayed detailing information about either
   a class or a student. If name is NULL, information is about the open class.
   if name is the name of another class, that class will be used. If name is
   a student in the open class, information will be display about that student
   alone. If an error occurs, NULL is returned and *error is set. */
gchar ** faircall_info (gchar const *const restrict name, GError **error);

/* If a class is open, returns an array of students in the class. Otherwise,
   returns an array of saved classes. On error, NULL is returned and *error
   is set */
gchar ** faircall_list (GError **error);

/* Returns the name of the currently opened class or "" */
gchar *  faircall_get_open_class_name(void);

#endif /* __FAIRCALL_H__ */
