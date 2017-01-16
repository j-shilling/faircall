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

#include <stdlib.h>

#include "class.h"

struct _class_s {
  char * name;
  unsigned int size;
  char * last_called;
  unsigned int last_index;
};

struct _class_s *
new_class (char * _name, unsigned int _size, char *_last_called, unsigned int _last_index)
{
  struct _class_s *ret = (struct _class_s *) malloc (sizeof (struct _class_s));

  if (!ret)
    return NULL;

  ret->name = _name;
  ret->size = _size;
  ret->last_called = _last_called;
  ret->last_index = _last_index;

  return ret;
}

void
free_class (struct _class_s *class)
{
  if (class)
    {
      if (class->name) free (class->name);
      if (class->last_called) free (class->last_called);

      free (class);
    }
}

char *
class_get_name(class_t *class)
{
  return class->name;
}

unsigned int
class_get_size(class_t *class){
  return class->size;
}
char *
class_get_last_called(class_t *class)
{
  return class->last_called;
}

unsigned int
class_get_last_index (class_t *class)
{
  return class->last_index;
}
