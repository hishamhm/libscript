
lib_LTLIBRARIES = libscript-ruby.la

libscript_ruby_la_SOURCES = src/libscript-ruby.c src/libscript-ruby.h

AM_CFLAGS = -ansi -Wall $(RUBY_CFLAGS)

libscript_ruby_la_LDFLAGS = -module -version-info 0:0:0 $(RUBY_LDFLAGS)
libscript_ruby_la_LIBADD = $(RUBY_LIBS)

idsdir = $(datadir)/libscript/ids
ids_DATA = ids/rb.id ids/ruby.id

superclean: distclean
	rm -f *~ aclocal.m4 configure autom4te.cache Makefile.in config.h.in

