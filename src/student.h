#ifndef __STUDENT_H__
#define __STUDENT_H__

typedef struct {
	char *name;
	unsigned int times_called_on;
	unsigned int max_index;
} student_t;

student_t *new_student(char *name);
void free_student(student_t *student);

#endif /* __STUDENT_H__ */
