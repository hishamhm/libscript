
#ifndef LIBSCRIPT_LUA_H
#define LIBSCRIPT_LUA_H

#ifdef __GNUC__
#define INLINE __inline
#else
#define INLINE
#endif

script_plugin_state script_plugin_init_lua(script_env* env);
script_err script_plugin_run_lua(script_plugin_state state, char* programtext);
script_err script_plugin_call_lua(script_plugin_state state, char* fn);
void script_plugin_done_lua(script_plugin_state state);

#endif
