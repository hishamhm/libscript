
lib_LTLIBRARIES = libscript-py.la

libscript_py_la_SOURCES = src/libscript-py.c src/libscript-py.h

AM_CFLAGS = -ansi -Wall $(PYTHON_CPPFLAGS)

idsdir = $(datadir)/libscript/ids
ids_DATA = ids/py.id ids/python.id

libscript_py_la_LDFLAGS = -module -version-info 0:0:0 $(PYTHON_LDFLAGS)
libscript_py_la_LIBADD = $(PYTHON_EXTRA_LIBS)
