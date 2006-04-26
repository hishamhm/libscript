
#ifndef LIBSCRIPT_PY_H
#define LIBSCRIPT_PY_H

#include <Python.h>

#ifdef __GNUC__
#define INLINE __inline
#else
#define INLINE
#endif

typedef struct {
   PyObject_HEAD
   script_fn fn;
} script_py_object;

script_plugin_state script_plugin_init_py(script_env* env);
int script_plugin_run_py(script_plugin_state state, char* programtext);
int script_plugin_call_py(script_plugin_state state, char* fn);
void script_plugin_done_py(script_plugin_state state);

#endif
