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

#include <glib.h> // gboolean
		  // GError

#include "class-priv.h" // struct Class;

/* Write class data to fill. Returns TRUE on success; on failure,
   *error is set and FALSE is returned */
gboolean faircall_io_save_class(struct Class const *const class,
		                GError **error);

/* Read class data from file. On Failure *error is set and NULL is
   returned. */
struct Class *faircall_io_get_class (gchar const *const restrict name,
		                     GError **error);

/* Checks whether a name is the name of a saved class. */
gboolean faircall_io_is_class (gchar const *const restrict name,
		               GError **error);

/* Returns an array of saved class names */
gchar ** faircall_io_saved_classes (GError **error);

/* Delete a student from the saved data for a class. Returns TRUE on
   success; on failure, *error is set and FALSE is returned */
gboolean faircall_io_del_student (gchar const *const restrict classname,
				  gchar const *const restrict name,
				  GError **error);

/* Delete the saved data for a class. Returns TRUE on success; on failure,
   *error is set and FALSE is returned */
gboolean faircall_io_del_class (gchar const *const restrict name,
			        GError **error);

#endif /* __IO_H__ */
