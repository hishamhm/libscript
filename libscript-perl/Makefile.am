
lib_LTLIBRARIES = libscript-perl.la

libscript_perl_la_SOURCES = src/libscript-perl.c src/libscript-perl.h

AM_CFLAGS = -ansi -Wall $(perl_CFLAGS)

idsdir = $(datadir)/libscript/ids
ids_DATA = ids/pl.id ids/perl.id

libscript_perl_la_LDFLAGS = -module -version-info 0:0:0 $(PERL_LDFLAGS)
