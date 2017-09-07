TESTS = check_roster check_class
check_PROGRAMS = check_roster check_class
check_roster_SOURCES = check_roster.c
check_roster_CFLAGS = @CHECK_CFLAGS@ @GLIB_CFLAGS@ -I$(top_builddir)/src/shared
check_roster_LDADD = $(top_builddir)/src/shared/libfaircall.a \
		    @GLIB_LIBS@ @XML_LIBS@ @CHECK_LIBS@
check_class_SOURCES = check_class.c \
		      stats.c
check_class_CFLAGS = @CHECK_CFLAGS@ @GLIB_CFLAGS@ -I$(top_builddir)/src/shared
check_class_LDADD = $(top_builddir)/src/shared/libfaircall.a \
		    @GLIB_LIBS@ @XML_LIBS@ @CHECK_LIBS@
