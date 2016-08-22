AC_PREREQ([2.68])
AC_INIT([execs], [0.9], [info@v2.cs.unibo.it])
AM_INIT_AUTOMAKE([foreign dist-bzip2])
AC_CONFIG_SRCDIR([execs.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_LIBTOOL
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h stdio.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE

# Checks for library functions.
AC_CHECK_FUNCS([execv execvp execvpe])
AC_CHECK_FUNCS([strlen strdup])
AC_CHECK_FUNCS([calloc free])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
