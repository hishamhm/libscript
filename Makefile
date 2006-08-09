
export CFLAGS = -g -DDEBUG -ansi -Wall

root = $(PWD)
BUILD_LIBDIR=$(root)/build/lib
LDFLAGS = -L$(BUILD_LIBDIR)
CPPFLAGS = -I$(root)/build/include
LD_LIBRARY_PATH += :$(root)/build/lib

TESTSDIR = tests

LIBSCRIPT_MODULES = libscript libscript-lua libscript-python libscript-ruby libscript-perl

all: $(LIBSCRIPT_MODULES) $(TESTS)

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

libscript-python/configure: libscript-python/configure.ac libscript-python/Makefile.am
	cd libscript-python; ./autogen.sh
libscript-python/Makefile: libscript-python/configure
	cd libscript-python; LDFLAGS=$(LDFLAGS) CPPFLAGS=$(CPPFLAGS) ./configure --prefix=$(root)/build
build/lib/libscript-python.so: libscript-python/Makefile libscript-python/src/*
	cd libscript-python; make all install
libscript-python: build/lib/libscript-python.so

libscript-ruby/configure: libscript-ruby/configure.ac libscript-ruby/Makefile.am
	cd libscript-ruby; ./autogen.sh
libscript-ruby/Makefile: libscript-ruby/configure
	cd libscript-ruby; LDFLAGS=$(LDFLAGS) CPPFLAGS=$(CPPFLAGS) ./configure --prefix=$(root)/build
build/lib/libscript-ruby.so: libscript-ruby/Makefile libscript-ruby/src/*
	cd libscript-ruby; make all install
libscript-ruby: build/lib/libscript-ruby.so

libscript-perl/configure: libscript-perl/configure.ac libscript-perl/Makefile.am
	cd libscript-perl; ./autogen.sh
libscript-perl/Makefile: libscript-perl/configure
	cd libscript-perl; LDFLAGS=$(LDFLAGS) CPPFLAGS=$(CPPFLAGS) ./configure --prefix=$(root)/build
build/lib/libscript-perl.so: libscript-perl/Makefile libscript-perl/src/*
	cd libscript-perl; make all install
libscript-perl: build/lib/libscript-perl.so

test1: $(TESTSDIR)/test1.c
	gcc -o $(TESTSDIR)/test1 $(TESTSDIR)/test1.c -lscript $(LDFLAGS) $(CPPFLAGS)

testcall: $(TESTSDIR)/testcall.c
	gcc -o $(TESTSDIR)/testcall $(TESTSDIR)/testcall.c -lscript $(LDFLAGS) $(CPPFLAGS)

testexc: $(TESTSDIR)/testexc.c
	gcc -o $(TESTSDIR)/testexc $(TESTSDIR)/testexc.c -lscript $(LDFLAGS) $(CPPFLAGS)

testexcrun: $(TESTSDIR)/testexcrun.c
	gcc -o $(TESTSDIR)/testexcrun $(TESTSDIR)/testexcrun.c -lscript $(LDFLAGS) $(CPPFLAGS)

tests: test1 testcall testexc testexcrun
	LD_LIBRARY_PATH=$(BUILD_LIBDIR) $(TESTSDIR)/test1 $(TESTSDIR)/test1.lua
	LD_LIBRARY_PATH=$(BUILD_LIBDIR) $(TESTSDIR)/test1 $(TESTSDIR)/test1.py
	LD_LIBRARY_PATH=$(BUILD_LIBDIR) $(TESTSDIR)/test1 $(TESTSDIR)/test1.rb
	LD_LIBRARY_PATH=$(BUILD_LIBDIR) $(TESTSDIR)/test1 $(TESTSDIR)/test1.pl
	LD_LIBRARY_PATH=$(BUILD_LIBDIR) $(TESTSDIR)/testcall
	LD_LIBRARY_PATH=$(BUILD_LIBDIR) $(TESTSDIR)/testexc $(TESTSDIR)/testexc.rb
	LD_LIBRARY_PATH=$(BUILD_LIBDIR) $(TESTSDIR)/testexc $(TESTSDIR)/testexc.lua
	LD_LIBRARY_PATH=$(BUILD_LIBDIR) $(TESTSDIR)/testexc $(TESTSDIR)/testexc.py
	LD_LIBRARY_PATH=$(BUILD_LIBDIR) $(TESTSDIR)/testexcrun $(TESTSDIR)/testexcrun.pl

clean:
	rm -rf build
	for dir in $(LIBSCRIPT_MODULES); \
	do cd $$dir; if test -e Makefile; then make clean; \
	fi; cd ..; done
	rm -f $(TESTSDIR)/test1
	rm -f $(TESTSDIR)/testcall

realclean: clean
	for dir in $(LIBSCRIPT_MODULES); \
	do cd $$dir; if test -e Makefile; then make maintainer-clean; \
	fi; ./autogen.sh --clean; \
	cd ..; done

gobo: realclean
	cd libscript                                       ;\
	./autogen.sh                                       ;\
	configure --prefix=/Programs/LibScript/svn/        ;\
	make                                               ;\
	sudo make install                                  ;\
	sudo SymlinkProgram LibScript
	cd libscript-lua                                   ;\
	./autogen.sh                                       ;\
	configure --prefix=/Programs/LibScript-Lua/svn/    ;\
	make                                               ;\
	sudo make install                                  ;\
	sudo SymlinkProgram LibScript-Lua
	cd libscript-ruby                                  ;\
	./autogen.sh                                       ;\
	configure --prefix=/Programs/LibScript-Ruby/svn/   ;\
	make                                               ;\
	sudo make install                                  ;\
	sudo SymlinkProgram LibScript-Ruby
	cd libscript-python                                ;\
	./autogen.sh                                       ;\
	configure --prefix=/Programs/LibScript-Python/svn/ ;\
	make                                               ;\
	sudo make install                                  ;\
	sudo SymlinkProgram LibScript-Python
