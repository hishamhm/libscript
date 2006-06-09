

PYTHON_CPPFLAGS = -I/Programs/Python/2.4/include/python2.4
PYTHON_EXTRA_LDFLAGS = -Xlinker -export-dynamic
PYTHON_EXTRA_LIBS =  -lpthread -ldl  -lutil
PYTHON_LDFLAGS = -L/Programs/Python/2.4/lib/python2.4 -lpython2.4
PYTHON_FLAGS = $(PYTHON_CPPFLAGS) $(PYTHON_EXTRA_LDFLAGS) $(PYTHON_EXTRA_LIBS) $(PYTHON_LDFLAGS)

PYTHON_PROGS = pythonstring pythonconvert pythonbuiltins

JAVA_C_FLAGS = -shared -I/Programs/JDK/Current/include/linux/

JAVA_PROGS = libjavatestjni.so javatestjni.h javatestjni.class

JAVA = java -Djava.library.path=.

PROGS = $(PYTHON_PROGS) $(JAVA_PROGS)

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
	gcc -o libjavatestjni.so javatestjni.c $(JAVA_C_FLAGS)

clean:
	rm -f $(PROGS)
