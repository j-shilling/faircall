#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#include "index.h"
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

unsigned int
call_student (list_node_t *node, const unsigned int last_called, student_t **ret)
{
	/*
	 * Get an index that will not return the same student as
	 * last_called
	 */
	unsigned int min = get_min (node, last_called);
	unsigned int max = get_max (node, last_called);
	unsigned int max_index = get_max_index (node);

	if ((max_index - max) > min) {
		min = max == max_index ? min = max : max + 1;
		max = max_index;
	} else {
		max = min == 0 ? min : min - 1;
		min = 0;
	}

	time_t t;
	srand(time(&t));

	int r = (int)((double)rand() / ((double)RAND_MAX + 1) * (max - min + 1));
	unsigned int index = r + min;

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
