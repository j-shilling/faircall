#include <check.h>
#include <stdlib.h>
#include <stdio.h>

#include "../src/shared/roster-priv.h"
#include "../src/shared/roster.r"

START_TEST (check_roster_create_and_delete)
{
  struct Roster *r = faircall_roster_new();

  ck_assert_ptr_ne (r, NULL);
  ck_assert_ptr_eq (r->arr, NULL);
  ck_assert_int_eq (r->size, 0);
  ck_assert_int_eq (r->nmemb, 0);
  ck_assert_ptr_ne (r->must_call, NULL);

  faircall_roster_delete (r);
}
END_TEST

START_TEST (check_roster_add_student)
{
  struct Student *a = faircall_student_new ("a", NULL);
  struct Student *b = faircall_student_new ("b", NULL);
  struct Student *c = faircall_student_new ("c", NULL);
  struct Student *d = faircall_student_new ("d", NULL);
  struct Student *e = faircall_student_new ("f", NULL);
  struct Student *f = faircall_student_new ("g", NULL);

  struct Roster *r = faircall_roster_new();

  faircall_roster_add_student (r, a);
  faircall_roster_add_student (r, b);
  faircall_roster_add_student (r, c);
  faircall_roster_add_student (r, d);
  faircall_roster_add_student (r, e);
  faircall_roster_add_student (r, f);
  faircall_roster_add_student (r, NULL);

  ck_assert_int_eq (r->size, 6);
  ck_assert_int_eq (r->nmemb, 6);
  ck_assert_ptr_ne (r->arr, NULL);

  int found = 0;
  for (int i = 0; i < r->nmemb; i++)
    if (r->arr[i] == a)
      found = 1;
  ck_assert_int_eq (found, 1);
  found = 0;
  for (int i = 0; i < r->nmemb; i++)
    if (r->arr[i] == b)
      found = 1;
  ck_assert_int_eq (found, 1);
  found = 0;
  for (int i = 0; i < r->nmemb; i++)
    if (r->arr[i] == c)
      found = 1;
  ck_assert_int_eq (found, 1);
  found = 0;
  for (int i = 0; i < r->nmemb; i++)
    if (r->arr[i] == d)
      found = 1;
  ck_assert_int_eq (found, 1);
  found = 0;
  for (int i = 0; i < r->nmemb; i++)
    if (r->arr[i] == e)
      found = 1;
  ck_assert_int_eq (found, 1);
  found = 0;
  for (int i = 0; i < r->nmemb; i++)
    if (r->arr[i] == f)
      found = 1;
  ck_assert_int_eq (found, 1);

  faircall_roster_delete (r);
}
END_TEST

START_TEST (check_roster_del_student)
{
  struct Student *a = faircall_student_new ("a", NULL);
  struct Student *b = faircall_student_new ("b", NULL);
  struct Student *c = faircall_student_new ("c", NULL);
  struct Student *d = faircall_student_new ("d", NULL);
  struct Student *e = faircall_student_new ("f", NULL);
  struct Student *f = faircall_student_new ("g", NULL);

  struct Roster *r = faircall_roster_new();

  faircall_roster_add_student (r, a);
  faircall_roster_add_student (r, b);
  faircall_roster_add_student (r, c);
  faircall_roster_add_student (r, d);
  faircall_roster_add_student (r, e);
  faircall_roster_add_student (r, f);

  faircall_roster_del_student (r, d);

  ck_assert_int_eq (r->size, 6);
  ck_assert_int_eq (r->nmemb, 6);
  ck_assert_ptr_ne (r->arr, NULL);

  int found = 0;
  for (int i = 0; i < r->nmemb; i++)
    if (r->arr[i] == a)
      found = 1;
  ck_assert_int_eq (found, 1);
  found = 0;
  for (int i = 0; i < r->nmemb; i++)
    if (r->arr[i] == b)
      found = 1;
  ck_assert_int_eq (found, 1);
  found = 0;
  for (int i = 0; i < r->nmemb; i++)
    if (r->arr[i] == c)
      found = 1;
  ck_assert_int_eq (found, 1);
  found = 0;
  for (int i = 0; i < r->nmemb; i++)
    if (r->arr[i] == d)
      found = 1;
  ck_assert_int_eq (found, 0);
  found = 0;
  for (int i = 0; i < r->nmemb; i++)
    if (r->arr[i] == e)
      found = 1;
  ck_assert_int_eq (found, 1);
  found = 0;
  for (int i = 0; i < r->nmemb; i++)
    if (r->arr[i] == f)
      found = 1;
  ck_assert_int_eq (found, 1);

  faircall_roster_delete (r);
}
END_TEST

