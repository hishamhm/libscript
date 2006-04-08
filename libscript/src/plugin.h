
#ifndef PLUGIN_H
#define PLUGIN_H

#include "libscript.h"
#include "libscript-plugin.h"

#include <ltdl.h>

struct script_plugin_ {
   char* extension;
   lt_dlhandle dlhandle;
   script_plugin_run_fn run;
   script_plugin_state state;
};

script_plugin* script_plugin_load(script_env* env, const char* extension);

script_err script_plugin_run(script_env* env, script_plugin* plugin, const char* filename);

script_err script_plugin_unload(script_env* env, script_plugin* plugin);

#endif
