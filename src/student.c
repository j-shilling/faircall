/*
 *    This file is part of faircall.
 *
 *    Foobar is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    Foobar is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <string.h>

#include "student.h"

student_t *new_student (char *name) {
	student_t *ret = (student_t *) malloc (sizeof (student_t));

	ret->name = (char *) malloc (strlen(name)+1);
	strcpy (ret->name, name);

	ret->times_called_on = 0;
	ret->max_index = 0;

	return ret;
}

void free_student (student_t *student) {

	if (student) {
		if (student->name) {
			free (student->name);
			student->name = NULL;
		}

		free (student);
	}

}
