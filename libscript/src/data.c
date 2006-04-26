
#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>

#include "libscript.h"
#include "internals.h"
#include "data.h"

#include <stdio.h>

int script_param_count(script_env* env) {
   return env->param_size - env->param_ins;
}

script_type script_in_type(script_env* env) {
   script_data* data;
   int size = env->param_size;
   int ins = env->param_ins;
   if (ins == size) return SCRIPT_NONE;
   data = &(env->params[ins]);
   return data->type;
}

INLINE static script_data* script_in_data(script_env* env, script_type type) {
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

static script_data* script_out_data(script_env* env, script_type type) {
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

const char* script_in_string(script_env* env) {
   script_data* data = script_in_data(env, SCRIPT_STRING);
   if (!data) return NULL;
   return data->u.string_value;
}

double script_in_double(script_env* env) {
   script_data* data = script_in_data(env, SCRIPT_DOUBLE);
   if (!data) return 0;
   return data->u.double_value;
}

int script_in_int(script_env* env) {
   script_data* data = script_in_data(env, SCRIPT_DOUBLE);
   if (!data) return 0;
   return (int) data->u.double_value;
}

void script_out_string(script_env* env, const char* value) {
   script_data* data = script_out_data(env, SCRIPT_STRING);
   if (!data) return;
   /* TODO: review the lifetime of strings for leaks */
   data->u.string_value = value;
}

void script_out_double(script_env* env, double value) {
   script_data* data = script_out_data(env, SCRIPT_DOUBLE);
   if (!data) return;
   data->u.double_value = value;
}

void script_out_int(script_env* env, int value) {
   script_data* data = script_out_data(env, SCRIPT_DOUBLE);
   if (!data) return;
   data->u.double_value = value;
}

void script_start_params(script_env* env) {
   env->param_size = 0;
   env->param_ins = 0;
   env->param_outs = 0;
}
