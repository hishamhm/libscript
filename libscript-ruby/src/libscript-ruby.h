
#ifndef LIBSCRIPT_RUBY_H
#define LIBSCRIPT_RUBY_H

typedef struct {
   script_env* env;
   VALUE klass;
   char* namespace;
} script_ruby_state;

script_plugin_state script_plugin_init_ruby(script_env* env);
int script_plugin_run_ruby(script_plugin_state state, char* programtext);
int script_plugin_call_ruby(script_ruby_state* state, char* fn);
void script_plugin_done_ruby(script_ruby_state* state);

#endif
