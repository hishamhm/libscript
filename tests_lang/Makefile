
PYTHON_CPPFLAGS = -I/Programs/Python/Current/include/python2.4
PYTHON_EXTRA_LDFLAGS = -Xlinker -export-dynamic
PYTHON_EXTRA_LIBS =  -lpthread -ldl  -lutil
PYTHON_LDFLAGS = -L/Programs/Python/Current/lib/python2.4 -lpython2.4
PYTHON_FLAGS = $(PYTHON_CPPFLAGS) $(PYTHON_EXTRA_LDFLAGS) $(PYTHON_EXTRA_LIBS) $(PYTHON_LDFLAGS)
PYTHON_PROGS = pythonstring pythonconvert pythonbuiltins pythontype pythoncall

JAVA_CFLAGS = -g -shared -I/Programs/JDK/Current/include/linux/
JAVA = java -Djava.library.path=.
JAVA_PROGS = libjavatestjni.so javatestjni.h javatestjni.class

RUBY_PROC = $(shell proc=$$(uname -m);[ "$$proc" = "ppc" ] && echo powerpc || echo $$proc)

RUBY_CFLAGS = -I/Programs/Ruby/Current/lib/ruby/1.8/$(RUBY_PROC)-linux
RUBY_LDFLAGS = -rdynamic -Wl,-export-dynamic
RUBY_LIBS = -lruby-static -ldl -lcrypt -lm 
RUBY_FLAGS = $(RUBY_CFLAGS) $(RUBY_LDFLAGS) $(RUBY_LIBS)
RUBY_PROGS = rubygc rubycall rubyintern

LUA_LDFLAGS = -llua -ldl -lm
LUA_FLAGS = $(LUA_LDFLAGS)
LUA_PROGS = luafnmt luauserdata

PROGS = $(PYTHON_PROGS) $(JAVA_PROGS) $(RUBY_PROGS) $(LUA_PROGS)

all: $(PROGS)

run: run_python run_java run_ruby run_lua

run_python: $(PYTHON_PROGS)
	./pythonstring
	./pythonconvert
	./pythonbuiltins
	./pythontype
	./pythoncall

pythonstring: pythonstring.c
	gcc -o pythonstring pythonstring.c $(PYTHON_FLAGS)

pythonconvert: pythonconvert.c
	gcc -o pythonconvert pythonconvert.c $(PYTHON_FLAGS)

pythonbuiltins: pythonbuiltins.c
	gcc -o pythonbuiltins pythonbuiltins.c $(PYTHON_FLAGS)

pythontype: pythontype.c
	gcc -o pythontype pythontype.c $(PYTHON_FLAGS)

pythoncall: pythoncall.c
	gcc -o pythoncall pythoncall.c $(PYTHON_FLAGS)

run_java: $(JAVA_PROGS)
	$(JAVA) javatestjni

javatestjni.class: javatestjni.java
	javac javatestjni.java

javatestjni.h: javatestjni.class
	javah -jni javatestjni

libjavatestjni.so: javatestjni.c javatestjni.h
	gcc $(JAVA_CFLAGS) -o libjavatestjni.so javatestjni.c

run_ruby: $(RUBY_PROGS)
	./rubygc
	./rubycall

rubygc: rubygc.c
	gcc -o rubygc rubygc.c $(RUBY_FLAGS)

rubycall: rubycall.c
	gcc -o rubycall rubycall.c $(RUBY_FLAGS)

rubyintern: rubyintern.c
	gcc -o rubyintern rubyintern.c $(RUBY_FLAGS)

run_lua: $(LUA_PROGS)
	./luafnmt

luafnmt: luafnmt.c
	gcc -o luafnmt luafnmt.c $(LUA_FLAGS)

luauserdata: luauserdata.c
	gcc -o luauserdata luauserdata.c $(LUA_FLAGS)

clean:
	rm -f $(PROGS)
