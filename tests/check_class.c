#include <check.h>
#include <faircall.h>

#include <stdlib.h>
#include <stdio.h>

START_TEST(check_class_standard_mode)
{
  struct Class *class = faircall_class_new ("Test", NULL);
  
  char buf[3];
  for (int i = 0; i < 25; i++)
    {
      sprintf (buf, "%d", i);
      faircall_class_new_student (class, buf, NULL);
    }

  for (int i = 0; i < 100; i++)
    {
      char *name = faircall_class_call_student (class);
      ck_assert_ptr_ne (name, NULL);
      free (name);
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
  suite_add_tcase (s, tc_core);

  sr = srunner_create (s);

  srunner_set_fork_status (sr, CK_NOFORK);
  srunner_run_all (sr, CK_VERBOSE);
  number_failed = srunner_ntests_failed (sr);
  srunner_free (sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
