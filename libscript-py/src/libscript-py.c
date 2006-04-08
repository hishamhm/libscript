
#include <libscript-plugin.h>

#include <Python.h>

#include "libscript-py.h"

static script_env* script_py_env;

static PyObject* script_py_dict;

static PyObject* script_py_object_call(script_py_object *obj, PyObject *args, PyObject *kwds) {
   int i;
   int nargs = PyTuple_GET_SIZE(args);
   script_flush_params(script_py_env);
   for (i = 0; i < nargs; i++) {
      PyObject* arg = PyTuple_GET_ITEM(args, i);
      if (PyString_Check(arg)) {
         script_out_string(script_py_env, PyString_AS_STRING(arg));
      } else if (PyInt_Check(arg)) {
         script_out_int(script_py_env, PyInt_AS_LONG(arg));
      } else if (PyLong_Check(arg)) {
         script_out_double(script_py_env, PyLong_AsDouble(arg));
      } else if (PyFloat_Check(arg)) {
         script_out_double(script_py_env, PyFloat_AS_DOUBLE(arg));
      } else {
         /* TODO: other types */
      }
   }
   script_reset_outs(script_py_env);
   obj->fn(script_py_env);
   /* TODO: return values */
   Py_RETURN_NONE;
}

static PyTypeObject script_py_object_type = {
   PyObject_HEAD_INIT(NULL)
   .ob_size = 0,
   .tp_name = "libscript.Object",
   .tp_basicsize = sizeof(script_py_object),
   .tp_call = (ternaryfunc)script_py_object_call,
   .tp_flags = Py_TPFLAGS_DEFAULT,
   .tp_doc = "LibScript object"
};

static PyObject* script_py_get(PyObject* self, PyObject *attr_name) {
   char* name;
   script_fn fn;
   script_py_object* obj;
   
   if ((obj = (script_py_object*)PyObject_GetItem(script_py_dict, attr_name)))
      return (PyObject*)obj;
   name = PyString_AS_STRING(attr_name);
   fn = script_get_function(script_py_env, name);
   if (fn) {
      obj = PyObject_New(script_py_object, &script_py_object_type);
      obj->fn = fn;
      PyObject_SetItem(script_py_dict, attr_name, (PyObject*)obj);
      return (PyObject*)obj;
   }
   Py_RETURN_NONE;
}

static PyMethodDef script_py_methods[] = {
   {NULL}
};

script_plugin_state script_plugin_init_py(script_env* env, char* namespace) {
   PyObject* module;
   char import_namespace[200];

   Py_Initialize();
   
   script_py_env = env;

   module = Py_InitModule3(namespace, script_py_methods, namespace);
   script_py_dict = PyModule_GetDict(module);
   module->ob_type->tp_getattro = script_py_get;

   script_py_object_type.tp_new = PyType_GenericNew;
   if (PyType_Ready(&script_py_object_type) < 0)
      return NULL;

   sprintf(import_namespace, "import %s\n", namespace);
   PyRun_SimpleString(import_namespace);

   return SCRIPT_GLOBAL_STATE;
}

int script_plugin_run_py(script_plugin_state state, char* programtext) {
   PyRun_SimpleString(programtext);
   return SCRIPT_OK;
}

void script_plugin_done_py(script_plugin_state state) {
   Py_Finalize();
}
