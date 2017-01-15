TESTS = check_student check_list check_random
check_PROGRAMS = check_student check_list check_random
check_student_SOURCES = check_student.c \
		    		 $(top_builddir)/src/student.c
check_list_SOURCES = check_list.c \
		    		$(top_builddir)/src/list.c \
		     		$(top_builddir)/src/student.c \
		     		$(top_builddir)/src/random.c
check_random_SOURCES = check_random.c \
					stats.c \
					stats.h \
					$(top_builddir)/src/random.c

check_student_CFLAGS = @CHECK_CFLAGS@
check_list_CFLAGS = @CHECK_CFLAGS@
check_random_CFLAGS = @CHECK_CFLAGS@

check_student_LDADD = @CHECK_LIBS@
check_list_LDADD = @CHECK_LIBS@
check_random_LDADD = @CHECK_LIBS@ -lm