START_TEST (check_roster_set_must_call)
{
  struct Student *a = faircall_student_new ("a", NULL);
  struct Student *b = faircall_student_new ("b", NULL);
  struct Student *c = faircall_student_new ("c", NULL);
  struct Student *d = faircall_student_new ("d", NULL);
  struct Student *e = faircall_student_new ("f", NULL);
  struct Student *f = faircall_student_new ("g", NULL);

  struct Roster *r = faircall_roster_new();

  faircall_roster_add_student (r, a);
  faircall_roster_add_student (r, b);
  faircall_roster_add_student (r, c);
  faircall_roster_add_student (r, d);
  faircall_roster_add_student (r, e);
  faircall_roster_add_student (r, f);

  faircall_roster_set_must_call (r, d);

  ck_assert_int_eq (r->size, 6);
  ck_assert_int_eq (r->nmemb, 6);
  ck_assert_ptr_ne (r->arr, NULL);

  int found = 0;
  for (int i = 0; i < r->nmemb; i++)
    if (r->arr[i] == a)
      found = 1;
  ck_assert_int_eq (found, 1);
  found = 0;
  for (int i = 0; i < r->nmemb; i++)
    if (r->arr[i] == b)
      found = 1;
  ck_assert_int_eq (found, 1);
  found = 0;
  for (int i = 0; i < r->nmemb; i++)
    if (r->arr[i] == c)
      found = 1;
  ck_assert_int_eq (found, 1);
  found = 0;
  for (int i = 0; i < r->nmemb; i++)
    if (r->arr[i] == d)
      found = 1;
  ck_assert_int_eq (found, 1);
  found = 0;
  for (int i = 0; i < r->nmemb; i++)
    if (r->arr[i] == e)
      found = 1;
  ck_assert_int_eq (found, 1);
  found = 0;
  for (int i = 0; i < r->nmemb; i++)
    if (r->arr[i] == f)
      found = 1;
  ck_assert_int_eq (found, 1);

  ck_assert_int_eq (g_queue_get_length (r->must_call), 1);
  ck_assert_ptr_eq (g_queue_pop_head (r->must_call), d);

  faircall_roster_delete (r);
}
END_TEST

START_TEST (check_roster_set_may_call)
{
  struct Student *a = faircall_student_new ("a", NULL);
  struct Student *b = faircall_student_new ("b", NULL);
  struct Student *c = faircall_student_new ("c", NULL);
  struct Student *d = faircall_student_new ("d", NULL);
  struct Student *e = faircall_student_new ("f", NULL);
  struct Student *f = faircall_student_new ("g", NULL);

  struct Roster *r = faircall_roster_new();

  faircall_roster_add_student (r, a);
  faircall_roster_add_student (r, b);
  faircall_roster_add_student (r, c);
  faircall_roster_add_student (r, d);
  faircall_roster_add_student (r, e);
  faircall_roster_add_student (r, f);

  faircall_roster_set_cant_call (r, d);
  faircall_roster_set_may_call (r, d);

  ck_assert_int_eq (r->size, 6);
  ck_assert_int_eq (r->nmemb, 6);
  ck_assert_ptr_ne (r->arr, NULL);

  int found = 0;
  for (int i = 0; i < r->nmemb; i++)
    if (r->arr[i] == a)
      found = 1;
  ck_assert_int_eq (found, 1);
  found = 0;
  for (int i = 0; i < r->nmemb; i++)
    if (r->arr[i] == b)
      found = 1;
  ck_assert_int_eq (found, 1);
  found = 0;
  for (int i = 0; i < r->nmemb; i++)
    if (r->arr[i] == c)
      found = 1;
  ck_assert_int_eq (found, 1);
  found = 0;
  for (int i = 0; i < r->nmemb; i++)
    if (r->arr[i] == d)
      found = 1;
  ck_assert_int_eq (found, 1);
  found = 0;
  for (int i = 0; i < r->nmemb; i++)
    if (r->arr[i] == e)
      found = 1;
  ck_assert_int_eq (found, 1);
  found = 0;
  for (int i = 0; i < r->nmemb; i++)
    if (r->arr[i] == f)
      found = 1;
  ck_assert_int_eq (found, 1);

  faircall_roster_delete (r);

}
END_TEST

