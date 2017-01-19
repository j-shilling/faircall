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
 * INCLUDE FILES                                                        *
 ************************************************************************/
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <assert.h>

#include "list.h"
#include "list-priv.h"
#include "random.h"
#include "asprintf.h"
#include "strdup.h"

/************************************************************************
 * CONSTRUCTOR AND DESTRUCTOR                                           *
 ************************************************************************/

List *
list_new (char *class_name)
{
  if (!class_name)
    return NULL;

  List *ret = (List *) malloc (sizeof (List *));

  ret->first_node = NULL;
  ret->max_index = 0;
  ret->last_called = 0;

  ret->class_name = strdup (class_name);

  return ret;
}

void
list_free (List *list)
{
  if (list)
    {
      if (list->first_node)
	list_node_free (list->first_node);

     // if (list->class_name)
	//free (list->class_name);

      free (list);
    }
}

/************************************************************************
 * LIST ACCESS FUNCTIONS                                                *
 ************************************************************************/

void
list_add (List *list, char *name, unsigned int called, unsigned int slots)
{
  assert (list);

  ListItem *item = (ListItem *) malloc (sizeof (ListItem));
  item->name = strdup (name);
  item->called = called;
  item->slots = slots;

  ListNode *node = (ListNode *) malloc (sizeof (ListNode));
  node->item = item;
  node->next = NULL;

  if (list->first_node)
    {
      ListNode *cur = list->first_node;

      while (cur->next)
	cur = cur->next;

      cur->next = node;
      node->prev = cur;
    }
  else
    {
      list->first_node = node;
      node->max_index = item->slots;
      node->prev = NULL;
    }

  list_set_indexes (list);
}


char *
list_get_name (List *list, unsigned int index)
{
  char *ret = NULL;

  ListItem *item = list_get_item (list, index);

  if (item)
    ret = item->name;

  return ret;
}

unsigned int
list_get_times_called_on (List * list, unsigned int index)
{
  ListItem *item = list_get_item (list, index);

  if (item)
    return item->called;

  return 0;
}

double
list_get_odds (List *list, unsigned int index)
{
  int min = get_rand_min (list);
    int max = get_rand_max (list);

    ListNode *node = list_get_node (list, index);

    if ((min > node->max_index) || (max < node->max_index))
      return 0.0;

    return ((double) node->item->slots / (double) (max - min + 1));
}

char *
list_call_next (List *list)
{
  /*
     * Get an index that will not return the same student as
     * last_called
     */
    unsigned int index = get_rand_int (
	get_rand_min (list), get_rand_max (list));

    /*
     * Get selection
     */
    ListItem *item = list_get_item (list, index);
    list->last_called = index;

    /*
     * Update the list by making whoever was called on
     * less likely to get picked and who ever wasn't more
     * likely.
     */
    if (item->slots > 1) item->slots--;

    ListNode *node = list->first_node;
    while (node)
      {
	if (node->item != item)
	  node->item->slots++;

	node = node->next;
      }

    list_set_indexes (list);

    /*
     * Return selected name
     */
    return item->name;
}

/************************************************************************
 * STATIC FUNCTIONS DEFINITIONS                                         *
 ************************************************************************/

static void
list_node_free (ListNode *node)
{
  if (node)
    {
      if (node->next)
	list_node_free (node->next);

      if (node->item)
	{
	  if (node->item->name)
	    free (node->item->name);

	  free (node->item);
	}

      free (node);
    }
}

static void
list_set_indexes (List *list)
{
  ListNode *cur = list->first_node;

  int prev = -1;
  while (cur)
    {
      if (cur->prev)
	prev = cur->prev->max_index;

      cur->max_index = prev + cur->item->slots;

      cur = cur->next;
    }
}

static ListNode *
list_get_node (List *list, unsigned int index)
{
  ListNode *cur = list->first_node;

    if (!cur)
      return NULL;

    while (cur->next)
      {
	int min = 0;

	if (cur->prev)
	  min = cur->prev->max_index + 1;

        if ((index <= cur->max_index) && (index >= min))
          return cur;

        cur = cur->next;
      }

    return NULL;
}

static ListItem *
list_get_item (List *list, unsigned int index)
{
  ListNode *node = list_get_node(list, index);

  if (node)
    return node->item;

  return NULL;
}


static unsigned int
get_rand_min (List *list)
{
  ListNode *node = list_get_node (list, list->last_called);

  unsigned int min = node->prev ?
      node->prev->max_index + 1 : 0;
  unsigned int max = node->max_index;
  unsigned int max_index = list->max_index;

  if ((max_index - max) > min)
    {
      min = max == max_index ? min = max : max + 1;
    }
  else
    {
      min = 0;
    }

  return min;
}

static unsigned int
get_rand_max (List *list)
{
  ListNode *node = list_get_node (list, list->last_called);

  unsigned int min = node->prev ?
      node->prev->max_index + 1 : 0;
  unsigned int max = node->max_index;
  unsigned int max_index = list->max_index;

  if ((max_index - max) > min)
    {
      min = max == max_index ? min = max : max + 1;
    }
  else
    {
      min = 0;
    }

  return max;
}


