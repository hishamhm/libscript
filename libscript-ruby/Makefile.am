
lib_LTLIBRARIES = libscript-rb.la

libscript_rb_la_SOURCES = src/libscript-rb.c src/libscript-rb.h

AM_CFLAGS = -ansi -Wall $(RUBY_CFLAGS)

libscript_rb_la_LDFLAGS = -module -version-info 0:0:0 $(RUBY_LDFLAGS)
libscript_rb_la_LIBADD = $(RUBY_LIBS)

idsdir = $(datadir)/libscript/ids
ids_DATA = ids/rb.id ids/ruby.id

superclean: distclean
	rm -f *~ aclocal.m4 configure autom4te.cache Makefile.in config.h.in

