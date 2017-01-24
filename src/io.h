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

#ifndef __IO_H__
#define __IO_H__

#include "list.h"

typedef enum {
  SUCCESS = 0,
  ASPRINTF_FAILED = -1,
  MKDIR_FAILED = -2,
  ACCESS_FAILED = -3,
  PARSE_FAILED = -4,
  ROOT_FAILED = -5,
  NOSUCHCLASS = -6
} io_err;

extern io_err io_set_filename (const char *name);
extern io_err io_save_list (List *list);
extern io_err io_save_list_item (List *list, unsigned int index);
extern List * io_load_list (const char *class_name);
extern char **io_get_available_classes();

#endif /* __IO_H__ */
