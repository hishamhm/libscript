
lib_LTLIBRARIES = libscript-perl.la

libscript_perl_la_SOURCES = src/libscript-perl.c src/libscript-perl.h perlxsi.c libscript-perl-xs.c

BUILT_SOURCES = perlxsi.c libscript-perl-xs.c
CLEANFILES = perlxsi.c libscript-perl-xs.c

AM_CFLAGS = -ansi -Wall $(PERL_CFLAGS) -I./src

idsdir = $(datadir)/libscript/ids
ids_DATA = ids/pl.id ids/perl.id

EXTRA_DIST = typemap

libscript_perl_la_LDFLAGS = -module -version-info 0:0:0
libscript_perl_la_LIBADD = $(PERL_LDFLAGS)

perlxsi.c: Makefile.am
	perl -MExtUtils::Embed -e xsinit -- -o $@ DynaLoader LibScript

libscript-perl-xs.c: src/libscript-perl.xs
	xsubpp -csuffix "-xs.c" -prototypes $< > $@

