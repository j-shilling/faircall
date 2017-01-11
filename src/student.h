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

#ifndef __STUDENT_H__
#define __STUDENT_H__

typedef struct
{
  char *name;
  unsigned int times_called_on;
  unsigned int max_index;
} student_t;

extern student_t *
new_student (char *name);
extern void
free_student (student_t *student);

#endif /* __STUDENT_H__ */
