TESTS = check_student check_list
check_PROGRAMS = check_student check_list
check_student_SOURCES = check_student.c \
		     $(top_builddir)/src/student.c
check_list_SOURCES = check_list.c \
		     $(top_builddir)/src/list.c \
		     $(top_builddir)/src/student.c \
		     stats.c
check_student_CFLAGS = @CHECK_CFLAGS@ -ggdb -O0
check_list_CFLAGS = @CHECK_CFLAGS@ -ggdb -O0
check_student_LDADD = @CHECK_LIBS@
check_list_LDADD = @CHECK_LIBS@
