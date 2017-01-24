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

#include "stats.h"

#include "../src/list.h"
#include "../src/list-priv.h"

START_TEST (test_list_new)
    {
      List *list = list_new (NULL);
      ck_assert(list == NULL);

      list = list_new ("class1");

      ck_assert_ptr_ne(list, NULL);
      ck_assert_ptr_eq(list->first_node, NULL);

      ck_assert_int_eq(list->size, 0);

      list_free (list);
    }END_TEST

START_TEST (test_list_add)
    {
      List *list = list_new ("class1");

      list_add (list, "one", 1, 3);
      list_add (list, "two", 3, 2);
      list_add (list, "three", 2, 4);
      list_add (list, "four", 3, 3);
      list_add (list, "five", 2, 1);

      ListNode *cur = list->first_node;

      ck_assert_ptr_ne(cur, NULL);
      ck_assert_str_eq(cur->name, "one");
      ck_assert_int_eq(cur->called, 1);
      ck_assert_int_eq(cur->slots, 3);
      ck_assert_ptr_ne(cur->prev, NULL);

      cur = cur->next;

      ck_assert_ptr_ne(cur, NULL);
      ck_assert_str_eq(cur->name, "two");
      ck_assert_int_eq(cur->called, 3);
      ck_assert_int_eq(cur->slots, 2);
      ck_assert_ptr_ne(cur->prev, NULL);

      cur = cur->next;

      ck_assert_ptr_ne(cur, NULL);
      ck_assert_str_eq(cur->name, "three");
      ck_assert_int_eq(cur->called, 2);
      ck_assert_int_eq(cur->slots, 4);
      ck_assert_ptr_ne(cur->prev, NULL);

      cur = cur->next;

      ck_assert_ptr_ne(cur, NULL);
      ck_assert_str_eq(cur->name, "four");
      ck_assert_int_eq(cur->called, 3);
      ck_assert_int_eq(cur->slots, 3);
      ck_assert_ptr_ne(cur->prev, NULL);

      cur = cur->next;

      ck_assert_ptr_ne(cur, NULL);
      ck_assert_str_eq(cur->name, "five");
      ck_assert_int_eq(cur->called, 2);
      ck_assert_int_eq(cur->slots, 1);
      ck_assert_ptr_ne(cur->prev, NULL);

    }END_TEST

START_TEST(test_get_name)
    {
      List *list = list_new ("class1");

      list_add (list, "one", 1, 3);
      list_add (list, "two", 3, 2);
      list_add (list, "three", 2, 4);
      list_add (list, "four", 3, 3);
      list_add (list, "five", 2, 1);

      ck_assert_str_eq("one", list_get_name (list, 0));

      ck_assert_str_eq("two", list_get_name (list, 1));

      ck_assert_str_eq("three", list_get_name (list, 2));

      ck_assert_str_eq("four", list_get_name (list, 3));

      ck_assert_str_eq("five", list_get_name (list, 4));

      ck_assert_ptr_eq(list_get_name (list, 112), NULL);

      list_free (list);
    }END_TEST

START_TEST(test_get_times_called_on)
    {
      List *list = list_new ("class1");

      list_add (list, "one", 1, 3);
      list_add (list, "two", 3, 2);
      list_add (list, "three", 2, 4);
      list_add (list, "four", 3, 3);
      list_add (list, "five", 2, 1);

      ck_assert_int_eq(1, list_get_times_called_on (list, 0));
      ck_assert_int_eq(3, list_get_times_called_on (list, 1));
      ck_assert_int_eq(2, list_get_times_called_on (list, 2));
      ck_assert_int_eq(3, list_get_times_called_on (list, 3));
      ck_assert_int_eq(2, list_get_times_called_on (list, 4));

      list_free (list);
    }END_TEST

START_TEST (test_call_next)
    {
      const unsigned int nstudents = 25;
      const unsigned int ntests = 1000;

      /*
       * Build List of with nstudents
       */
      List *list = list_new ("class1");

      for (int i = 0; i < nstudents; i++)
	{
	  char name[256];
	  sprintf (name, "%i", i);

	  list_add (list, name, 0, 1);
	}

      /*
       * Run test ntests times
       */
      char *last = NULL;
      char *this = NULL;

      for (int i = 0; i < ntests; i++)
	{
	  last = this;
	  this = list_call_next (list);

	  if (last)
	    ck_assert_msg(strcmp (last, this) != 0,
			  "Called student %s is the same as last student %s",
			  this, last);
	}
    }END_TEST

START_TEST(test_fairness)
    {
      const unsigned int nstudents = 22;
      const unsigned int ntests = 100000;

      /*
       * Build List of with nstudents
       */
      List *list = list_new ("class1");

      for (int i = 0; i < nstudents; i++)
	{
	  char name[256];
	  sprintf (name, "%i", i);

	  list_add (list, name, 0, 1);
	}

      /*
       * Run test ntests times
       */

      for (int i = 0; i < ntests; i++)
	{
	  list_call_next (list);
	}

      /*
       * Check list stats
       */

      double vals[nstudents];

      ListNode *node = list->first_node;
      int i = 0;
      for (int i = 0; i < list->size; i ++)
	{
	  vals[i] = node->called;
	  node = node->next;
	}

      double lower_bound = (lowq (vals, nstudents)
	  - ((upq (vals, nstudents) - lowq (vals, nstudents)) * 1.5));
      double upper_bound = (upq (vals, nstudents)
	  + ((upq (vals, nstudents) - lowq (vals, nstudents)) * 1.5));

      double high, low = -1;
      for (int i = 0; i < nstudents; i++)
	{
	  if ((low < 0) || (low > vals[i]))
	    low = vals[i];

	  if (high < vals[i])
	    high = vals[i];

	  ck_assert_msg(((vals[i] <= upper_bound) || (vals[i] >= lower_bound)),
			"%f is outside the outlier range: (%f, %f)\n", vals[i],
			lower_bound, upper_bound);
	}

      double cov = coefficient_of_variance (vals, nstudents);

      ck_assert_msg(cov < 1, "There is too much variance in this set: %f\n",
		    cov);

      printf ("mean: %f\n", mean (vals, nstudents));
      printf ("standard deviation: %f\n", stdev (vals, nstudents));
      printf ("median: %f\n", median (vals, nstudents));
      printf ("range: %f\n", (high - low));
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

  tcase_add_test(tc_core, test_list_new);
  tcase_add_test(tc_core, test_list_add);
  tcase_add_test(tc_core, test_get_name);
  tcase_add_test(tc_core, test_get_times_called_on);
  tcase_add_test(tc_core, test_call_next);
  tcase_add_test(tc_core, test_fairness);
  suite_add_tcase (s, tc_core);

  sr = srunner_create (s);

  srunner_set_fork_status (sr, CK_NOFORK);
  srunner_run_all (sr, CK_VERBOSE);
  number_failed = srunner_ntests_failed (sr);
  srunner_free (sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
