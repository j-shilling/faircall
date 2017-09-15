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

#ifndef __COMPARE_H__
#define __COMPARE_H__

/* Comparator functions which can be passed to qsort */

int faircall_strcmp (const void *_x, const void *_y);
int faircall_student_cmp (const void *_x, const void *_y);
int faircall_student_cmp_str (const void *_x, const void *_y);
int faircall_class_cmp (const void *_x, const void *_y);
int faircall_class_cmp_str (const void *_x, const void *_y);

#endif /* __COMPARE_H__ */
