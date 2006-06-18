
PYTHON_CPPFLAGS = -I/Programs/Python/2.4/include/python2.4
PYTHON_EXTRA_LDFLAGS = -Xlinker -export-dynamic
PYTHON_EXTRA_LIBS =  -lpthread -ldl  -lutil
PYTHON_LDFLAGS = -L/Programs/Python/2.4/lib/python2.4 -lpython2.4
PYTHON_FLAGS = $(PYTHON_CPPFLAGS) $(PYTHON_EXTRA_LDFLAGS) $(PYTHON_EXTRA_LIBS) $(PYTHON_LDFLAGS)
PYTHON_PROGS = pythonstring pythonconvert pythonbuiltins

JAVA_CFLAGS = -g -shared -I/Programs/JDK/Current/include/linux/
JAVA = java -Djava.library.path=.
JAVA_PROGS = libjavatestjni.so javatestjni.h javatestjni.class

RUBY_CFLAGS = -I/Programs/Ruby/1.8.4/lib/ruby/1.8/i686-linux
RUBY_LDFLAGS = -rdynamic -Wl,-export-dynamic
RUBY_LIBS = -lruby-static -ldl -lcrypt -lm 
RUBY_FLAGS = $(RUBY_CFLAGS) $(RUBY_LDFLAGS) $(RUBY_LIBS)

RUBY_PROGS = rubygc

PROGS = $(PYTHON_PROGS) $(JAVA_PROGS) $(RUBY_PROGS)

all: $(PROGS)

run: run_python run_java

run_python: $(PYTHON_PROGS)
	./pythonstring
	./pythonconvert
	./pythonbuiltins

pythonstring: pythonstring.c
	gcc -o pythonstring pythonstring.c $(PYTHON_FLAGS)

pythonconvert: pythonconvert.c
	gcc -o pythonconvert pythonconvert.c $(PYTHON_FLAGS)

pythonbuiltins: pythonbuiltins.c
	gcc -o pythonbuiltins pythonbuiltins.c $(PYTHON_FLAGS)

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

rubygc: rubygc.c
	gcc -o rubygc rubygc.c $(RUBY_FLAGS)

clean:
	rm -f $(PROGS)
