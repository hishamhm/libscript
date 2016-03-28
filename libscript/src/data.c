
#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>

#include "libscript.h"
#include "internals.h"
#include "data.h"

#include <stdio.h>
#include <assert.h>

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

void script_reset_buffer(script_env* env) {
   int size = env->buffer_len;
   int i;
   for (i = 0; i < size; i++) {
      script_data* data = &(env->buffer[i]);
      if (data->type == SCRIPT_STRING) {
         free(data->u.string_value);
      }
   }
   env->buffer_len = 0;
   env->next_get = 0;
}

int script_buffer_len(script_env* env) {
   return env->buffer_len;
}

script_type script_get_type(script_env* env, int i) {
   script_data* data;
   assert (i == env->next_get);
   if (i >= env->buffer_len) return SCRIPT_NONE;
   data = &(env->buffer[i]);
   return data->type;
}

INLINE static script_data* script_get_data(script_env* env, int i, script_type type) {
   script_data* data;
   assert (i == env->next_get);
   script_check_ret(i >= env->buffer_len, SCRIPT_ERRPARMISSING, NULL);
   data = &(env->buffer[i]);
   script_check_ret(data->type != type, SCRIPT_ERRPARTYPE, NULL);
   env->next_get++;
   return data;
}

static script_data* script_put_data(script_env* env, int i, script_type type) {
   script_data* data;

   if (i == 0 && env->buffer_len > 0)
      script_reset_buffer(env);
   assert(i == env->buffer_len);
   script_check_ret(i != env->buffer_len, SCRIPT_ERRPAREXCESS, NULL);
   script_check_ret(i >= SCRIPT_BUFFER_MAXSIZE, SCRIPT_ERRPAREXCESS, NULL);
   data = &(env->buffer[i]);
   data->type = type;
   env->buffer_len = i + 1;
   return data;
}

/**
 * @return The string, now owned by the caller.
 */
char* script_get_string(script_env* env, int i) {
   script_data* data = script_get_data(env, i, SCRIPT_STRING);
   char* result;
   if (!data) return NULL;
   result = data->u.string_value;
   data->u.string_value = NULL;
   assert(result);
   return result;
}

double128 script_get_double(script_env* env, int i) {
   script_data* data = script_get_data(env, i, SCRIPT_DOUBLE);
   if (!data) return 0;
   return data->u.double_value;
}

/*
Why do we need this function anyway?
We do not store information about the number subtype at all
Same question also valid for put_int
*/
int64 script_get_int(script_env* env, int i) {
   return (int64) script_get_double(env, i);
}

int script_get_bool(script_env* env, int i) {
   script_data* data = script_get_data(env, i, SCRIPT_BOOL);
   if (!data) return 0;
   return (int) data->u.bool_value;
}

/**
 * @param value The string value -- libscript stores its own copy of the string.
 */
void script_put_string(script_env* env, int i, const char* value) {
   script_data* data = script_put_data(env, i, SCRIPT_STRING);
   if (!data) return;
   data->u.string_value = strdup(value);
}

void script_put_double(script_env* env, int i, double128 value) {
   script_data* data = script_put_data(env, i, SCRIPT_DOUBLE);
   if (!data) return;
   data->u.double_value = value;
}

void script_put_int(script_env* env, int i, int64 value) {
   script_put_double(env, i, value);
}

void script_put_bool(script_env* env, int i, int value) {
   script_data* data = script_put_data(env, i, SCRIPT_BOOL);
   if (!data) return;
   data->u.bool_value = value;
}

/*
Determines if conversion from a double128 number to VM's integer is lossless.
Currently only used to choose between lua_pushinteger and lua_pushnumber for Lua.
EDIT: Also used by Python now.
*/
INLINE char script_isinteger(script_env* env, int i)
{
   double128 x = env->buffer[i].u.double_value;
   return x==(int64)x;
   /*TODO: Sometimes stuff like 13.99999 != 14 may happen.
   Change return criteria to check if difference within certain tolerance.
   Put the tolerance as a const or macro*/
   /*TODO: Determine the double and int type from the env and build settings.
   Dont assume int64 and double128.
   When you do this, remove the check macros from libscript-lua.c*/
}
