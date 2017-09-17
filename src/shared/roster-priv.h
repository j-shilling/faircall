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

#ifndef __ROSTER_PRIV_H__
#define __ROSTER_PRIV_H__

#include <glib.h> // guint
                // gsize
#include "student-priv.h" // struct Student

struct Roster;

/************************************************************************
  CONSTRUCT AND DESTROY
*************************************************************************/

/* Create a new instance of Roster */
struct Roster *faircall_roster_new (void);

/* Free the memory used by Roster */
void faircall_roster_delete (struct Roster *const restrict self);

/************************************************************************
  GET INFORMATION AND DATA
*************************************************************************/

/* Returns the size of the Roster's internal array. This is also the total
   number of students in the roster */
gsize faircall_roster_length (struct Roster const *const self);

/* Return the number of students which have not been made uncallable */
guint faircall_roster_could_call (struct Roster const *const self);

/* Return a newly allocated array of students in this roster. The array
   is NULL-terminated */
struct Student **faircall_roster_as_array (struct Roster const *const restrict self);

/* Returns TRUE if there is a student in this roster with the given
   name */
gboolean faircall_roster_is_student (struct Roster const *const restrict self,
			             gchar const *const _name);

/************************************************************************
  EDIT
*************************************************************************/

/* Add this student to the Roster. Student will initially be callable */
void faircall_roster_add_student (struct Roster *const restrict self,
				  struct Student const *const restrict student);

/* Remove this student from the roster. */
void faircall_roster_del_student (struct Roster *const restrict self,
				  struct Student const *const restrict student);

/* Mark all students as uncallable */
void faircall_roster_set_cant_call_anyone (struct Roster *const restrict self);

/* Set all students as callable */
void faircall_roster_set_may_call_anyone (struct Roster *const restrict self);

/* Set the given student to the queue of students who must be called */
void faircall_roster_set_must_call(struct Roster *const restrict self,
				  struct Student const *const restrict student);

/* Set the given student as callable */
void faircall_roster_set_may_call(struct Roster *const restrict self,
				  struct Student const *const restrict student);

/* Set the given student as uncallable */
void faircall_roster_set_cant_call(struct Roster *const restrict self,
			  	   struct Student const *const restrict student);

/************************************************************************
  CALL
*************************************************************************/

/* Call a student and return their name */
struct Student *faircall_roster_call_student (struct Roster const *const restrict self);

#endif /* __ROSTER_PRIV_H__ */
