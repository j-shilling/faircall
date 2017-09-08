SUBDIRS = shared

bin_PROGRAMS = faircall gfaircall
faircall_SOURCES = main.c
faircall_CFLAGS = $(CFLAGS) -I$(srcdir)/shared $(GLIB_CFLAGS) $(XML_CFLAGS)
faircall_LDADD = shared/libfaircall.a $(GLIB_LIBS) $(XML_LIBS)

gfaircall_SOURCES = gmain.c
gfaircall_CFLAGS = $(CFLAGS) -I$(srcdir)/shared
gfaircall_LDADD = shared/libfaircall.a


