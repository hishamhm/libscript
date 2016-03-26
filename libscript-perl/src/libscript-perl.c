
#include <libscript-plugin.h>

#include <EXTERN.h>
#include <perl.h>
      

#include "libscript-perl.h"
#include "config.h"


EXTERN_C void xs_init(pTHX);
   
#define LEN_CODE 1024

#if P_DONT_USE_MULTIPLICITY
static char script_perl_state_exists = 0;
#endif

char* script_perl_make_package_name(const char* name) {
   int name_size = strlen(name) + 1;
   char* package_name = malloc(name_size);
   strncpy(package_name, name, name_size);
   package_name[0] = toupper(package_name[0]);
   return package_name;
}
   
script_plugin_state script_plugin_init_perl(script_env* env) {

   #if P_DONT_USE_MULTIPLICITY
   if(script_perl_state_exists) {
      script_set_error_message(env, "System does not support multiple instances of Perl.");
      return NULL;
   }
   #endif

   /* Has to be called "my_perl" because the
      PL_perl_destruct_level macro expects it... */
   char* package_name;
   PerlInterpreter *my_perl;
   char *embedding[] = { "", "-e", "0" };
   char code[LEN_CODE+1];
   
   my_perl = perl_alloc();

   PERL_SET_CONTEXT(my_perl);
   PL_perl_destruct_level = 1;
   perl_construct(my_perl);
   perl_parse(my_perl, xs_init, 3, embedding, NULL);
   perl_run(my_perl);

   package_name = script_perl_make_package_name(script_namespace(env));
   snprintf(code, LEN_CODE,
   "bootstrap LibScript;"
   "package %s;"
   "$LibScript::env = %p;"
   "$LibScript::package = '%s';"
   "sub AUTOLOAD {"
      "our $AUTOLOAD;"
      "$AUTOLOAD =~ s/[^:]*:://;"
      "LibScript::caller(%p, $AUTOLOAD, @_);"
   "}"
   , package_name, env, package_name, env);
   free(package_name);
   Perl_eval_pv(my_perl, code, TRUE);

   #if P_DONT_USE_MULTIPLICITY
   script_perl_state_exists = 1;
   #endif
   return (script_plugin_state) my_perl;
}

void script_plugin_done_perl(script_perl_state state) {

   PerlInterpreter* my_perl = (PerlInterpreter*) state;
   PERL_SET_CONTEXT(my_perl);

   PL_perl_destruct_level = 1;
   perl_destruct(my_perl);
   perl_free(my_perl);
   #if P_DONT_USE_MULTIPLICITY
   script_perl_state_exists = 0;
   #endif
}

script_err script_plugin_run_perl(script_perl_state state, char* programtext) {
   PerlInterpreter* my_perl = (PerlInterpreter*) state;
   script_env* env;

   PERL_SET_CONTEXT(my_perl);
   env = (script_env*) SvIV(Perl_get_sv(aTHX_ "LibScript::env", FALSE));

   Perl_eval_pv(my_perl, programtext, FALSE);
   if(SvTRUE(ERRSV)) {
      script_set_error_message(env, SvPV(ERRSV, PL_na));
      return SCRIPT_ERRLANGRUN;
   }
   return SCRIPT_OK;
}

static char* script_perl_full_name(const char* package_name, const char* fn_name) {
   int package_len = strlen(package_name);
   int fn_len = strlen(fn_name);
   int full_len = package_len+fn_len+3;
   char* full_name = malloc(full_len);
   snprintf(full_name, full_len, "%s::%s", package_name, fn_name);
   return full_name;
}

script_err script_plugin_call_perl(script_perl_state state, char* fn) {
   PerlInterpreter* my_perl = (PerlInterpreter*) state;
   script_env* env;
   char* package_name;
   char* full_name;
   int n_results;
   CV* function;
   dSP;

   PERL_SET_CONTEXT(my_perl);

   env = (script_env*) SvIV(Perl_get_sv(aTHX_ "LibScript::env", FALSE));
   package_name = SvPV_nolen(Perl_get_sv(aTHX_ "LibScript::package", FALSE));
   full_name = script_perl_full_name(package_name, fn);
   
   function = Perl_get_cv(aTHX_ full_name, FALSE);
   if (!function)
      return SCRIPT_ERRFNUNDEF;

   ENTER;
   SAVETMPS;
   PUSHMARK(SP);
   sp = script_perl_buffer_to_stack(aTHX_ sp, env);

   PUTBACK;
   n_results = Perl_call_pv(aTHX_ full_name, G_ARRAY);
   SPAGAIN;
   if(SvTRUE(ERRSV)) {
      script_set_error_message(env, SvPV(ERRSV, PL_na));
      PUTBACK;
      FREETMPS;
      LEAVE;
      return SCRIPT_ERRLANGRUN;
   }
   script_perl_stack_to_buffer(aTHX_ 0, env, n_results, 1);
   PUTBACK;
   FREETMPS;
   LEAVE;
   return SCRIPT_OK;
}
