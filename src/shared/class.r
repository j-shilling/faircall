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

#ifndef __CLASS_R__
#define __CLASS_R__

#include <glib.h>

/* Stores information about each class */
struct Class
{
  gchar *name;                        // The unique name of this class
  gboolean forced_even;               // The method to deterime who should be called

  GMutex m;                           // Make this struct thread-safe

  struct Roster *r;                   // Student data
  struct Student *last_called;        // Who was last called
  struct Student *last_last_called;   // Who was called before that
};

#endif /* __CLASS_R__ */
