AC_PREREQ([2.69])
AC_INIT([faircall], [1.0], [shilling.jake@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects])

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

AX_CHECK_COMPILE_FLAG([-std=c11], [
	CFLAGS+=" -std=c11"
], [
	echo "C compiler cannot compile C11 code"
	exit -1
])

# Checks for libraries.
PKG_CHECK_MODULES([CHECK], [check >= 0.9.4])
PKG_CHECK_MODULES(XML, libxml-2.0 >= 2.4)
PKG_CHECK_MODULES([GTK], [gtk+-2.0])

# Checks for header files.
AC_CHECK_HEADERS([limits.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_RESTRICT
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([mkdir sqrt asprintf strdup strerror])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 tests/Makefile])
AC_OUTPUT
