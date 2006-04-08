
#ifndef LIBSCRIPT_LUA_H
#define LIBSCRIPT_LUA_H

script_plugin_state script_plugin_init_lua();
int script_plugin_run_lua(script_plugin_state state, char* programtext);
void script_plugin_done_lua(script_plugin_state state);

#endif
