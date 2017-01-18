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

#ifndef __LIST_H__
#define __LIST_H__

#include <stdbool.h>

#include "student.h"

typedef struct _list_t list_t;

list_t *
list_new (char *class_name);
void
list_free (list_t *list);

void
list_add (list_t *list, char *name, unsigned int called, unsigned int slots);
char *
list_get_name (list_t *list, unsigned int index);
unsigned int
list_get_times_called_on (list_t * list, unsigned int index);
double
list_get_odds (list_t *list, unsigned int index);
char *
list_call_next (list_t *list);

#endif /* __LIST_H__ */
