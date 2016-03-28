
AC_INIT([libscript-ruby], [1.0], [hisham@inf.puc-rio.br])
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_SRCDIR([src/libscript-ruby.c])
AC_CONFIG_HEADER([config.h])

AC_PROG_LIBTOOL
AM_PROG_LIBTOOL

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

AC_RUBY_DEVEL(1.9)

#AC_CHECK_LIB([script], [script_init], , AC_MSG_ERROR([Could not find suitable LibScript library.]))

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset strchr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

