
#include <libscript-plugin.h>

#include <Python.h>

#include "libscript-py.h"

static script_env* script_py_env;

static PyObject* script_py_dict;

INLINE static void script_py_get_params(script_env* env, PyObject* args) {
   int i;
   int nargs = PyTuple_GET_SIZE(args);
   script_start_params(env);
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
}

INLINE static PyObject* script_py_put_params(script_env* env) {
   PyObject* args;
   int nargs;
   int i;

   nargs = script_param_count(env);
   args = PyTuple_New(nargs);
   for(i = 0; i < nargs; i++) {
      PyObject* arg = NULL;
      switch (script_in_type(env)) {
      case SCRIPT_DOUBLE:
         arg = PyFloat_FromDouble(script_in_double(env)); 
         break;
      case SCRIPT_STRING:
         arg = PyString_FromString(script_in_string(env)); 
         break;
      default:
         /* pacify gcc warnings */
         assert(0);
      }
      assert(arg);
      PyTuple_SetItem(args, i, arg);
   }
   return args;
}

static PyObject* script_py_call(script_py_object *obj, PyObject *args, PyObject *kwds) {
   script_py_get_params(script_py_env, args);
   obj->fn(script_py_env);
   if (script_param_count(script_py_env) == 0) {
      Py_RETURN_NONE;
   } else {
      PyObject* ret = script_py_put_params(script_py_env);
      if (PyTuple_GET_SIZE(ret) > 1) {
         return ret;
      } else {
         return PyTuple_GET_ITEM(ret, 0);
      }
   }
}

static PyTypeObject script_py_object_type = {
   PyObject_HEAD_INIT(NULL)
   .ob_size = 0,
   .tp_name = "libscript.Object",
   .tp_basicsize = sizeof(script_py_object),
   .tp_call = (ternaryfunc)script_py_call,
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

script_plugin_state script_plugin_init_py(script_env* env) {
   /* TODO: trap interpreter error messages */
   PyObject* module;
   char* namespace;
   char import_namespace[201];

   Py_Initialize();
   
   script_py_env = env;
   namespace = (char*) script_get_namespace(env);

   module = Py_InitModule3(namespace, script_py_methods, namespace);
   script_py_dict = PyModule_GetDict(module);
   Py_INCREF(script_py_dict);
   module->ob_type->tp_getattro = script_py_get;

   script_py_object_type.tp_new = PyType_GenericNew;
   if (PyType_Ready(&script_py_object_type) < 0)
      return NULL;

   snprintf(import_namespace, 200, "import %s\n", namespace);
   PyRun_SimpleString(import_namespace);

   return SCRIPT_GLOBAL_STATE;
}

int script_plugin_run_py(script_plugin_state state, char* programtext) {
   if (!PyRun_SimpleString(programtext))
      return SCRIPT_OK;
   else
      return SCRIPT_ERRLANGRUN;
}

int script_plugin_call_py(script_plugin_state state, char* fn) {
   PyObject *obj, *args, *ret;
   script_env* env;
   const char* namespace;

   env = script_py_env;
   namespace = script_get_namespace(env);

   obj = PyDict_GetItemString(script_py_dict, fn);
   if (!(obj && PyFunction_Check(obj)))
      return SCRIPT_ERRFNUNDEF;
   args = script_py_put_params(env);
   /* TODO: process returns */
   ret = PyEval_CallObject(obj, args);
   if (!ret) {
      /* TODO: trap error message */
      return SCRIPT_ERRLANGRUN;
   }
   return SCRIPT_OK;
}

void script_plugin_done_py(script_plugin_state state) {
   Py_DECREF(script_py_dict);
   Py_Finalize();
}
