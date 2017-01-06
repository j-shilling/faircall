#include <stdlib.h>
#include <string.h>

#include "student.h"

student_t *new_student (char *name) {
	student_t *ret = (student_t *) malloc (sizeof (student_t));

	ret->name = (char *) malloc (strlen(name)+1);
	strcpy (ret->name, name);

	ret->times_called_on = 0;
	ret->max_index = 0;

	return ret;
}

void free_student (student_t *student) {

	if (student) {
		if (student->name) {
			free (student->name);
			student->name = NULL;
		}

		free (student);
	}

}
