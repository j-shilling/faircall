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

#include "list.h"

/************************************************************************
 * STRUCT DEFINITIONS                                                   *
 ************************************************************************/

typedef struct _ListNode ListNode;
typedef struct _RandLimits RandLimits;

struct _ListNode
{
  ListNode *next;
  ListNode *prev;

  char *name;

  unsigned int max_index;
  unsigned int called;
  unsigned int slots;
};

struct _List
{
  ListNode *first_node;
  char *class_name;
  unsigned int max_index;
  unsigned int last_called;
};

struct _RandLimits
{
  unsigned int min;
  unsigned int max;
};

/************************************************************************
 * STATIC FUNCTIONS DECLARATIONS                                        *
 ************************************************************************/

static void
list_node_free (ListNode *node);

static void
list_set_indexes (List *list);

static ListNode *
list_get_node (List *list, unsigned int index);

static RandLimits
get_rand_limits (List *list);
