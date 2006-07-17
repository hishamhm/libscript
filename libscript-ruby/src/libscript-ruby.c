
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


#include "libscript-ruby.h"

static VALUE script_ruby_class;
static script_env* script_ruby_env;
static char* script_ruby_namespace;
static ID method_id;

INLINE static void script_ruby_get_param(script_env* env, VALUE arg) {
   if (arg == Qtrue) {
      script_out_bool(env, 1);
      return;
   } else if (arg == Qfalse) {
      script_out_bool(env, 0);
      return;
   }
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

INLINE static void script_ruby_get_params(script_env* env, VALUE args) {
   VALUE arg;

   script_start_params(env);
   while ( (arg = rb_ary_shift(args)) != Qnil ) {
      script_ruby_get_param(env, arg);
   }
}

INLINE static VALUE* script_ruby_put_params(script_env* env, int params) {
   int i;
   VALUE* args = malloc(sizeof(VALUE) * params);
   for (i = 0; i < params; i++) {
      VALUE arg = (VALUE) NULL;
      switch (script_in_type(env)) {
      case SCRIPT_DOUBLE:
         arg = rb_float_new(script_in_double(env)); 
         break;
      case SCRIPT_STRING: {
         char* param = script_in_string(env);
         arg = rb_str_new2(param); 
         free(param);
         break;
      }
      case SCRIPT_BOOL:
         if (script_in_bool(env))
            arg = Qtrue; 
         else
            arg = Qfalse;
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

INLINE static VALUE script_ruby_params_to_array(int nargs, script_env* env) {
   VALUE ret;
   VALUE* rets;
   rets = script_ruby_put_params(env, nargs);
   ret = rb_ary_new4(nargs, rets);
   free(rets);
   return ret;
}

INLINE static VALUE script_ruby_params_to_value(script_env* env) {
   VALUE ret;
   int nargs;
   nargs = script_param_count(env);
   if (nargs > 0) {
      return script_ruby_params_to_array(nargs, env);
   } else
      ret = Qnil;
   return ret;
}

static VALUE script_ruby_call(VALUE self, VALUE fn_value, VALUE args) {
   script_fn fn;

   fn = (script_fn) NUM2LONG(fn_value);
   script_ruby_get_params(script_ruby_env, args);
   fn(script_ruby_env);
   return script_ruby_params_to_value(script_ruby_env);
}

static VALUE script_ruby_method_missing(int argc, VALUE *argv, VALUE self) {
   char* name;
   script_fn fn;
   name = rb_id2name(SYM2ID(argv[0]));
   fn = script_get_function(script_ruby_env, name);
   if (fn) {
      char fn_code[1024];
      VALUE args;
      snprintf(fn_code, 1023, "def %s.%s(*args); %s.call(%ld, args); end;",
         script_ruby_namespace, name, script_ruby_namespace, (long int) fn);
      rb_eval_string(fn_code);
      args = rb_ary_new4(argc - 1, argv+1);
      return script_ruby_call(self, rb_int_new((long int) fn), args);
   } else {
      script_err err;
      int i;
      script_start_params(script_ruby_env);
      for (i = 0; i < argc; i++)
         script_ruby_get_param(script_ruby_env, argv[i]);
      err = script_call(script_ruby_env, name);
      if (err != SCRIPT_OK) {
         /* FIXME: I'm getting a Ruby segfault when Ruby raises exceptions. */
         rb_raise(rb_eRuntimeError, "No such function: '%s'.", name);
         return Qnil;
      }
      return script_ruby_params_to_value(script_ruby_env);
   }
   return Qnil;
}

script_plugin_state script_plugin_init_ruby(script_env* env) {
   const char* namespace;
   int namespace_size;

   namespace = script_get_namespace(env);
   ruby_init();
   ruby_script(namespace);
   namespace_size = strlen(namespace) + 1;
   script_ruby_namespace = malloc(namespace_size);
   strncpy(script_ruby_namespace, namespace, namespace_size);
   script_ruby_env = env;
   script_ruby_namespace[0] = toupper(script_ruby_namespace[0]);
   script_ruby_class = rb_define_class(script_ruby_namespace, rb_cObject);
   method_id = rb_intern("method");
   rb_define_singleton_method(script_ruby_class, "method_missing", script_ruby_method_missing, -1);
   rb_define_singleton_method(script_ruby_class, "call", script_ruby_call, 2);
   return SCRIPT_GLOBAL_STATE;
}

int script_plugin_run_ruby(script_plugin_state state, char* programtext) {
   rb_eval_string(programtext);
   return SCRIPT_OK;
}

INLINE static VALUE script_ruby_pcall(VALUE args) {
   ID fn_id = SYM2ID(rb_ary_pop(args));
   return rb_apply(script_ruby_class, fn_id, args);
}

int script_plugin_call_ruby(script_plugin_state state, char* fn) {
   VALUE method;
   VALUE fn_value;
   VALUE args;
   int error;
   script_env* env = script_ruby_env;
   
   VALUE ret;
   fn_value = rb_str_new2(fn);
   method = rb_funcall(script_ruby_class, method_id, 1, fn_value);
   if (method == Qnil)
      return SCRIPT_ERRFNUNDEF;
   args = script_ruby_params_to_array(script_param_count(env), env);
   rb_ary_push(args, ID2SYM(rb_intern(fn)));
   ret = rb_protect(script_ruby_pcall, args, &error);
   script_start_params(script_ruby_env);
   script_ruby_get_param(script_ruby_env, ret);
   if (error) {
      script_set_error_message(env, StringValuePtr(ruby_errinfo));
      ruby_errinfo = Qnil;
      return SCRIPT_ERRLANGRUN;
   } else
      return SCRIPT_OK;
}

void script_plugin_done_ruby(script_plugin_state state) {
   ruby_finalize();
}
