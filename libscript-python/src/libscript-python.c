
#include <libscript-plugin.h>

#include <Python.h>

#include "libscript-python.h"

static int script_python_state_count = 0;

INLINE static void script_python_get_params(script_env* env, PyObject* args) {
   int i;
   int nargs = PyTuple_GET_SIZE(args);
   script_params(env);
   for (i = 0; i < nargs; i++) {
      PyObject* arg = PyTuple_GET_ITEM(args, i);
      if (PyString_Check(arg)) {
         script_out_string(env, PyString_AS_STRING(arg));
      } else if (PyInt_Check(arg)) {
         script_out_int(env, PyInt_AS_LONG(arg));
      } else if (PyLong_Check(arg)) {
         script_out_double(env, PyLong_AsDouble(arg));
      } else if (PyFloat_Check(arg)) {
         script_out_double(env, PyFloat_AS_DOUBLE(arg));
      } else if (PyBool_Check(arg)) {
         if (arg == Py_True)
            script_out_bool(env, 1);
         else
            script_out_bool(env, 0);
      } else {
         /* TODO: other types */
      }
   }
}

INLINE static PyObject* script_python_put_params(script_env* env) {
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
      case SCRIPT_STRING: {
         char* param = script_in_string(env);
         arg = PyString_FromString(param); 
         free(param);
         break;
      }
      case SCRIPT_BOOL:
         arg = PyBool_FromLong(script_in_bool(env));
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

static void script_python_destructor(PyObject* o) {
   script_python_object* spo = (script_python_object*) o;
   free(spo->fn_name);
   spo->fn_name = NULL;
   PyObject_DEL(o);
}

static PyObject* script_python_call(script_python_object *obj, PyObject *args, PyObject *kwds) {
   script_err err;
   script_python_get_params(obj->state->env, args);
   err = script_call(obj->state->env, obj->fn_name);
   if (err != SCRIPT_OK) {
      script_params(obj->state->env);
      PyErr_SetString(PyExc_RuntimeError, "No such function");
      Py_RETURN_NONE;
   }
   if (script_param_count(obj->state->env) == 0) {
      Py_RETURN_NONE;
   } else {
      PyObject* ret = script_python_put_params(obj->state->env);
      if (PyTuple_GET_SIZE(ret) > 1) {
         return ret;
      } else {
         return PyTuple_GET_ITEM(ret, 0);
      }
   }
}

static PyTypeObject script_python_object_type = {
   PyObject_HEAD_INIT(NULL)
   .ob_size = 0,
   .tp_name = "libscript.Object",
   .tp_basicsize = sizeof(script_python_object),
   .tp_dealloc = script_python_destructor,
   .tp_call = (ternaryfunc)script_python_call,
   .tp_flags = Py_TPFLAGS_DEFAULT,
   .tp_doc = "LibScript object"
};

static PyObject* script_python_getattro(PyObject* self, PyObject *attr_name) {
   char* name;
   script_python_object* obj;

   PyObject* dict = PyModule_GetDict(self);
   if ((obj = (script_python_object*)PyDict_GetItem(dict, attr_name))) {
      Py_INCREF(obj);
      return (PyObject*)obj;
   }
   
   name = PyString_AS_STRING(attr_name);
   /* TODO: create new entry here only if some plugin has the function */
   obj = PyObject_New(script_python_object, &script_python_object_type);
   obj->fn_name = strdup(name);
   obj->state = PyCObject_AsVoidPtr(PyDict_GetItemString(dict, "__state"));
   PyDict_SetItem(dict, attr_name, (PyObject*)obj);
   return (PyObject*)obj;
}

static PyMethodDef script_python_methods[] = {
   {NULL}
};

script_plugin_state script_plugin_init_python(script_env* env) {
   /* TODO: trap interpreter error messages */
   PyObject* module;
   char* namespace;
   char import_namespace[201];
   script_python_state* state;
   
   if (script_python_state_count == 0) {
      Py_Initialize();
   }
   script_python_state_count++;
   
   state = malloc(sizeof(script_python_state));
   state->env = env;

   namespace = (char*) script_get_namespace(env);
   module = Py_InitModule3(namespace, script_python_methods, namespace);
   state->dict = PyModule_GetDict(module);
   Py_INCREF(state->dict);
   module->ob_type->tp_getattro = script_python_getattro;
   PyDict_SetItemString(state->dict, "__state", PyCObject_FromVoidPtr(state, NULL));

   script_python_object_type.tp_new = PyType_GenericNew;
   if (PyType_Ready(&script_python_object_type) < 0)
      return NULL;

   snprintf(import_namespace, 200, "import %s\n", namespace);
   PyRun_SimpleString(import_namespace);

   return state;
}

int script_plugin_run_python(script_plugin_state state, char* programtext) {
   if (!PyRun_SimpleString(programtext))
      return SCRIPT_OK;
   else
      return SCRIPT_ERRLANGRUN;
}

int script_plugin_call_python(script_python_state* state, char* fn) {
   PyObject *obj, *args, *ret;
   script_env* env;
   const char* namespace;

   env = state->env;
   namespace = script_get_namespace(env);

   obj = PyDict_GetItemString(state->dict, fn);
   if (!(obj && PyFunction_Check(obj)))
      return SCRIPT_ERRFNUNDEF;

   args = script_python_put_params(env);
   ret = PyEval_CallObject(obj, args);
   if (!ret) {
      PyObject* message = PyObject_Str(PyErr_Occurred());
      script_set_error_message(env, PyString_AS_STRING(message));
      Py_DECREF(message);
      return SCRIPT_ERRLANGRUN;
   }
   if (!PyTuple_Check(ret)) {
      PyObject* tuple = PyTuple_New(1);
      PyTuple_SetItem(tuple, 0, ret);
      ret = tuple;
   }
   script_python_get_params(env, ret);
   return SCRIPT_OK;
}

void script_plugin_done_python(script_python_state* state) {
   Py_DECREF(state->dict);
   free(state);
   script_python_state_count--;
   if (script_python_state_count == 0) {
      /* FIXME: Getting strange gc-related errors. */
      /* Py_Finalize(); */
   }
}
