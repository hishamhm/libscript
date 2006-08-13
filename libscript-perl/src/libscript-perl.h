
#ifndef LIBSCRIPT_RUBY_H
#define LIBSCRIPT_RUBY_H

#include "libscript.h"
#include "libscript-plugin.h"

#include <EXTERN.h>
#include <perl.h>

typedef PerlInterpreter* script_perl_state;

SV* script_perl_get_sv(pTHX_ script_env* env, int i);
SV** script_perl_buffer_to_stack(pTHX_ SV** sp, script_env* env);
void script_perl_stack_to_buffer(pTHX_ int ax, script_env* env, int count, int offset);

script_plugin_state script_plugin_init_perl(script_env* env);
script_err script_plugin_run_perl(script_perl_state state, char* programtext);
script_err script_plugin_call_perl(script_perl_state state, char* fn);
void script_plugin_done_perl(script_perl_state state);

#endif
