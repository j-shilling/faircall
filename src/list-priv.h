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


/************************************************************************
 * STRUCT DEFINITIONS                                                   *
 ************************************************************************/

typedef struct _list_item_t list_item_t;
typedef struct _list_node_t list_node_t;

struct _list_item_t
{
  char *name;
  unsigned int called;
  unsigned int slots;
};

struct _list_node_t
{
  list_item_t *item;
  unsigned int max_index;
  list_node_t *next;
  list_node_t *prev;
};

struct _list_t
{
  list_node_t *first_node;
  char *class_name;
  unsigned int max_index;
  unsigned int last_called;
};

/************************************************************************
 * STATIC FUNCTIONS DECLARATIONS                                        *
 ************************************************************************/

static void
list_node_free (list_node_t *node);

static void
list_set_indexes (list_t *list);

static list_node_t *
list_get_node (list_t *list, unsigned int index);

static list_item_t *
list_get_item (list_t *list, unsigned int index);

static unsigned int
get_rand_min (list_t *list);

static unsigned int
get_rand_max (list_t *list);
