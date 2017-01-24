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

  List *ret = (List *) malloc (sizeof (List));

  ret->first_node = NULL;
  ret->last_node = NULL;
  ret->last_called = NULL;
  ret->total_slots = 0;
  ret->size = 0;

  ret->class_name = strdup (class_name);

  return ret;
}

void
list_free (List *list)
{
  if (list)
    {
      ListNode *node = list->first_node;
      for (int i = 0; i < list->size; i++)
	{
	  if (node)
	    {
	      ListNode *this = node;
	      node = node->next;
	      list_node_free (this);
	    }
	}

      if (list->class_name)
	free (list->class_name);

      free (list);
    }
}

/************************************************************************
 * LIST ACCESS FUNCTIONS                                                *
 ************************************************************************/
void
list_set_last_called (List *list, const char *name)
{
  if (strlen (name) == 0)
    {
      list->last_called = NULL;
      return;
    }

  ListNode *node = list->first_node;
  for (int i = 0; i < list->size; i++)
    {
      if (!node)
	break;

      if (!strcmp (name, node->name))
	{
	  list->last_called = node;
	  return;
	}

      node = node->next;
    }

  list->last_called = NULL;
}

void
list_add (List *list, const char *name, unsigned int called, unsigned int slots)
{
  assert (list);

  ListNode *node = list_node_new();
  node->called = called;
  node->slots = slots;

  node->name = (char *) malloc (strlen (name) + 1);
    strcpy (node->name, name);

  if (list->first_node)
    {
      node->next = list->first_node;
      node->prev = list->last_node;

      list->first_node->prev = node;
      list->last_node->next = node;

      list->last_node = node;
    }
  else
    {
      list->first_node = node;
      list->last_node = node;

      node->next = node;
      node->prev = node;
    }

  list->total_slots += node->slots;
  list->size ++;
}


char *
list_get_name (List *list, unsigned int index)
{
  char *ret = NULL;

  ListNode *item = list_get_node (list, index);

  if (item)
    ret = item->name;

  return ret;
}

char *
list_get_class_name (List *list)
{
  return list->class_name;
}

unsigned int
list_get_times_called_on (List * list, unsigned int index)
{
  ListNode *item = list_get_node (list, index);

  if (item)
    return item->called;

  return 0;
}

double
list_get_odds (List *list, unsigned int index)
{
  ListNode *node = list_get_node (list, index);

  if (list->last_called == node)
    return 0.0;

  unsigned int sub = list->last_called ?
      list->last_called->slots : 0;
  unsigned int total = list->total_slots - sub;

    return ((double) node->slots / (double) total);
}

char *
list_call_next (List *list)
{
    int steps = get_rand_int (0,
	list->total_slots - (list->last_called ? list->last_called->slots : 0));

    ListNode *cur = list_get_node (list,
	get_rand_int (0, list->size - 1));
    ListNode *item = NULL;
    while (!item)
      {
	if (cur == list->last_called)
	  cur = cur->next;

	steps -= cur->slots;

	if (steps < 0)
	  {
	    item = cur;
	  }
      }

    /*
     * Update the list by making whoever was called on
     * less likely to get picked and who ever wasn't more
     * likely.
     */
    if (item->slots > 1) item->slots--;

    ListNode *node = list->first_node;
    for (int i = 0; i < list->size; i ++)
      {
	if (node != item)
	  node->slots++;

	node = node->next;
      }

    list->last_called = item;
    item->called ++;

    /*
     * Return selected name
     */
    return item->name;
}

char *
list_get_last_called (List *list)
{
  if (list->last_called)
    return list->last_called->name;

  return NULL;
}

unsigned int
list_get_slots (List *list, unsigned int index)
{
  ListNode *node = list_get_node (list, index);

  if (!node)
    return 0;

  return node->slots;
}

void
list_for_each (List *list, process_list_item func, void *data)
{
  ListNode *node = list->first_node;

  unsigned int index = 0;

  for (int i = 0; i < list->size; i ++)
    {
      func (node->name,
	    index, node == list->last_called,
	    node->called, node->slots,
	    data);
      index++;
      node = node->next;
    }
}

/************************************************************************
 * STATIC FUNCTIONS DEFINITIONS                                         *
 ************************************************************************/

static ListNode *
list_node_new ()
{
  ListNode *ret = (ListNode *) malloc (sizeof(ListNode));

  ret->next = NULL;
  ret->prev = NULL;
  ret->name = NULL;

  ret->called = 0;
  ret->slots = 0;

  return ret;

}
static void
list_node_free (ListNode *node)
{
  if (node)
    {
      if (node->name)
	free (node->name);

      free (node);
    }
}

static ListNode *
list_get_node (List *list, unsigned int index)
{
  if (index >= list->size)
    return NULL;

  ListNode *cur = list->first_node;

  for (int i = 0; i < index; i ++)
    {
      cur = cur->next;
    }

  return cur;
}


