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

#ifndef __LIST_H__
#define __LIST_H__

#include <stdbool.h>

#include "student.h"

typedef struct _list_node_t {
	student_t *item;
	struct _list_node_t *next;	
} list_node_t;

extern list_node_t *new_list_node(student_t *item);
extern void free_list_node(list_node_t *node);

extern void add (list_node_t *node, student_t *student);
extern student_t *get(list_node_t *node, const unsigned int index);

extern unsigned int call_student (list_node_t *node, const unsigned int last_called, student_t **ret);

extern double get_odds (list_node_t *list, student_t *student, const unsigned int last_called);

#endif
