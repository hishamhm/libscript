
CFLAGS = -g -DDEBUG -ansi -Wall

root = $(PWD)
LDFLAGS = -L$(root)/build/lib
CPPFLAGS = -I$(root)/build/include
LD_LIBRARY_PATH += :$(root)/build/lib

TESTSDIR = tests

all: libscript libscript-lua libscript-py libscript-ruby tests

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

test1: $(TESTSDIR)/test1.c
	gcc -o $(TESTSDIR)/test1 $(TESTSDIR)/test1.c -lscript $(LDFLAGS) $(CPPFLAGS)

testcall: $(TESTSDIR)/testcall.c
	gcc -o $(TESTSDIR)/testcall $(TESTSDIR)/testcall.c -lscript $(LDFLAGS) $(CPPFLAGS)

tests: test1 testcall
	$(TESTSDIR)/test1 $(TESTSDIR)/test1.lua
	$(TESTSDIR)/test1 $(TESTSDIR)/test1.py
	$(TESTSDIR)/test1 $(TESTSDIR)/test1.rb
	$(TESTSDIR)/testcall

clean:
	rm -rf build
	for dir in libscript libscript-lua libscript-python libscript-ruby; \
	do cd $$dir; if test -e Makefile; then make clean; \
	fi; cd ..; done
	rm -f $(TESTSDIR)/test1

realclean: clean
	for dir in libscript libscript-lua libscript-python libscript-ruby; \
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
