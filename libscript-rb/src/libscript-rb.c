
#include <libscript-plugin.h>

/* Ouch! Ruby's headers are not ANSI C! */
#ifdef __GNUC__
#define INLINE __inline
#define inline __inline
#else
#define INLINE
#define inline
#endif

/* Since we're not ANSI C anyway, use snprintf */
#define _GNU_SOURCE
#include <stdio.h>
#include <ctype.h>
#include <assert.h>

#include <ruby.h>


#include "libscript-rb.h"

static VALUE script_rb_class;
static script_env* script_rb_env;
static char* script_rb_namespace;
static ID method_id;

INLINE static void script_rb_get_param(script_env* env, VALUE arg) {
   switch (TYPE(arg)) {
   case T_FLOAT:
   case T_FIXNUM:
   case T_BIGNUM:
      script_out_double(env, NUM2DBL(arg));
      break;
   case T_STRING:
      script_out_string(env, StringValuePtr(arg));
      break;
   default:;
      /* TODO: other types */
   }
}

INLINE static void script_rb_get_params(script_env* env, VALUE args) {
   VALUE arg;

   script_start_params(env);
   while ( (arg = rb_ary_shift(args)) != Qnil ) {
      script_rb_get_param(env, arg);
   }
}

INLINE static VALUE* script_rb_put_params(script_env* env, int params) {
   int i;
   VALUE* args = malloc(sizeof(VALUE) * params);
   for (i = 0; i < params; i++) {
      VALUE arg = (VALUE) NULL;
      switch (script_in_type(env)) {
      case SCRIPT_DOUBLE:
         arg = rb_float_new(script_in_double(env)); 
         break;
      case SCRIPT_STRING:
         arg = rb_str_new2(script_in_string(env)); 
         break;
      default:
         /* pacify gcc warnings */
         assert(0);
      }
      assert(arg);
      args[i] = arg;
   }
   return args;
}

INLINE static VALUE script_rb_params_to_value(script_env* env) {
   VALUE* rets;
   VALUE ret;
   int nargs;

   nargs = script_param_count(env);
   if (nargs > 0) {
      rets = script_rb_put_params(env, nargs);
      if (nargs == 1)
         ret = rets[0];
      else
         ret = rb_ary_new4(nargs, rets);
      free(rets);
   } else
      ret = Qnil;
   return ret;
}

static VALUE script_rb_call(VALUE self, VALUE fn_value, VALUE args) {
   script_fn fn;

   fn = (script_fn) NUM2LONG(fn_value);
   script_rb_get_params(script_rb_env, args);
   fn(script_rb_env);
   return script_rb_params_to_value(script_rb_env);
}

static VALUE script_rb_method_missing(int argc, VALUE* argv, VALUE obj) {
   char* name;
   script_fn fn;

   name = rb_id2name(SYM2ID(argv[0]));
   fn = script_get_function(script_rb_env, name);
   
   if (fn) {
      char fn_code[1024];
      VALUE args;
      snprintf(fn_code, 1023, "def %s.%s(*args); %s.call(%ld, args); end;",
         script_rb_namespace, name, script_rb_namespace, (long int) fn);
      rb_eval_string(fn_code);
      args = rb_ary_new4(argc - 1, argv+1);
      return script_rb_call(obj, rb_int_new((long int) fn), args);
   } else {
      script_err err;
      int i;
      for (i = 0; i < argc; i++)
         script_rb_get_param(script_rb_env, argv[i]);
      err = script_call(script_rb_env, name);
      if (err != SCRIPT_OK) {
         /* FIXME: I'm getting a Ruby segfault when Ruby raises exceptions. */
         rb_raise(rb_eRuntimeError, "No such function: '%s'.", name);
         return Qnil;
      }
      return script_rb_params_to_value(script_rb_env);
   }
   return Qnil;
}

script_plugin_state script_plugin_init_rb(script_env* env) {
   const char* namespace;
   int namespace_size;

   namespace = script_get_namespace(env);
   ruby_init();
   ruby_script(namespace);
   namespace_size = strlen(namespace) + 1;
   script_rb_namespace = malloc(namespace_size);
   strncpy(script_rb_namespace, namespace, namespace_size);
   script_rb_env = env;
   script_rb_namespace[0] = toupper(script_rb_namespace[0]);
   script_rb_class = rb_define_class(script_rb_namespace, rb_cObject);
   method_id = rb_intern("method");
   rb_define_singleton_method(script_rb_class, "method_missing", script_rb_method_missing, -1);
   rb_define_singleton_method(script_rb_class, "call", script_rb_call, 2);
   return SCRIPT_GLOBAL_STATE;
}

int script_plugin_run_rb(script_plugin_state state, char* programtext) {
   rb_eval_string(programtext);
   return SCRIPT_OK;
}

int script_plugin_call_rb(script_plugin_state state, char* fn) {
   VALUE method;
   ID fn_id;
   VALUE fn_value;
   int params;
   VALUE* args;
   VALUE ret;

   fn_value = rb_str_new2(fn);
   method = rb_funcall(script_rb_class, method_id, 1, fn_value);
   if (method == Qnil)
      return SCRIPT_ERRFNUNDEF;
   fn_id = rb_intern(fn);
   params = script_param_count(script_rb_env);
   args = script_rb_put_params(script_rb_env, params); 
   ret = rb_funcall2(script_rb_class, fn_id, params, args);
   script_rb_get_param(script_rb_env, ret);
   return SCRIPT_OK;
}

void script_plugin_done_rb(script_plugin_state state) {
   /* TODO: at least reset the environment, if possible */
}
