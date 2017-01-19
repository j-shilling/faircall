bin_PROGRAMS = faircall gfaircall
faircall_SOURCES = main.c \
		 student.c \
		 student.h \
		 random.h \
		 random.c \
		 list.c \
		 list.h \
		 io.c \
		 io.h \
		 class.c \
		 class.h
faircall_CFLAGS = $(XML_CFLAGS) $(GTK_CFLAGS)
faircall_LDADD= $(XML_LIBS) $(GTK_LIBS) -lm
gfaircall_SOURCES = gmain.c \
		 student.c \
		 student.h \
		 random.h \
		 random.c \
		 list.c \
		 list.h \
		 class.c \
		 class.h \
		 io.c \
		 io.h
gfaircall_CFLAGS = $(XML_CFLAGS) $(GTK_CFLAGS)
gfaircall_LDADD= $(XML_LIBS) $(GTK_LIBS) -lm
