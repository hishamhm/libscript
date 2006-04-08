
#ifndef LIBSCRIPT_PLUGIN_H
#define LIBSCRIPT_PLUGIN_H

#include "libscript.h"

typedef void* script_plugin_state;

typedef script_plugin_state (*script_plugin_init_fn) (script_env*, const char*);
typedef script_err (*script_plugin_run_fn) (script_plugin_state, const char*);
typedef void (*script_plugin_done_fn) (script_plugin_state);

#endif
