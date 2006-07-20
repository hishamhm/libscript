
#include <Python.h>

void show_module(char* name) {
   PyObject *module, *dict, *keys;
   int i;
   printf("\n%s\n", name);
   printf("--------------------------------\n");
   module = PyImport_AddModule(name);
   dict = PyModule_GetDict(module);
   keys = PyDict_Keys(dict);
   for (i = 0; i < PyDict_Size(dict); i++) {
      PyObject* key = PyList_GetItem(keys, i);
      PyObject* item = PyDict_GetItem(dict, key);
      PyObject* key_pystr = PyObject_Str(key);
      PyObject* item_pystr = PyObject_Str(item);
      char* key_str = PyString_AsString(key_pystr);
      char* item_str = PyString_AsString(item_pystr);
      printf("%s => %s\n", key_str, item_str);
      Py_DECREF(key_pystr);
      Py_DECREF(item_pystr);
   }
   Py_DECREF(keys);
}

int main() {
   Py_Initialize();
   show_module("__builtin__");
   show_module("__main__");
   Py_Finalize();
}

