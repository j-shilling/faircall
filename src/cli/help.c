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
#include <glib/gprintf.h>
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

static gchar *list[] =
{
"(l)ist\n",
"\n",
"If a class is opened, list students in that class. Otherwise list\n",
"available classes.\n",
0
};

static gchar *open[] =
{
"(o)pen [class name]\n",
"\n",
"Opens the specified class. If [class name] matches a saved class,\n",
"that class will be opened. Otherwise, a new class will be created.\n",
0
};

static gchar *close[] =
{
"(c)lose\n",
"\n",
"Closes the currently opened class. Closing a class saves it.\n",
0
};

static gchar *undo[] =
{
"(u)ndo\n",
"\n",
"Undoes the effects of calling the last student. This is ment to\n",
"be used when the called student is out of the room for some reason\n",
"and should be called on later.\n",
0
};

static gchar *absent[] =
{
"(a)bsent\n",
"\n",
"Undoes the effects of callign the last student and then removes\n",
"that student from the class, but does not delete them from the saved\n",
"data for the class. This ensures that they will not be called again\n",
"until the class is closed a reopened. This should be used when the\n",
"called on student is absent.\n",
0
};

static gchar *even[] =
{
"(e)ven [true/false]\n",
"\n",
"Sets the currently open class to either ensure that all students are\n",
"called exactly the same number of times (when true), or to ensure that\n",
"no student is called an excessive number of times while alowing for\n",
"some variation.\n",
0
};

static gchar *call[] =
{
"call\n",
"\n",
"Calls the next student from the currently opened class.\n",
"\n",
"call [student name]\n",
"\n",
"Increments the number of times the given student has been called.\n",
"\n",
"call [integer]\n",
"\n",
"Calls the given number of students from the currently open class.\n",
0
};

static gchar *quit[] =
{
"(q)uit\n",
"\n",
"Saves the open class and ends the program.\n",
0
};

static gchar **help_menus[N_MENUS] =
{
  help,
  add,
  delete,
  info,
  list,
  open,
  close,
  undo,
  absent,
  even,
  call,
  quit
};

void
faircall_print_help_menu (enum HelpMenu const menu)
{
  for (gchar **line = help_menus[menu]; line && *line; line++)
    g_printf ("%s", *line);
}
