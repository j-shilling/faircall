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

#ifndef __HELP_H__
#define __HELP_H__

#include <glib.h> // gchar

enum HelpMenu
{
  HELP_M,
  ADD_M,
  DELETE_M,
  INFO_M,
  LIST_M,
  OPEN_M,
  CLOSE_M,
  UNDO_M,
  ABSENT_M,
  EVEN_M,
  CALL_M,
  QUIT_M,

  N_MENUS
};

void    faircall_print_help_menu (enum HelpMenu const menu);

#endif /* __HELP_H__ */
