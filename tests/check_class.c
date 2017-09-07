#include <check.h>
#include <faircall.h>

#include <stdlib.h>
#include <stdio.h>

#include "../src/shared/student-priv.h"
#include "../src/shared/class-priv.h"
#include "../src/shared/roster-priv.h"
#include "stats.h"

#define N_STUDENTS    15
#define N_TESTS	      500

static void
check_for_outliers(struct Roster const *const r, int j)
{
  struct Student **students = faircall_roster_as_array (r);
  int len = faircall_roster_length (r);

  double *calls = g_malloc (sizeof (double) * len);
  for (int i = 0; i < len; i ++)
    calls[i] = (double) students[i]->called_on;

  bool ret = no_outliers (calls, len);

  if (!ret)
    {
      printf ("Outliers after %d tests\n\n", j);
      for (struct Student **cur = students;
	   *cur != 0; cur++)
	{
	  printf ("%s\t\t%d\n",
		  (*cur)->name,
		  (*cur)->called_on);
	}
      
    }

  if (calls) free (calls);
  if (students) free (students);
  ck_assert (ret);
}

static void
check_forced_even (struct Roster const *const r, int j)
{
  struct Student **students = faircall_roster_as_array (r);
  int min = 0;
  if (!students[0])
    return;
  else
    min = students[0]->called_on;

  bool ret = true;

  for (struct Student **cur = students;
       *cur; cur++)
    {
      int called_on = (*cur)->called_on;

      if (called_on > (min + 1)
	  || called_on < (min - 1))
	ret = false;

      if (called_on < min)
	min = called_on;
    }

  if (!ret)
    {
      printf ("Not even after %d tests\n\n", j);
      for (struct Student **cur = students;
	   *cur != 0; cur++)
	{
	  printf ("%s\t\t%d\n",
		  (*cur)->name,
		  (*cur)->called_on);
	}
      
    }

  if (students) free (students);
  ck_assert (ret);
}

START_TEST(check_class_standard_mode)
{
  struct Class *class = faircall_class_new ("Test", NULL);
  
  char buf[3];
  for (int i = 0; i < N_STUDENTS; i++)
    {
      sprintf (buf, "%d", i);
      faircall_class_new_student (class, buf, NULL);
    }

  for (int i = 0; i < N_TESTS; i++)
    {
      char *name = faircall_class_call_student (class);
      ck_assert_ptr_ne (name, NULL);
      free (name);

      if (i > N_STUDENTS && i % N_STUDENTS == 0)
	check_for_outliers(class->r, i);
    }

    faircall_class_delete (class);
}
END_TEST

START_TEST(check_class_forced_even)
{
  struct Class *class = faircall_class_new ("Test", NULL);
  faircall_class_set_forced_even (class, true);
  
  char buf[3];
  for (int i = 0; i < N_STUDENTS; i++)
    {
      sprintf (buf, "%d", i);
      faircall_class_new_student (class, buf, NULL);
    }

  for (int i = 0; i < N_TESTS; i++)
    {
      char *name = faircall_class_call_student (class);
      ck_assert_ptr_ne (name, NULL);
      free (name);

      if (i > N_STUDENTS && i % N_STUDENTS == 0)
	check_forced_even(class->r, i);
    }

    faircall_class_delete (class);
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

  tcase_add_test(tc_core, check_class_standard_mode);
  tcase_add_test(tc_core, check_class_forced_even);
  suite_add_tcase (s, tc_core);

  sr = srunner_create (s);

  srunner_set_fork_status (sr, CK_NOFORK);
  srunner_run_all (sr, CK_VERBOSE);
  number_failed = srunner_ntests_failed (sr);
  srunner_free (sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
