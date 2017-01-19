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

#include "io.h"

#define CLASS	"class"
#define STUDENT "student"
#define NAME    "name"
#define CALLED  "called"
#define INDEX   "index"
#define SLOTS   "slots"

extern int errno;

char *file = 0;
const char *DOCNAME = "roster";

static char *
get_doc_name ();

static xmlDocPtr
open_doc ();

static xmlNodePtr
get_root (xmlDocPtr doc);

static xmlNodePtr
get_class (xmlDocPtr doc, char *class);

static void save_item (char *name, unsigned int index, bool is_last_called,
			unsigned int called, unsigned int slots,
			void *data);
