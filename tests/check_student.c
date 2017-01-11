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
#include <check.h>

#include "../src/student.h"

START_TEST (test_new_student)
    {
      const char *name = "name";
      student_t *result = new_student (name);

      ck_assert(result);
      ck_assert(result->name);
      ck_assert_str_eq(result->name, name);

      ck_assert_int_eq(result->times_called_on, 0);
      ck_assert_int_eq(result->max_index, 0);

      free_student (result);
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

  tcase_add_test(tc_core, test_new_student);
  suite_add_tcase (s, tc_core);

  sr = srunner_create (s);

  srunner_run_all (sr, CK_VERBOSE);
  number_failed = srunner_ntests_failed (sr);
  srunner_free (sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
