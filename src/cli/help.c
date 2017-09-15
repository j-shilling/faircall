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

#include <glib.h>
#include "help.h"

static gchar *help[] =
{
"\nAvailable Commands:\n",
"\n",
"(q)uit                 Exit the program.\n",
"\n",
"(l)ist                 List saved classes.\n",
"(o)pen                 Open a saved class.\n",
"(c)lose                Close an opened class.\n",
"(i)nfo                 Show infomation about a class or student.\n"
"(e)ven                 Configure a class to call students exactly the same number of times.\n"
"\n",
"call                   Call a student.\n",
"(u)ndo                 Undo the last call.\n",
"(a)bsent               Undo the last call and mark student as absent.\n",
"\n",
"For more information about a command, type \"(h)elp [command]\"\n",
0
};

static gchar *add[] =
{
"(a)dd [name]\n",
"\n",
"Adds a new student to the currently opened class. This student's\n",
"name must be unique within the class. Type (h)elp open for more\n",
"information about opening and creating classes.\n",
0
};

static gchar *delete[] =
{
"(d)elete [name]\n",
"\n",
"Removes a student from the currently opened class. This operation\n",
"cannot be undone.\n",
0
};

static gchar *info[] =
{
"(i)nfo\n",
"\n",
"Prints details about the currently open class.\n",
"\n",
"(i)nfo [student's name]\n",
"\n",
"Prints details about the specified student within the currently\n",
"opened class.\n",
"\n",
"(i)nfo [class name]\n",
"Prints details about the specified class. This command only works\n",
"if no class is opened.\n",
0
};

static gchar *list[];
static gchar *open[];
static gchar *close[];
static gchar *undo[];
static gchar *absent[];
static gchar *even[];
static gchar *call[];
static gchar *quit[];

static gchar **help_menus[N_MENUS] =
{
  help,
  add,
  delete,
  list,
  open,
  close,
  undo,
  absent,
  even,
  call,
  quit
};

gchar **
faircall_help_menu (HelpMenu const menu)
{
  return help_menus[menu];
}
