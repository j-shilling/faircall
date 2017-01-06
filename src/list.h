#ifndef __LIST_H__
#define __LIST_H__

#include <stdbool.h>

#include "student.h"

typedef struct _list_node_t {
	student_t *item;
	struct _list_node_t *next;	
} list_node_t;

extern list_node_t *new_list_node(student_t *item);
extern void free_list_node(list_node_t *node);

extern void add (list_node_t *node, student_t *student);
extern student_t *get (list_node_t *node, const unsigned int index);

extern unsigned int get_min(list_node_t *node, const unsigned int index);
extern unsigned int get_max(list_node_t *node, const unsigned int index);

extern void update (list_node_t *node, const unsigned int index);

extern unsigned int get_max_index(list_node_t *node);

extern unsigned int call_student (list_node_t *node, const unsigned int last_called, student_t **ret);

#endif
