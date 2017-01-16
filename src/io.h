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

#include "class.h"

typedef enum {
  ASPRINTF_FAILED = -1,
  MKDIR_FAILED = -2,
  ACCESS_FAILED = -3,
  PARSE_FAILED = -4,
  ROOT_FAILED = -5
} io_err;

extern void set_file (char *name);
extern class_t **get_class_list ();

#endif /* __IO_H__ */
