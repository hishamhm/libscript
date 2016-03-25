
#ifndef LIBSCRIPT_H
#define LIBSCRIPT_H

typedef enum {
   SCRIPT_OK = 0,
   SCRIPT_ERRAPI,
   SCRIPT_ERRFILE,
   SCRIPT_ERRFILENOTFOUND,
   SCRIPT_ERRFILEUNKNOWN,
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
   SCRIPT_ERRPARORDER,
   SCRIPT_ERRPARMISSING,
   SCRIPT_ERRPAREXCESS,
   SCRIPT_ERRPARTYPE
} script_err;

typedef enum {
   SCRIPT_FALSE = 0,
   SCRIPT_TRUE = 1
} script_bool;

typedef enum {
   SCRIPT_NONE = 0,
   SCRIPT_STRING,
   SCRIPT_DOUBLE,
   SCRIPT_BOOL,
   SCRIPT_LLINT
} script_type;

#define SCRIPT_GLOBAL_STATE ((void*)1)

typedef struct script_plugin_ script_plugin;

typedef struct script_env_ script_env;

typedef struct script_data_ script_data;

typedef script_err (*script_fn)(script_env*);

/* libscript.c */
script_env* script_init(const char* namespace);
script_err script_done(script_env* env);
script_err script_new_function(script_env* env, script_fn fn, const char* name);
script_err script_run(script_env* env, const char* language, const char* code);
script_err script_run_file(script_env* env, const char* name);
script_err script_call(script_env* env, const char* fn);
script_err script_error(script_env* env);
const char* script_error_message(script_env* env);
void script_set_error_message(script_env* env, const char* message);
const char* script_namespace(script_env* env);
const char* script_fn_name(script_env* env);

/* data.c */
char* script_get_string(script_env* env, int i);
double script_get_double(script_env* env, int i);
int script_get_int(script_env* env, int i);
int script_get_bool(script_env* env, int i);
long long script_get_llint(script_env* env, int i) ;
script_type script_get_type(script_env* env, int i);
int script_buffer_len(script_env* env);
void script_put_string(script_env* env, int i, const char* value);
void script_put_double(script_env* env, int i, double value);
void script_put_int(script_env* env, int i, int value);
void script_put_bool(script_env* env, int i, int value);
void script_reset_buffer(script_env* env);
void script_put_llint(script_env* env, int i, long long value) ;
#define SCRIPT_CHECK_INPUTS(env) do { script_err err = script_error(env); if (err) return err; } while (0)

#endif

