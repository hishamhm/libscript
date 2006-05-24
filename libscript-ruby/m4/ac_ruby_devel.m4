dnl contents: Ruby related macros.
dnl
dnl Copyright (C) 2004 Nikolai Weibull <source / pcppopper.org>
dnl
dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software
dnl Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA



AC_DEFUN([AC_RUBY_DEVEL],
[
  AC_PATH_PROG([cv_path_ruby], [ruby])
  if test "x$cv_path_ruby" != x; then
    AC_MSG_CHECKING([for ruby version >= $1])
    if $cv_path_ruby -e 'VERSION >= "$1" or exit 1' >/dev/null 2>/dev/null;then
      cv_ruby_version=`$cv_path_ruby -e 'print VERSION'`
      AC_MSG_RESULT([$cv_ruby_version])
      rubyhdrdir=`$cv_path_ruby -r mkmf -e 'print Config::CONFIG[["archdir"]]'\
                          || echo $hdrdir`
      RUBY_CFLAGS="-I$rubyhdrdir"

      rubylibs=`$cv_path_ruby -r rbconfig -e 'print Config::CONFIG[["LIBS"]]'`
      if test "x$rubylibs" != x; then
        RUBY_LIBS="$rubylibs"
              fi

      librubyarg=`$cv_path_ruby -r rbconfig -e 'print \
                    Config.expand(Config::CONFIG[["LIBRUBYARG"]])'`
      if test -f "$rubyhdrdir/$librubyarg"; then
        librubyarg="$rubyhdrdir/$librubyarg"
      else
        librubyarg=`$cv_path_ruby -r rbconfig -e "print \
                    '$librubyarg'.gsub(/-L\./, %'-L#{ \
                      Config.expand(Config::CONFIG[[\"libdir\"]])}')"`
      fi
      if test "x$librubyarg" != x; then
        RUBY_LIBS="$librubyarg $RUBY_LIBS"
      fi

      RUBY_LDFLAGS=`$cv_path_ruby -r rbconfig -e 'print \
                    Config::CONFIG[["LDFLAGS"]]'`
    else
      AC_MSG_RESULT([too old; need Ruby version $1 or newer])
          fi
  fi

  AC_SUBST([RUBY_CFLAGS])
  AC_SUBST([RUBY_LIBS])
  AC_SUBST([RUBY_LDFLAGS])
])
