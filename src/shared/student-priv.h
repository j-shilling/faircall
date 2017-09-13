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

#ifndef __STUDENT_PRIV_H__
#define __STUDENT_PRIV_H__

#include <glib.h>

struct Student;

/* Create a new instance of Student */
struct Student *faircall_student_new (gchar const *const restrict name,
		                      GError **error);

/* Free memory used by Student */
void faircall_student_delete (struct Student *student);

/* Returns information strings about this student or NULL on error */
gchar **faircall_student_info (struct Student const *const restrict student,
			       GError **error);

#endif /* __STUDENT_PRIV_H__ */
