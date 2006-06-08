
#define _GNU_SOURCE

#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <ltdl.h>
#include <stdio.h>
#include <ctype.h>

#include "libscript.h"
#include "internals.h"
#include "plugin.h"
#include "data.h"

#ifndef SCRIPT_MAXARGS
#define SCRIPT_MAXARGS 128
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

   key.str = (char*) name;
   existing = ht_get(env->functions, key);
   script_check(existing, SCRIPT_ERRFNREDEF);

   ht_put(env->functions, key, fn);
   return SCRIPT_OK;
}

script_fn script_get_function(script_env* env, const char* name) {
   script_fn fn;
   ht_key key;

   key.str = (char*) name;
   fn = ht_get(env->functions, key);
   script_check_ret(!fn, SCRIPT_ERRFNUNDEF, NULL);
   env->error = SCRIPT_OK;
   return fn;
}

script_err script_run(script_env* env, const char* language, const char* code) {
   script_plugin* plugin = script_plugin_load(env, language);
   script_check_err(!plugin);
   return script_plugin_run(env, plugin, code);
}

script_err script_run_file(script_env* env, const char* filename) {
   char *id, *programtext, *programinput;
   script_err err;

   programtext = read_file(filename);
   script_check(!programtext, SCRIPT_ERRFILE);
   programinput = programtext;
   if (programtext[0] == '#' && programtext[1] == '!') {
      char *at, *start, *end;

      at = programtext + 2;
      while (isspace(*at++));
      start = at;
      end = at;
      while (!isspace(*end)) {
         end++;
         if (*end == '/' || *end == '\\')
            start = end + 1;
      }
      id = calloc(end - start + 1, 1);
      strncpy(id, start, end - start);
      programinput = strchr(end, '\n') + 1;
   } else if ( (id = strrchr(filename, SCRIPT_EXTSEP)) ) {
      id++;
      id = strdup(id);
   } else {
      return SCRIPT_ERRFILEUNKNOWN;
   }
    
   script_plugin* plugin = script_plugin_load(env, id);
   /* TODO: identify errors to client code */
   script_check_err(!plugin);

   err = script_plugin_run(env, plugin, programinput);
   free(programtext);
   free(id);
   return err;
}

script_err script_call(script_env* env, const char* fn) {
   ht_iterator iter;
   script_plugin* plugin;
   script_fn function;
   ht_key key;
 
   key.str = (char*) fn;
   function = ht_get(env->functions, key);
   if (function) {
      env->fn_name = fn;
      env->error = function(env);
      return env->error;
   }
   
   ht_start(env->plugins, &iter);
   while ( (plugin = ht_iterate(&iter) ) ) {
      env->error = script_plugin_call(env, plugin, fn);
      env->param_ins = 0; /* TODO */
      if (env->error == SCRIPT_ERRFNUNDEF)
         continue;
      return env->error;
   }
   return SCRIPT_ERRFNUNDEF;
}

script_err script_done(script_env* env) {
   script_err err = SCRIPT_OK;
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

const char* script_get_namespace(script_env* env) {
   return env->namespace;
}

/**
 * Obtain the last error message of the given environment.
 * Unlike script_error, the error code or message are NOT cleared. 
 * @return The error message, in a buffer owned by script_env.
 */
const char* script_error_message(script_env* env) {
   switch (env->error) {
   case SCRIPT_OK: return "No error";
   case SCRIPT_ERRAPI: return "API call error";
   case SCRIPT_ERRFILE: return "File error";
   case SCRIPT_ERRFILENOTFOUND: return "File not found";
   case SCRIPT_ERRDL: return "Plugin error";
   case SCRIPT_ERRDLOPEN: return "Could not open plugin"; 
   case SCRIPT_ERRDLINVALID: return "Invalid plugin";
   case SCRIPT_ERRDLCLOSE: return "Could not close plugin";
   case SCRIPT_ERRLANG: return "Language error";
   case SCRIPT_ERRLANGCOMP: return (env->error_message[0] ? env->error_message : "Compile error");
   case SCRIPT_ERRLANGRUN: return (env->error_message[0] ? env->error_message : "Runtime error");
   case SCRIPT_ERRFN: return "Function error";
   case SCRIPT_ERRFNREDEF: return "Function redefinition error";
   case SCRIPT_ERRFNUNDEF: return "Undef'd function";
   case SCRIPT_ERRPAR: return "Parameter error";
   case SCRIPT_ERRPARMISSING: return "Expected parameter missing";
   case SCRIPT_ERRPAREXCESS: return "Too many parameters";
   case SCRIPT_ERRPARTYPE: return "Parameter type error";
   default: return env->error_message;
   }
}

void script_set_error_message(script_env* env, const char* message) {
   if (message) {
      strncpy(env->error_message, message, SCRIPT_ERROR_MESSAGE_LEN);
      env->error_message[SCRIPT_ERROR_MESSAGE_LEN - 1] = '\0';
   } else {
      env->error_message[0] = '\0';
   }
}

const char* script_fn_name(script_env* env) {
   return env->fn_name;
}
