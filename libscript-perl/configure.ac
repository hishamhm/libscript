
AC_PREREQ(2.59)
AC_INIT([libscript-perl], [1.0], [hisham@inf.puc-rio.br])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/libscript-perl.c])
AC_CONFIG_HEADER([config.h])

AM_PROG_LIBTOOL

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

AC_PERL_DEVEL

AC_PERL_DEFINE_CHECK([usemultiplicity], ,AC_DEFINE([P_DONT_USE_MULTIPLICITY], [1], [Perl was compiled without the -Dusemultiplicity flag.]) )

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

