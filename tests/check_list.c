#include <stdlib.h>
#include <stdio.h>
#include <check.h>

#include "../src/list.h"
#include "../src/student.h"

START_TEST (test_new_list_node)
{
	list_node_t *list = new_list_node(NULL);
	ck_assert(list == NULL);

	student_t *item = new_student("one");
	list = new_list_node(item);

	ck_assert (list);
	ck_assert (list->item == item);
	ck_assert (list->next == NULL);

	free_list_node(list);
}
END_TEST

START_TEST (test_add)
{
	student_t *one = new_student ("one");
	student_t *two = new_student ("two");
	student_t *three = new_student("three");
	
	list_node_t *list = new_list_node(one);

	add (list, two);
	add (list, three);

	ck_assert (list->item);
	ck_assert_str_eq (list->item->name, one->name);

	ck_assert (list->next->item);
	ck_assert_str_eq (list->next->item->name, two->name);

	ck_assert (list->next->next->item);
	ck_assert_str_eq (list->next->next->item->name, three->name);

	ck_assert (list->item->max_index == 0);
	ck_assert (list->next->item->max_index == 1);
	ck_assert (list->next->next->item->max_index == 2);
	free_list_node(list);
}
END_TEST

START_TEST(test_get)
{
	student_t *one = new_student ("one");
	student_t *two = new_student ("two");
	student_t *three = new_student("three");

	list_node_t *list = new_list_node(one);

	add (list, two);
	add (list, three);

	one->max_index = 1;
	two->max_index = 5;
	three->max_index = 10;

	student_t *result = get(list, 0);
	ck_assert (result);
	ck_assert_str_eq (result->name, one->name);

	result = get(list, 3);
	ck_assert (result);
	ck_assert_str_eq (result->name, two->name);

	result = get(list, 8);
	ck_assert (result);
	ck_assert_str_eq (result->name, three->name);

	result = get(list, 10000);
	ck_assert (result == NULL);

	free_list_node(list);
}
END_TEST

START_TEST(test_get_min)
{
	student_t *one = new_student ("one");
	student_t *two = new_student ("two");
	student_t *three = new_student("three");

	list_node_t *list = new_list_node(one);
	add (list, two);
	add (list, three);

	one->max_index = 1;
	two->max_index = 5;
	three->max_index = 10;

	int result = get_min(list, 1);
	ck_assert_int_eq (result, 0);

	result = get_min (list, 3);
	ck_assert_int_eq (result, 2);

	result = get_min (list, 9);
	ck_assert_int_eq (result, 6);

	free_list_node(list);
}
END_TEST

START_TEST(test_get_max)
{
	student_t *one = new_student ("one");
	student_t *two = new_student ("two");
	student_t *three = new_student("three");

	list_node_t *list = new_list_node(one);

	add (list, two);
	add (list, three);

	one->max_index = 1;
	two->max_index = 5;
	three->max_index = 10;

	int result = get_max(list, 1);
	ck_assert_int_eq (result, 1);

	result = get_max (list, 3);
	ck_assert_int_eq (result, 5);

	result = get_max (list, 9);
	ck_assert_int_eq (result, 10);

	free_list_node(list);

}
END_TEST

START_TEST(test_update)
{
	student_t *one = new_student ("one");
	student_t *two = new_student ("two");
	student_t *three = new_student("three");

	list_node_t *list = new_list_node(one);

	add (list, two);
	add (list, three);

	one->max_index = 0;
	two->max_index = 1;
	three->max_index = 2;

	// one gets called on
	update (list, 0);

	ck_assert_int_eq (one  ->max_index, 0);  // 0
	ck_assert_int_eq (two  ->max_index, 2);  // 1 2
	ck_assert_int_eq (three->max_index, 4);  // 3 4
	
	// two gets called on
	update (list, 2);

	ck_assert_int_eq (one  ->max_index, 1);  // 0 1
	ck_assert_int_eq (two  ->max_index, 2);  // 2
	ck_assert_int_eq (three->max_index, 5);  // 3 4 5

	// three gets called on
	update (list, 4);

	ck_assert_int_eq (one  ->max_index, 2);  // 0 1 2
	ck_assert_int_eq (two  ->max_index, 4);  // 3 4
	ck_assert_int_eq (three->max_index, 6);  // 5 6

	// one gets called on
	update (list, 0);

	ck_assert_int_eq (one  ->max_index, 1);  // 0 1
	ck_assert_int_eq (two  ->max_index, 4);  // 2 3 4
	ck_assert_int_eq (three->max_index, 7);  // 5 6 7

	free_list_node (list);
}
END_TEST

