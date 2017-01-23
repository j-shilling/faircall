TESTS = check_list check_random
check_PROGRAMS = check_list check_random
check_list_SOURCES = check_list.c \
					stats.c \
					stats.h \
		    		$(top_builddir)/src/list.c \
		     		$(top_builddir)/src/random.c
check_random_SOURCES = check_random.c \
					stats.c \
					stats.h \
					$(top_builddir)/src/random.c

check_list_CFLAGS = @CHECK_CFLAGS@
check_random_CFLAGS = @CHECK_CFLAGS@

check_list_LDADD = @CHECK_LIBS@
check_random_LDADD = @CHECK_LIBS@ -lm