START_TEST (check_roster_set_cant_call)
{
  struct Student *a = faircall_student_new ("a", NULL);
  struct Student *b = faircall_student_new ("b", NULL);
  struct Student *c = faircall_student_new ("c", NULL);
  struct Student *d = faircall_student_new ("d", NULL);
  struct Student *e = faircall_student_new ("f", NULL);
  struct Student *f = faircall_student_new ("g", NULL);

  struct Roster *r = faircall_roster_new();

  faircall_roster_add_student (r, a);
  faircall_roster_add_student (r, b);
  faircall_roster_add_student (r, c);
  faircall_roster_add_student (r, d);
  faircall_roster_add_student (r, e);
  faircall_roster_add_student (r, f);

  faircall_roster_set_cant_call (r, d);

  ck_assert_int_eq (r->size, 6);
  ck_assert_int_eq (r->nmemb, 5);
  ck_assert_ptr_ne (r->arr, NULL);

  int found = 0;
  for (int i = 0; i < r->nmemb; i++)
    if (r->arr[i] == a)
      found = 1;
  ck_assert_int_eq (found, 1);
  found = 0;
  for (int i = 0; i < r->nmemb; i++)
    if (r->arr[i] == b)
      found = 1;
  ck_assert_int_eq (found, 1);
  found = 0;
  for (int i = 0; i < r->nmemb; i++)
    if (r->arr[i] == c)
      found = 1;
  ck_assert_int_eq (found, 1);
  found = 0;
  for (int i = 0; i < r->nmemb; i++)
    if (r->arr[i] == d)
      found = 1;
  ck_assert_int_eq (found, 0);
  found = 0;
  for (int i = 0; i < r->nmemb; i++)
    if (r->arr[i] == e)
      found = 1;
  ck_assert_int_eq (found, 1);
  found = 0;
  for (int i = 0; i < r->nmemb; i++)
    if (r->arr[i] == f)
      found = 1;
  ck_assert_int_eq (found, 1);

  found = 0;
  for (int i = r->nmemb; i < r->size; i++)
    if (r->arr[i] == a)
      found = 1;
  ck_assert_int_eq (found, 0);
  found = 0;
  for (int i = r->nmemb; i < r->size; i++)
    if (r->arr[i] == b)
      found = 1;
  ck_assert_int_eq (found, 0);
  found = 0;
  for (int i = r->nmemb; i < r->size; i++)
    if (r->arr[i] == c)
      found = 1;
  ck_assert_int_eq (found, 0);
  found = 0;
  for (int i = r->nmemb; i < r->size; i++)
    if (r->arr[i] == d)
      found = 1;
  ck_assert_int_eq (found, 1);
  found = 0;
  for (int i = r->nmemb; i < r->size; i++)
    if (r->arr[i] == e)
      found = 1;
  ck_assert_int_eq (found, 0);
  found = 0;
  for (int i = r->nmemb; i < r->size; i++)
    if (r->arr[i] == f)
      found = 1;
  ck_assert_int_eq (found, 0);

  faircall_roster_delete (r);


}
END_TEST

START_TEST (check_roster_call_student_cant_call)
{
  struct Student *a = faircall_student_new ("a", NULL);
  struct Student *b = faircall_student_new ("b", NULL);
  struct Student *c = faircall_student_new ("c", NULL);
  struct Student *d = faircall_student_new ("d", NULL);
  struct Student *e = faircall_student_new ("f", NULL);
  struct Student *f = faircall_student_new ("g", NULL);

  struct Roster *r = faircall_roster_new();

  faircall_roster_add_student (r, a);
  faircall_roster_add_student (r, b);
  faircall_roster_add_student (r, c);
  faircall_roster_add_student (r, d);
  faircall_roster_add_student (r, e);
  faircall_roster_add_student (r, f);

  faircall_roster_set_cant_call (r, d);
  faircall_roster_set_cant_call (r, a);
  faircall_roster_set_cant_call (r, e);

  for (int i = 0; i < 100; i++)
    {
      ck_assert_ptr_ne (d, faircall_roster_call_student);
      ck_assert_ptr_ne (a, faircall_roster_call_student);
      ck_assert_ptr_ne (e, faircall_roster_call_student);
    }

  faircall_roster_delete (r);


}
END_TEST

START_TEST (check_roster_call_student_must_call)
{
  struct Student *a = faircall_student_new ("a", NULL);
  struct Student *b = faircall_student_new ("b", NULL);
  struct Student *c = faircall_student_new ("c", NULL);
  struct Student *d = faircall_student_new ("d", NULL);
  struct Student *e = faircall_student_new ("e", NULL);
  struct Student *f = faircall_student_new ("f", NULL);

  struct Roster *r = faircall_roster_new();

  faircall_roster_add_student (r, a);
  faircall_roster_add_student (r, b);
  faircall_roster_add_student (r, c);
  faircall_roster_add_student (r, d);
  faircall_roster_add_student (r, e);
  faircall_roster_add_student (r, f);

  struct Student *must = faircall_roster_call_student (r);
  faircall_roster_set_must_call (r, must);
  for (int i = 0; i < 100; i++)
    {
      struct Student *called = faircall_roster_call_student (r);
      if (called != must)
	{
	  printf ("Testing must call failed on test %d", i);
	  ck_assert (0);
	}

      must = faircall_roster_call_student (r);
      faircall_roster_set_must_call (r, must);
     }

  faircall_roster_delete (r);
}
END_TEST


int
main (void)
{
  int number_failed;

  Suite *s;
  SRunner *sr;
  TCase *tc_core;

  s = suite_create ("Class");
  tc_core = tcase_create ("Core");

  tcase_add_test(tc_core, check_roster_create_and_delete);
  tcase_add_test(tc_core, check_roster_add_student);
  tcase_add_test(tc_core, check_roster_del_student);
  tcase_add_test(tc_core, check_roster_set_must_call);
  tcase_add_test(tc_core, check_roster_set_may_call);
  tcase_add_test(tc_core, check_roster_set_cant_call);
  tcase_add_test(tc_core, check_roster_call_student_cant_call);
  tcase_add_test(tc_core, check_roster_call_student_must_call);
  suite_add_tcase (s, tc_core);

  sr = srunner_create (s);

  srunner_set_fork_status (sr, CK_NOFORK);
  srunner_run_all (sr, CK_VERBOSE);
  number_failed = srunner_ntests_failed (sr);
  srunner_free (sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
