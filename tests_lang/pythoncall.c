
#include <Python.h>

int main() {
   PyObject *globals, *foo, *bar, *noargs, *ret;
   Py_Initialize();
   PyRun_SimpleString(
      "def foo(): \n"
      "   return 2, 10 \n"
      "def bar(): \n"
      "   return \n"
   );
   globals = PyModule_GetDict(PyImport_AddModule("__main__"));
   foo = PyDict_GetItemString(globals, "foo");
   noargs = PyTuple_New(0);
   ret = PyObject_Call(foo, noargs, NULL);
   printf("foo returned (%d, %d)\n", PyInt_AsLong(PyTuple_GET_ITEM(ret, 0)),
                                     PyInt_AsLong(PyTuple_GET_ITEM(ret, 1)));
   bar = PyDict_GetItemString(globals, "bar");
   Py_DECREF(ret);
   ret = PyObject_Call(bar, noargs, NULL);
   if (ret == Py_None) printf("bar returned Py_None\n");
   Py_DECREF(ret);
   Py_DECREF(noargs);
   Py_Finalize();
}
