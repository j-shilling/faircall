/*
 *    This file is part of faircall.
 *
 *    Foobar is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    Foobar is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "list.h"

list_node_t *new_list_node(student_t *item) {
	if (!item)
		return NULL;

	list_node_t *ret = (list_node_t *) malloc (sizeof (list_node_t));

	ret->item = item;
	ret->next = NULL;

	return ret;
}

void free_list_node (list_node_t *node) {
	if (node) {
		if (node->next)
			free_list_node (node->next);

		free_student (node->item);
		free (node);
	}
}

void add (list_node_t *node, student_t *student) {

	if (!node) {
		return;
	}

	unsigned int index = 1;

	while (node->next) {
		
		index++;
		node = node->next;	
	}

	node->next = new_list_node(student);
	node->next->item->max_index = index;
}

student_t *get (list_node_t *node, const unsigned int index) {

	unsigned int min = 0;
	unsigned int max = 0;

	while (node) {

		max = node->item->max_index;

		if ((index >= min) && (index <= max))
			return node->item;

		node = node->next;
		min = max+1;
	}

	return NULL;
}

unsigned int get_min (list_node_t *node, const unsigned int index) {

	unsigned int min = 0;
	unsigned int max = 0;

	while (node) {

		max = node->item->max_index;

		if ((index >= min) && (index <= max))
			return min;

		node = node->next;
		min = max+1;
	}

	return 0;
}

unsigned int get_max (list_node_t *node, const unsigned int index) {

	unsigned int min = 0;
	unsigned int max = 0;

	while (node) {

		max = node->item->max_index;

		if ((index >= min) && (index <= max))
			return max;

		node = node->next;
		min = max+1;

	}

	return 0;
}

void update (list_node_t *node, const unsigned int index) {
	
	unsigned int min = 0;
	unsigned int max = 0;
	unsigned int new_min = 0;
	unsigned int range = 0;

	while (node) {
		
		max = node->item->max_index;
		range = max - min;

		if ((index >= min) && (index <= max)) {
			if (range > 0) {
				range --;
			}
		} else {
			range ++;
		}

		node->item->max_index = new_min + range;
		new_min = node->item->max_index + 1;

		min = max + 1;
		node = node->next;
	}

}

unsigned int get_max_index(list_node_t *node) {
	if (!node)
	{
		fprintf (stderr, "ERROR: cannot get the max index of a null list!\n");
		exit (EXIT_FAILURE);
	}

	while (node->next) {
		node = node->next;
	}

	return node->item->max_index;

}


static inline int
get_rand_int (int min, int max)
{
	double val = 0;
	FILE *f = fopen ("/dev/urandom", "r");

	if (f)
	{
		unsigned char byte = 0;
		fread (&byte, sizeof(byte), 1, f);

		val = (double) byte / 256.0;
	}
	else
	{
		fprintf (stderr,
			"WARNING: could not open /dev/urandom\n%s\n", strerror (errno));
		srand (time(NULL));
		val = (double) rand() / (double) RAND_MAX;
	}

	int r = (int)(val * (max - min + 1));
	return r + min;
}

static inline int
get_rand_min(list_node_t *node, const unsigned int last_called)
{
	unsigned int min = get_min (node, last_called);
	unsigned int max = get_max (node, last_called);
	unsigned int max_index = get_max_index (node);

	if ((max_index - max) > min) {
		min = max == max_index ? min = max : max + 1;
	} else {
		min = 0;
	}

	return min;
}

static inline int
get_rand_max(list_node_t *node, const unsigned int last_called)
{
	unsigned int min = get_min (node, last_called);
	unsigned int max = get_max (node, last_called);
	unsigned int max_index = get_max_index (node);

	if ((max_index - max) > min) {
		max = max_index;
	} else {
		max = min == 0 ? min : min - 1;
	}

	return max;
}

double
get_odds (list_node_t *list, student_t *student, const unsigned int last_called)
{
	int min = get_rand_min (list, last_called);
	int max = get_rand_max (list, last_called);
	
	if ((min > student->max_index) || (max < student->max_index))
		return 0.0;

	int chances = get_max(list, student->max_index) - get_min (list, student->max_index) + 1;

	return ((double) chances / (double) (max - min + 1));
}

unsigned int
call_student (list_node_t *node, const unsigned int last_called, student_t **ret)
{
	/*
	 * Get an index that will not return the same student as
	 * last_called
	 */
	unsigned int index = get_rand_int (get_rand_min(node, last_called),
			get_rand_max(node, last_called));

	/*
	 * Place new student in ret
	 */
	(*ret) = get (node, index);
	(*ret)->times_called_on++;

	update (node, index);

	/*
	 * Return an index that will return the new student
	 */
	return (*ret)->max_index;
}
