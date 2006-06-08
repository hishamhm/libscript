
#include <Python.h>

PyObject* builtins;

PyObject* my_PyObject_Str(PyObject* o) {
   static PyObject* str = NULL;
   if (!str)
      str = PyDict_GetItemString(builtins, "str");
   return PyObject_CallFunctionObjArgs(str, o, NULL);
}

int main() {
   PyObject *builtins_module, *an_int, *a_str, *str;
   char *result;
   int i;

   Py_Initialize();
   builtins_module = PyImport_AddModule("__builtin__");
   builtins = PyModule_GetDict(builtins_module);

   an_int = PyInt_FromLong(5556);

   a_str = my_PyObject_Str(an_int);

   result = PyString_AsString(a_str);
   printf("%s\n", result);

   Py_DECREF(a_str);

   Py_Finalize();
}
