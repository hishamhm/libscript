
#ifndef LIBSCRIPT_H
#define LIBSCRIPT_H

#define SCRIPT_MAX_PARAMS 100

typedef enum {
   SCRIPT_OK = 0,
   SCRIPT_ERRAPI,
   SCRIPT_ERRFILE,
   SCRIPT_ERRFILENOTFOUND,
   SCRIPT_ERRDL,
   SCRIPT_ERRDLOPEN,
   SCRIPT_ERRDLINVALID,
   SCRIPT_ERRDLCLOSE,
   SCRIPT_ERRLANG,
   SCRIPT_ERRLANGCOMP,
   SCRIPT_ERRLANGRUN,
   SCRIPT_ERRFN,
   SCRIPT_ERRFNREDEF,
   SCRIPT_ERRFNUNDEF,
   SCRIPT_ERRPAR,
   SCRIPT_ERRPARMISSING,
   SCRIPT_ERRPAREXCESS,
   SCRIPT_ERRPARTYPE
} script_err;

#define SCRIPT_GLOBAL_STATE ((void*)1)

typedef struct script_plugin_ script_plugin;

typedef struct script_env_ script_env;

typedef struct script_data_ script_data;

typedef script_err (*script_fn)(script_env*);

/* libscript.c */
script_env* script_init(const char* namespace);
script_err script_run(script_env* env, const char* language, const char* code);
script_err script_run_file(script_env* env, const char* name);
script_err script_done(script_env* env);
script_err script_new_function(script_env* env, script_fn fn, const char* name);
script_fn  script_get_function(script_env* env, const char* name);
script_err script_error(script_env* env);
const char* script_error_message(script_env* env);
void script_set_error_message(script_env* env, const char* message);

/* data.c */
const char* script_in_string(script_env* env);
double script_in_double(script_env* env);
int script_in_int(script_env* env);
void script_out_string(script_env* env, const char* value);
void script_out_double(script_env* env, double value);
void script_out_int(script_env* env, int value);
void script_flush_params(script_env* env);
void script_reset_outs(script_env* env);
#define SCRIPT_CHECK_INPUTS(env) do { script_err err = script_error(env); if (err) return err; } while (0)

#endif

