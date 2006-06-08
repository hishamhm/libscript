
PYTHON_CPPFLAGS = -I/Programs/Python/2.4/include/python2.4
PYTHON_EXTRA_LDFLAGS = -Xlinker -export-dynamic
PYTHON_EXTRA_LIBS =  -lpthread -ldl  -lutil
PYTHON_LDFLAGS = -L/Programs/Python/2.4/lib/python2.4 -lpython2.4
PYTHON_FLAGS = $(PYTHON_CPPFLAGS) $(PYTHON_EXTRA_LDFLAGS) $(PYTHON_EXTRA_LIBS) $(PYTHON_LDFLAGS)

PROGS = pythonstring pythonconvert pythonbuiltins

all: $(PROGS)

pythonstring: pythonstring.c
	gcc -o pythonstring pythonstring.c $(PYTHON_FLAGS)

pythonconvert: pythonconvert.c
	gcc -o pythonconvert pythonconvert.c $(PYTHON_FLAGS)

pythonbuiltins: pythonbuiltins.c
	gcc -o pythonbuiltins pythonbuiltins.c $(PYTHON_FLAGS)

clean:
	rm $(PROGS)
