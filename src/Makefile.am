SUBDIRS = shared

bin_PROGRAMS = faircall gfaircall
faircall_SOURCES = main.c
faircall_CFLAGS = $(CFLAGS) -I$(srcdir)/shared
faircall_LDADD = shared/libfaircall.a

gfaircall_SOURCES = gmain.c
gfaircall_CFLAGS = $(CFLAGS) -I$(srcdir)/shared
gfaircall_LDADD = shared/libfaircall.a


