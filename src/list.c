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

/************************************************************************
 * CONSTRUCTOR AND DESTRUCTOR                                           *
 ************************************************************************/

list_t *
list_new (char *class_name)
{
  if (!class_name)
    return NULL;

  list_t *ret = (list_t *) malloc (sizeof (list_t *));

  ret->first_node = NULL;
  ret->max_index = 0;
  ret->last_called = 0;

  ret->class_name = strdup (class_name);

  return ret;
}

void
list_free (list_t *list)
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
list_add (list_t *list, char *name, unsigned int called, unsigned int slots)
{
  assert (list);

  list_item_t *item = (list_item_t *) malloc (sizeof (list_item_t));
  item->name = strdup (name);
  item->called = called;
  item->slots = slots;

  list_node_t *node = (list_node_t *) malloc (sizeof (list_node_t));
  node->item = item;
  node->next = NULL;

  if (list->first_node)
    {
      list_node_t *cur = list->first_node;

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
list_get_name (list_t *list, unsigned int index)
{
  char *ret = NULL;

  list_item_t *item = list_get_item (list, index);

  if (item)
    ret = item->name;

  return ret;
}

unsigned int
list_get_times_called_on (list_t * list, unsigned int index)
{
  list_item_t *item = list_get_item (list, index);

  if (item)
    return item->called;

  return 0;
}

double
list_get_odds (list_t *list, unsigned int index)
{
  int min = get_rand_min (list);
    int max = get_rand_max (list);

    list_node_t *node = list_get_node (list, index);

    if ((min > node->max_index) || (max < node->max_index))
      return 0.0;

    return ((double) node->item->slots / (double) (max - min + 1));
}

char *
list_call_next (list_t *list)
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
    list_item_t *item = list_get_item (list, index);
    list->last_called = index;

    /*
     * Update the list by making whoever was called on
     * less likely to get picked and who ever wasn't more
     * likely.
     */
    if (item->slots > 1) item->slots--;

    list_node_t *node = list->first_node;
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
list_node_free (list_node_t *node)
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
list_set_indexes (list_t *list)
{
  list_node_t *cur = list->first_node;

  int prev = -1;
  while (cur)
    {
      if (cur->prev)
	prev = cur->prev->max_index;

      cur->max_index = prev + cur->item->slots;

      cur = cur->next;
    }
}

static list_node_t *
list_get_node (list_t *list, unsigned int index)
{
  list_node_t *cur = list->first_node;

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

static list_item_t *
list_get_item (list_t *list, unsigned int index)
{
  list_node_t *node = list_get_node(list, index);

  if (node)
    return node->item;

  return NULL;
}


static unsigned int
get_rand_min (list_t *list)
{
  list_node_t *node = list_get_node (list, list->last_called);

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
get_rand_max (list_t *list)
{
  list_node_t *node = list_get_node (list, list->last_called);

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


