
lib_LTLIBRARIES = libscript-python.la

libscript_python_la_SOURCES = src/libscript-python.c src/libscript-python.h

AM_CFLAGS = -ansi -Wall $(PYTHON_CPPFLAGS)

idsdir = $(datadir)/libscript/ids
ids_DATA = ids/py.id ids/python.id

libscript_python_la_LDFLAGS = -module -version-info 0:0:0 $(PYTHON_LDFLAGS)
libscript_python_la_LIBADD = $(PYTHON_EXTRA_LIBS)
