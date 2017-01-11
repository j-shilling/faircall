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

#include <check.h>
#include <time.h>
#include <stdlib.h>

#include "stats.h"
#include "../src/random.h"

#define NVALS	1000

START_TEST (test_get_rand_int)
{
  int x = 0, y = 0;

      while (x == y)
	{
	  srand (time (NULL));
	  x = rand ();

	  srand (time (NULL));
	  y = rand ();
	}

  int min = x < y ? x : y;
  int max = x > y ? x : y;

  double *list = (int *) malloc (sizeof (double) * NVALS);

  for (int i = 0; i < NVALS; i ++)
    {
      list[i] = (double) get_rand_int (min, max);
    }

  double lower_bound =
      ( lowq (list, NVALS) - ((upq (list, NVALS) - lowq (list, NVALS)) * 1.5) );
  double upper_bound =
      ( upq (list, NVALS) + ((upq (list, NVALS) - lowq (list, NVALS)) * 1.5) );

  double high, low = -1;
  for (int i = 0; i < NVALS; i ++)
    {
      if ((low < 0) || (low > list[i]))
	low = list[i];

      if (high < list[i])
	high = list[i];

      ck_assert_msg (((list[i] <= max) || (list[i] >= min)),
		     "%f is outside the required range: (%i, %i)\n",
		     list[i], min, max);

      ck_assert_msg (((list[i] <= upper_bound) || (list[i] >= lower_bound)),
		     "%f is outside the outlier range: (%f, %f)\n",
		     list[i], lower_bound, upper_bound);
    }

  double cov = coefficient_of_variance (list, NVALS);

  ck_assert_msg (cov < 1, "There is too much variance in this set: %f\n", cov);

  printf ("mean: %f\n", mean (list, NVALS));
  printf ("standard deviation: %f\n", stdev (list, NVALS));
  printf ("median: %f\n", median (list, NVALS));
  printf ("range: %f\n", (high - low));

  free (list);
} END_TEST

int
main (void)
{
  int number_failed;

  Suite *s;
  SRunner *sr;
  TCase *tc_core;

  s = suite_create ("List");
  tc_core = tcase_create ("Core");

  tcase_add_test(tc_core, test_get_rand_int);
  suite_add_tcase (s, tc_core);

  sr = srunner_create (s);

  srunner_set_fork_status (sr, CK_NOFORK);
  srunner_run_all (sr, CK_VERBOSE);
  number_failed = srunner_ntests_failed (sr);
  srunner_free (sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
