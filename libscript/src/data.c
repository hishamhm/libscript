
#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>

#include "libscript.h"
#include "internals.h"
#include "data.h"

#include <stdio.h>

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

int script_param_count(script_env* env) {
   return env->param_size - env->param_ins;
}

script_type script_get_type(script_env* env) {
   script_data* data;
   int size = env->param_size;
   int ins = env->param_ins;
   if (ins == size) return SCRIPT_NONE;
   data = &(env->params[ins]);
   return data->type;
}

INLINE static script_data* script_get_data(script_env* env, script_type type) {
   script_data* data;
   int size = env->param_size;
   int ins = env->param_ins;
   script_check_ret(ins == size, SCRIPT_ERRPARMISSING, NULL);
   data = &(env->params[ins]);
   env->param_ins++;
   env->param_outs = 0;
   script_check_ret(data->type != type, SCRIPT_ERRPARTYPE, NULL);
   return data;
}

static script_data* script_put_data(script_env* env, script_type type) {
   script_data* data;
   
   int outs = env->param_outs;
   script_check_ret(outs >= SCRIPT_MAX_PARAMS, SCRIPT_ERRPAREXCESS, NULL);
   data = &(env->params[outs]);
   data->type = type;
   outs++;
   env->param_ins = 0;
   env->param_outs = outs;
   env->param_size = outs;
   return data;
}

/**
 * @return The string, now owned by the caller.
 */
char* script_get_string(script_env* env) {
   script_data* data = script_get_data(env, SCRIPT_STRING);
   char* result;
   if (!data) return NULL;
   result = data->u.string_value;
   data->u.string_value = NULL;
   return result;
}

double script_get_double(script_env* env) {
   script_data* data = script_get_data(env, SCRIPT_DOUBLE);
   if (!data) return 0;
   return data->u.double_value;
}

int script_get_int(script_env* env) {
   script_data* data = script_get_data(env, SCRIPT_DOUBLE);
   if (!data) return 0;
   return (int) data->u.double_value;
}

int script_get_bool(script_env* env) {
   script_data* data = script_get_data(env, SCRIPT_BOOL);
   if (!data) return 0;
   return (int) data->u.bool_value;
}

/**
 * @param value The string value -- libscript stores its own copy of the string.
 */
void script_put_string(script_env* env, const char* value) {
   script_data* data = script_put_data(env, SCRIPT_STRING);
   if (!data) return;
   data->u.string_value = strdup(value);
}

void script_put_double(script_env* env, double value) {
   script_data* data = script_put_data(env, SCRIPT_DOUBLE);
   if (!data) return;
   data->u.double_value = value;
}

void script_put_int(script_env* env, int value) {
   script_data* data = script_put_data(env, SCRIPT_DOUBLE);
   if (!data) return;
   data->u.double_value = value;
}

void script_put_bool(script_env* env, int value) {
   script_data* data = script_put_data(env, SCRIPT_BOOL);
   if (!data) return;
   data->u.bool_value = value;
}

void script_params(script_env* env) {
   int size = env->param_size;
   int i = MAX(env->param_ins, env->param_outs);
   for (; i < size; i++) {
      script_data* data = &(env->params[i]);
      if (data->type == SCRIPT_STRING) {
         free(data->u.string_value);
      }
   }
   env->param_size = 0;
   env->param_ins = 0;
   env->param_outs = 0;
}
