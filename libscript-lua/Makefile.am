
lib_LTLIBRARIES = libscript-lua.la

libscript_lua_la_SOURCES = src/libscript-lua.c src/libscript-lua.h

AM_CFLAGS = -ansi -Wall

idsdir = $(datadir)/libscript/ids
ids_DATA = ids/lua.id

libscript_lua_la_LDFLAGS = -module -version-info 0:0:0
libscript_lua_la_LIBADD = -lm -ldl

