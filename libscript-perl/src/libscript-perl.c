
#include <libscript-plugin.h>

#include <EXTERN.h>
#include <perl.h>

#include "libscript-perl.h"

EXTERN_C void xs_init(pTHX);
   
#define LEN_CODE 1024
   
script_plugin_state script_plugin_init_perl(script_env* env) {
   /* Has to be called "my_perl" because the
      PL_perl_destruct_level macro expects it... */
   script_perl_state* state;
   PerlInterpreter *my_perl;
   char *embedding[] = { "", "-e", "0" };
   char code[LEN_CODE+1];
   
   state = malloc(sizeof(script_perl_state));
   state->env = env;
   
   my_perl = perl_alloc();
   state->P = my_perl;

   state->package = strdup(script_get_namespace(env));
   state->package[0] = toupper(state->package[0]);

   PERL_SET_CONTEXT(my_perl);
   PL_perl_destruct_level = 1;
   perl_construct(my_perl);
   perl_parse(my_perl, xs_init, 3, embedding, NULL);
   perl_run(my_perl);

   snprintf(code, LEN_CODE,
   "bootstrap LibScript;"
   "package %s;"
   "$__state = %p;"
   "sub AUTOLOAD {"
      "our $AUTOLOAD, $__state;"
      "$AUTOLOAD =~ s/[^:]*:://;"
      "LibScript::call($__state, $AUTOLOAD, @_);"
   "}",
   state->package, state);
   Perl_eval_pv(my_perl, code, TRUE);

   return (script_plugin_state) state;
}

void script_plugin_done_perl(script_plugin_state state) {
   PerlInterpreter* my_perl = ((script_perl_state*)state)->P;

   PERL_SET_CONTEXT(my_perl);
   PL_perl_destruct_level = 1;
   perl_destruct(my_perl);
   perl_free(my_perl);
}

script_err script_plugin_run_perl(script_plugin_state state, char* programtext) {
   PerlInterpreter* my_perl = ((script_perl_state*)state)->P;

   PERL_SET_CONTEXT(my_perl);
   
   Perl_eval_pv(my_perl, programtext, TRUE);

   return SCRIPT_OK;
}

script_err script_plugin_call_perl(script_plugin_state state, char* fn) {
   /* TODO: stub */
   return SCRIPT_OK;
}
