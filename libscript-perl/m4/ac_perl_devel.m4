
dnl Perl detection macro
dnl
dnl Based on http://crystal.cvs.sourceforge.net/crystal/CS/mk/autoconf/crystal.m4
dnl from Crystal Space
dnl and http://autoconf-archive.cryp.to/ac_prog_perl_modules.html
dnl from the Autoconf Archive

AC_DEFUN([CS_PATH_NORMALIZE],
	[`echo "x$1" | tr '\\\\' '/' | sed 's/^x//;s/   */ /g;s/^ //;s/ $//'`]
)

AC_DEFUN(AC_PERL_DEFINE_CHECK,
	[
		if $PERL -V | grep config_args | tr ' ' '\n' | grep -q "^-D$1"
		then
			ifelse($3,,:,[$3])
		else
			ifelse($4,,:,[$4])
		fi
	]
)

AC_DEFUN(AC_PERL_DEVEL,
	[
		AC_ARG_WITH(perl-prefix,
			[  --with-perl-prefix=PFX       prefix where perl is installed.],
			[],
		)
		
		if test -n "$with_perl_prefix"
		then
			PERL_BIN_PATH=$with_perl_prefix"/bin"
		else
			PERL_BIN_PATH="$PATH"
		fi
		
		PERL=""
		
		AC_PATH_PROG(PERL,perl,no,$PERL_BIN_PATH)
		
		ac_cv_perl_valid=no
		
		if test "$PERL" != no
		then
			AC_CACHE_CHECK(
				[for perl ExtUtils::Embed module],
				[ac_cv_perl_mod_embed],
				[
					if AC_RUN_LOG([$PERL -MExtUtils::Embed -e 0 1>&2])
					then
						ac_cv_perl_mod_embed=yes
					else
						ac_cv_perl_mod_embed=no
					fi
				]
			)
			if test $ac_cv_perl_mod_embed = yes
			then
				AC_CACHE_CHECK(
					[for perl DynaLoader module],
					[ac_cv_perl_mod_dynaloader],
					[
						if AC_RUN_LOG([$PERL -MDynaLoader -e 0 1>&2])
						then
							ac_cv_perl_mod_dynaloader=yes
						else
							ac_cv_perl_mod_dynaloader=no
						fi
					]
				)
				if test $ac_cv_perl_mod_dynaloader = yes
				then
					ac_cv_perl_cflags=`AC_RUN_LOG([$PERL -MExtUtils::Embed -e ccopts])`
					ac_cv_perl_lflags=`AC_RUN_LOG([$PERL -MExtUtils::Embed -e ldopts])`
					ac_cv_perl_ext=`AC_RUN_LOG([$PERL -MConfig '-e $e = $Config{"dlext"} || $Config{"so"} || ""; print "$e"'])`
					ac_cv_perl_cflags=CS_PATH_NORMALIZE([$ac_cv_perl_cflags])
					ac_cv_perl_lflags=CS_PATH_NORMALIZE([$ac_cv_perl_lflags])
					CS_CHECK_BUILD([if Perl SDK is usable], [ac_cv_perl],
						[
							AC_LANG_PROGRAM(
								[[#include <EXTERN.h>
								#include <perl.h>]],
								[perl_run(0);]
							)
						],
						[CS_CREATE_TUPLE([$ac_cv_perl_cflags],[],[$ac_cv_perl_lflags])]
					)
					if test $ac_cv_perl = yes
					then
						ac_cv_perl_valid=yes
					fi
				else
					AC_MSG_NOTICE([DynaLoader absent. Recommend you install a recent release of Perl 5.])
					AC_MSG_NOTICE([http://www.perl.org/])
				fi
			else
				AC_MSG_NOTICE([ExtUtils::Embed absent. Recommend you install a recent release of Perl 5.])
				AC_MSG_NOTICE([http://www.perl.org/])
			fi
		fi
		
		if test "x$ac_cv_perl_valid" = "xno" ; then
			AC_MSG_ERROR([perl status: **** suitable version NOT FOUND])
		else
			AC_MSG_RESULT([perl status: **** suitable version FOUND])

			PERL_CFLAGS=$ac_cv_perl_cflags
			AC_SUBST(PERL_CFLAGS)
					
			PERL_LDFLAGS=$ac_cv_perl_lflags
			AC_SUBST(PERL_LDFLAGS)
		fi
	]
)
