
#ifndef LIBSCRIPT_PYTHON_H
#define LIBSCRIPT_PYTHON_H

#include <Python.h>

#ifdef __GNUC__
#define INLINE __inline
#else
#define INLINE
#endif

typedef struct {
   script_env* env;
   PyObject* dict;
} script_python_state;

typedef struct {
   PyObject_HEAD
   script_python_state* state;
   char* fn_name;
} script_python_object;


script_plugin_state script_plugin_init_python(script_env* env);
int script_plugin_run_python(script_plugin_state state, char* programtext);
int script_plugin_call_python(script_python_state* state, char* fn);
void script_plugin_done_python(script_plugin_state state);

#endif
