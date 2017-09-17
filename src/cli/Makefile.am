BUILT_SOURCES = parser.h
AM_YFLAGS = -d
bin_PROGRAMS = faircall

faircall_SOURCES = main.c \
		   help.c \
		   parser.y \
		   scanner.l
faircall_CFLAGS = $(CFLAGS) -I$(srcdir)/../shared $(GLIB_CFLAGS) $(XML_CFLAGS)
faircall_LDADD = $(srcdir)/../shared/libfaircall.a $(GLIB_LIBS) $(XML_LIBS) -lreadline
