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

#ifndef __ROSTER_R__
#define __ROSTER_R__

#include <glib.h> // gsize
	          // guint

#include "student-priv.h" // struct Student

/* Holds an array of students */
struct Roster
{
  struct Student **arr; // Internal array
  gsize size;           // Size of arr
  guint nmemb;          // Number of callable students

  GQueue *must_call;    // Students who must be called

  GMutex m;             // Make Roster thread-safe
};

#endif /* __ROSTER_R__ */
