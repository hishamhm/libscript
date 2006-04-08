
#define _GNU_SOURCE

#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <ltdl.h>
#include <stdio.h>
#include <stdarg.h>

#include "libscript.h"
#include "internals.h"
#include "plugin.h"
#include "data.h"

#ifndef SCRIPT_MAXARGS
#define SCRIPT_MAXARGS 128
#endif

#ifndef SCRIPT_EXTSEP
#define SCRIPT_EXTSEP '.'
#endif

/**
 * @return a string with the contents of the file, or NULL on error.
 */
static char* read_file(const char* filename) {
   struct stat statbuf;
   int ok, len;
   FILE* handle;
   char* ret;
   
   ok = stat(filename, &statbuf);
   if (ok != 0) return NULL;
   len = statbuf.st_size;
   handle = fopen(filename, "r");
   if (!handle) return NULL;
   ret = malloc(len + 1);
   fread(ret, len, 1, handle);
   ret[len] = '\0';
   fclose(handle);
   return ret;
}

/**
 * @return a handle to the environment on success, NULL on error.
 */
script_env* script_init(const char* namespace) {
   script_env* env;
   int ok; 
   
   ok = lt_dlinit();
   if (ok != 0)
      return NULL;
   env = calloc(sizeof(script_env), 1);
   env->namespace = strdup(namespace);

   env->plugins = ht_new(5, HT_STR, HT_BORROW_REFS);
   env->functions = ht_new(20, HT_STR, HT_BORROW_REFS);

   env->params = calloc(sizeof(script_data), SCRIPT_MAX_PARAMS + 1);

   return env;
}

script_err script_new_function(script_env* env, script_fn fn, const char* name) {
   script_fn existing;
   ht_key key;

   key.str = name;
   existing = ht_get(env->functions, key);
   script_check(existing, SCRIPT_ERRFNREDEF);

   ht_put(env->functions, key, fn);
   return SCRIPT_OK;
}

script_fn script_get_function(script_env* env, const char* name) {
   script_fn fn;
   ht_key key;

   key.str = name;
   fn = ht_get(env->functions, key);
   script_check_ret(!fn, SCRIPT_ERRFNUNDEF, NULL);
   return fn;
}

script_err script_run(script_env* env, const char* language, const char* code) {
   script_plugin* plugin = script_plugin_load(env, language);
   /* TODO: identify errors to client code */
   script_check_err(!plugin);
   return script_plugin_run(env, plugin, code);
}

script_err script_run_file(script_env* env, const char* filename) {
   char* extension;
   char* programtext;
   script_err err;

   programtext = read_file(filename);
   script_check(!programtext, SCRIPT_ERRFILE);
   /* TODO: also detect language based on first-line #! when extension fails */ 
     
   extension = strrchr(filename, SCRIPT_EXTSEP);
   extension++;
   script_plugin* plugin = script_plugin_load(env, extension);
   /* TODO: identify errors to client code */
   script_check_err(!plugin);

   err = script_plugin_run(env, plugin, programtext);
   free(programtext);
   return err;
}

script_err script_done(script_env* env) {
   script_err err;
   int dlerr;
   script_plugin* plugin;
   
   /* TODO: verify that removal of multiple states work for all plugins. */
   while ((plugin = ht_take_first(env->plugins))) {
      script_err pluginerr;

      pluginerr = script_plugin_unload(env, plugin);
      err = err ? err : pluginerr;
   }
   free(env->namespace);
   ht_delete(env->plugins);
   ht_delete(env->functions);
   script_flush_params(env);
   free(env);
   dlerr = lt_dlexit();
   if (dlerr)
      err = err ? err : SCRIPT_ERRDL;
   return err;
}

/**
 * Obtain the last error code of the given environment.
 * The error code is cleared (ie, two consecutive calls will
 * always return SCRIPT_OK in the second call.)
 * @return The last error code.
 */
script_err script_error(script_env* env) {
   script_err err = env->error;
   env->error = SCRIPT_OK;
   return err;
}

/**
 * Obtain the last error message of the given environment.
 * Unlike script_error, the error code or message are NOT cleared. 
 * @return The error message, in a buffer owned by script_env.
 */
const char* script_error_message(script_env* env) {
   return env->error_message;
}

void script_set_error_message(script_env* env, const char* message) {
   if (message) {
      strncpy(env->error_message, message, SCRIPT_ERROR_MESSAGE_LEN);
      env->error_message[SCRIPT_ERROR_MESSAGE_LEN - 1] = '\0';
   } else {
      env->error_message[0] = '\0';
   }
}
