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

#include <stdlib.h>
#include <stdio.h>
#include <check.h>

#include "../src/list.h"
#include "../src/student.h"

START_TEST (test_new_list_node)
    {
      list_node_t *list = new_list_node (NULL);
      ck_assert(list == NULL);

      student_t *item = new_student ("one");
      list = new_list_node (item);

      ck_assert(list);
      ck_assert(list->item == item);
      ck_assert(list->next == NULL);

      free_list_node (list);
    }END_TEST

START_TEST (test_add)
    {
      student_t *one = new_student ("one");
      student_t *two = new_student ("two");
      student_t *three = new_student ("three");

      list_node_t *list = new_list_node (one);

      add (list, two);
      add (list, three);

      ck_assert(list->item);
      ck_assert_str_eq(list->item->name, one->name);

      ck_assert(list->next->item);
      ck_assert_str_eq(list->next->item->name, two->name);

      ck_assert(list->next->next->item);
      ck_assert_str_eq(list->next->next->item->name, three->name);

      ck_assert(list->item->max_index == 0);
      ck_assert(list->next->item->max_index == 1);
      ck_assert(list->next->next->item->max_index == 2);
      free_list_node (list);
    }END_TEST

START_TEST(test_get)
    {
      student_t *one = new_student ("one");
      student_t *two = new_student ("two");
      student_t *three = new_student ("three");

      list_node_t *list = new_list_node (one);

      add (list, two);
      add (list, three);

      one->max_index = 1;
      two->max_index = 5;
      three->max_index = 10;

      student_t *result = NULL;

      result = get (list, 0);
      ck_assert(result);
      ck_assert_str_eq(result->name, one->name);

      result = get (list, 3);
      ck_assert(result);
      ck_assert_str_eq(result->name, two->name);

      result = get (list, 8);
      ck_assert(result);
      ck_assert_str_eq(result->name, three->name);

      result = get (list, 10000);
      ck_assert(result == NULL);

      free_list_node (list);
    }END_TEST

START_TEST (test_call_student)
    {
      const unsigned int nstudents = 25;
      const unsigned int ntests = 1000;

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
	  ck_assert(cur);

	  if (last)
	    ck_assert_msg(strcmp (cur->name, last->name) != 0,
			  "Called student %s is the same as last student %s",
			  cur->name, last->name);
	}
    }END_TEST

int
main (void)
{
  int number_failed;

  Suite *s;
  SRunner *sr;
  TCase *tc_core;

  s = suite_create ("List");
  tc_core = tcase_create ("Core");

  tcase_add_test(tc_core, test_new_list_node);
  tcase_add_test(tc_core, test_add);
  tcase_add_test(tc_core, test_get);
  tcase_add_test(tc_core, test_call_student);
  suite_add_tcase (s, tc_core);

  sr = srunner_create (s);

  srunner_set_fork_status (sr, CK_NOFORK);
  srunner_run_all (sr, CK_VERBOSE);
  number_failed = srunner_ntests_failed (sr);
  srunner_free (sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