START_TEST(test_get_max_index)
{
	list_node_t *list = new_list_node(new_student("0"));
	add (list, new_student("1"));
	add (list, new_student("2"));
	add (list, new_student("3"));
	add (list, new_student("4"));
	add (list, new_student("5"));
	add (list, new_student("6"));

	unsigned int result = get_max_index(list);
	ck_assert_int_eq (result, 6);
}
END_TEST

int
compare_ints (const void *a, const void *b)
{

	const int *da = (const int *) a;
	const int *db = (const int *) b;

	return (*da > *db) - (*da < *db);
}

void
find_outliers(int *set,
		int count)
{
	qsort (set, count, sizeof (int), compare_ints);

	int median = ((count % 2) == 0) ?
		set[count/2] + set[count/2 - 1] / 2.0
		: set[count/2];

	int q1 = ((count % 4) == 0) ?
		set[count/4] + set[count/4 - 1] / 2.0
		: set [count/4];

	int q3 = (((3 * count) % 4) == 0) ?
		set[(3 * count)/4] - set[(3 * count) / 4 - 1] / 2.0
		: set [(3 * count) / 4];

	int iqr = q3 - q1;

	int max = q3 + (iqr * 1.5);
	int min = q1 - (iqr * 1.5);

	for (int i = 0; i < count; i++) {
		printf ("%d: %d\n", i, set[i]);
		ck_assert_msg (set[i] >= min, "%d is not in [%d, %d]\n", set[i], min, max);
		ck_assert_msg (set[i] <= max, "%d is not in [%d, %d]\n", set[i], min, max);
	}
}

START_TEST (test_call_student)
{
	const unsigned int nstudents = 50;
	const unsigned int ntests = 10000000;

	/*
	 * Build List of with nstudents
	 */
	list_node_t *list = NULL;

	for (int i = 0; i < nstudents; i++)
	{
		char name[256];
		sprintf (name, "%i", i);
		student_t *student = new_student (name);

		if (list)
			add (list, student);
		else
			list = new_list_node (student);
	}

	/*
	 * Run test ntests times
	 */
	unsigned int index = 0;
	student_t *last = NULL;
	student_t *cur = NULL;

	for (int i = 0; i < ntests; i++)
	{
		last = cur;
		index = call_student (list, index, &cur);
		ck_assert (cur);

		if (last)
			ck_assert_msg (strcmp (cur->name, last->name) != 0,
					"Called student %s is the same as last student %s",
					cur->name,
					last->name);
	}

	/*
	 * Check for outliers
	 */
	int *set = (int *) malloc (sizeof (int) * nstudents);
	int final = get_max_index (list);
	int i = 0;

	while (list)
	{
		set[i] = list->item->times_called_on;

		list = list->next;
	}

	find_outliers (set, nstudents);
	free (set);


}
END_TEST

int main (void)
{
	int number_failed;

	Suite *s;
	SRunner *sr;
	TCase *tc_core;

	s = suite_create("List");
	tc_core = tcase_create("Core");

	tcase_add_test(tc_core, test_new_list_node);
	tcase_add_test(tc_core, test_add);
	tcase_add_test(tc_core, test_get);
	tcase_add_test(tc_core, test_get_min);
	tcase_add_test(tc_core, test_get_max);
	tcase_add_test(tc_core, test_update);
	tcase_add_test(tc_core, test_call_student);
	suite_add_tcase(s, tc_core);

	sr = srunner_create(s);

	srunner_set_fork_status (sr, CK_NOFORK);
	srunner_run_all(sr, CK_VERBOSE);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
