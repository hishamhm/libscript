
#ifndef LIBSCRIPT_PY_H
#define LIBSCRIPT_PY_H

script_plugin_state script_plugin_init_rb();
int script_plugin_run_rb(script_plugin_state state, char* programtext);
void script_plugin_done_rb(script_plugin_state state);

#endif
