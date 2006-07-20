
lib_LTLIBRARIES = libscript-perl.la

libscript_perl_la_SOURCES = src/libscript-perl.c src/libscript-perl.h src/perlxsi.c src/libscript-perl-xs.c

AM_CFLAGS = -ansi -Wall $(PERL_CFLAGS)

idsdir = $(datadir)/libscript/ids
ids_DATA = ids/pl.id ids/perl.id

EXTRA_DIST = typemap

libscript_perl_la_LDFLAGS = -module -version-info 0:0:0
libscript_perl_la_LIBADD = $(PERL_LDFLAGS)

src/perlxsi.c: Makefile.am
	perl -MExtUtils::Embed -e xsinit -- -o src/perlxsi.c DynaLoader LibScript

src/libscript-perl-xs.c: src/libscript-perl.xs
	xsubpp -csuffix "-xs.c" -prototypes src/libscript-perl.xs > src/libscript-perl-xs.c

