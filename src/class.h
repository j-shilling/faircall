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

#ifndef __CLASS_H__
#define __CLASS_H__

typedef struct _class_s class_t;

extern class_t *new_class (char * name, unsigned int size, char * last_called, unsigned int last_index);
extern void free_class (class_t *class);

extern char *class_get_name(class_t *class);
extern unsigned int class_get_size(class_t *class);
extern char *class_get_last_called(class_t *class);
extern unsigned int class_get_last_index (class_t *class);

extern void class_set_last_index (class_t *class, unsigned int index);

#endif /* __CLASS_H__ */
