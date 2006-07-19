
#include <Python.h>

PyObject* teste(PyObject* self, PyObject* args) {
   char* entrada; long n;
   printf("%p\n", self);
   if (!PyArg_ParseTuple(args, "sl", &entrada, &n)) {
      fprintf(stderr, "error!\n");
      return NULL;
   }
   printf("Received: %s and %ld \n", entrada, n);
   return PyInt_FromLong(42);
}

int main() {
   PyObject *globals, *foo, *bar, *noargs, *ret;
   Py_Initialize();
   PyRun_SimpleString(
      "def foo(): \n"
      "   return 2, 10 \n"
      "def bar(): \n"
      "   return \n"
      "def teste(a,b): \n"
      "   print a, b \n"
      "   return 42 \n"
   );
   globals = PyModule_GetDict(PyImport_AddModule("__main__"));
   Py_INCREF(globals);
   
   static PyMethodDef teste_def[] = {{ "teste", (PyCFunction) teste, METH_VARARGS, NULL }};
   PyObject* teste_obj = Py_FindMethod(teste_def, NULL, "teste");
   PyDict_SetItemString(globals, "teste", teste_obj);
   
   PyObject *teste = PyDict_GetItemString(globals, "teste");
   PyObject *obj_result = PyObject_CallFunction(teste, "si", "entrada", 2);
   if (!obj_result) {
      return;
   }
   long result = PyInt_AsLong(obj_result);
   Py_DECREF(obj_result);
   printf("teste returned %ld \n", result);
   
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
   Py_DECREF(globals);
   Py_Finalize();
}
