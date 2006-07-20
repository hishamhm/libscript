
#ifndef LIBSCRIPT_RUBY_H
#define LIBSCRIPT_RUBY_H

#include "libscript.h"
#include "libscript-plugin.h"

typedef struct {
   PerlInterpreter* P;
   script_env* env;
   char* package;
} script_perl_state;

script_err script_plugin_call_perl(script_plugin_state state, char* fn);

#endif
