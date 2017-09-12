SUBDIRS = shared

BUILT_SOURCES = parser.h
AM_YFLAGS = -d

bin_PROGRAMS = faircall gfaircall
faircall_SOURCES = main.c \
		   parser.y \
		   scanner.l
faircall_CFLAGS = $(CFLAGS) -I$(srcdir)/shared $(GLIB_CFLAGS) $(XML_CFLAGS)
faircall_LDADD = shared/libfaircall.a $(GLIB_LIBS) $(XML_LIBS) -lreadline

gfaircall_SOURCES = gmain.c
gfaircall_CFLAGS = $(CFLAGS) -I$(srcdir)/shared
gfaircall_LDADD = shared/libfaircall.a


