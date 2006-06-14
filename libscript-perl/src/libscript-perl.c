
#include <libscript-plugin.h>

#include <EXTERN.h>
#include <perl.h>

#include "libscript-perl.h"

EXTERN_C void boot_DynaLoader(pTHX_ CV* cv);
   
static int script_perl_loaded = 0;

script_plugin_state script_plugin_init_perl(script_env* env) {
   PerlInterpreter *P;

   char *embedding[] = { "", "-e", "0" };
    
   P = perl_alloc();

   PERL_SET_CONTEXT(P);

   if (!script_perl_loaded) {
      char* file = __FILE__;
      script_perl_loaded = 1;
      /* FIXME: dynamic loader is not linking nicely... */
      /* Perl_newXS(P, "DynaLoader::boot_DynaLoader", boot_DynaLoader, file); */
   }
  
   perl_construct(P);
   perl_parse(P, NULL, 3, embedding, NULL);
   perl_run(P);
   
   return (script_plugin_state) P;
}

void script_plugin_done_perl(script_plugin_state state) {
   PerlInterpreter *P = (PerlInterpreter*) state;

   perl_destruct(P);
   perl_free(P);
}

script_err script_plugin_run_perl(script_plugin_state state, char* programtext) {
   PerlInterpreter* P = (PerlInterpreter*) state;

   PERL_SET_CONTEXT(P);
   
   Perl_eval_pv(P, programtext, TRUE);

   return SCRIPT_OK;
}

script_err script_plugin_call_perl(script_plugin_state state, char* fn) {
   /* TODO: stub */
   return SCRIPT_OK;
}
