
CFLAGS = -g -DDEBUG -ansi -Wall

root = $(PWD)
LDFLAGS = -L$(root)/build/lib
CPPFLAGS = -I$(root)/build/include
LD_LIBRARY_PATH += :$(root)/build/lib

TESTSDIR = tests

all: libscript libscript-lua libscript-py libscript-rb tests

libscript/configure: libscript/configure.ac libscript/Makefile.am
	cd libscript; ./autogen.sh
libscript/Makefile: libscript/configure
	cd libscript; LDFLAGS=$(LDFLAGS) CPPFLAGS=$(CPPFLAGS) ./configure --prefix=$(root)/build
build/lib/libscript.so: libscript/Makefile libscript/src/*
	cd libscript; make LDFLAGS=$(LDFLAGS) CPPFLAGS=$(CPPFLAGS) all install
libscript: build/lib/libscript.so

libscript-lua/configure: libscript-lua/configure.ac libscript-lua/Makefile.am
	cd libscript-lua; ./autogen.sh
libscript-lua/Makefile: libscript-lua/configure
	cd libscript-lua; LDFLAGS=$(LDFLAGS) CPPFLAGS=$(CPPFLAGS) ./configure --prefix=$(root)/build
build/lib/libscript-lua.so: libscript-lua/Makefile libscript-lua/src/*
	cd libscript-lua; make all install
libscript-lua: build/lib/libscript-lua.so

libscript-py/configure: libscript-py/configure.ac libscript-py/Makefile.am
	cd libscript-py; ./autogen.sh
libscript-py/Makefile: libscript-py/configure
	cd libscript-py; LDFLAGS=$(LDFLAGS) CPPFLAGS=$(CPPFLAGS) ./configure --prefix=$(root)/build
build/lib/libscript-py.so: libscript-py/Makefile libscript-py/src/*
	cd libscript-py; make all install
libscript-py: build/lib/libscript-py.so

libscript-rb/configure: libscript-rb/configure.ac libscript-rb/Makefile.am
	cd libscript-rb; ./autogen.sh
libscript-rb/Makefile: libscript-rb/configure
	cd libscript-rb; LDFLAGS=$(LDFLAGS) CPPFLAGS=$(CPPFLAGS) ./configure --prefix=$(root)/build
build/lib/libscript-rb.so: libscript-rb/Makefile libscript-rb/src/*
	cd libscript-rb; make all install
libscript-rb: build/lib/libscript-rb.so

test1: $(TESTSDIR)/test1.c
	gcc -o $(TESTSDIR)/test1 $(TESTSDIR)/test1.c -lscript $(LDFLAGS) $(CPPFLAGS)

tests: test1
	$(TESTSDIR)/test1 $(TESTSDIR)/test1.lua
	$(TESTSDIR)/test1 $(TESTSDIR)/test1.py
	$(TESTSDIR)/test1 $(TESTSDIR)/test1.rb

clean:
	rm -rf build
	for dir in libscript libscript-lua libscript-py libscript-rb; \
	do cd $$dir; if test -e Makefile; then make clean; \
	fi; cd ..; done
	rm -f $(TESTSDIR)/test1

realclean: clean
	for dir in libscript libscript-lua libscript-py libscript-rb; \
	do cd $$dir; if test -e Makefile; then make maintainer-clean; \
	fi; ./autogen.sh --clean; \
	cd ..; done
