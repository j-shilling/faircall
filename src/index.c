#include <time.h>
#include <stdlib.h>

#include "index.h"

unsigned int get_next_index (list_node_t *node, const unsigned int last_index) {

	unsigned int min = get_min (node, last_index);
	unsigned int max = get_max (node, last_index);
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
	unsigned int ret = r + min;

	return ret;

}
