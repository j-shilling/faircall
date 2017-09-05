TESTS = check_class
check_PROGRAMS = check_class
check_class_SOURCES = check_class.c
check_class_CFLAGS = @CHECK_CFLAGS@ @GLIB_CFLAGS@ -I$(top_builddir)/src/shared
check_class_LDADD = $(top_builddir)/src/shared/libfaircall.a \
		    @GLIB_LIBS@ @XML_LIBS@ @CHECK_LIBS@
