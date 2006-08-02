
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

static ID method_id;

static int script_ruby_state_count = 0;

INLINE static void script_ruby_get_param(script_env* env, VALUE arg) {
   if (arg == Qtrue) {
      script_put_bool(env, 1);
      return;
   } else if (arg == Qfalse) {
      script_put_bool(env, 0);
      return;
   }
   switch (TYPE(arg)) {
   case T_FLOAT:
   case T_FIXNUM:
   case T_BIGNUM:
      script_put_double(env, NUM2DBL(arg));
      break;
   case T_STRING:
      script_put_string(env, StringValuePtr(arg));
      break;
   default:;
      /* TODO: other types */
   }
}

INLINE static void script_ruby_get_params(script_env* env, VALUE args) {
   VALUE arg;

   script_params(env);
   while ( (arg = rb_ary_shift(args)) != Qnil ) {
      script_ruby_get_param(env, arg);
   }
}

INLINE static VALUE* script_ruby_put_params(script_env* env, int params) {
   int i;
   VALUE* args = malloc(sizeof(VALUE) * params);
   for (i = 0; i < params; i++) {
      VALUE arg = (VALUE) NULL;
      switch (script_get_type(env)) {
      case SCRIPT_DOUBLE:
         arg = rb_float_new(script_get_double(env)); 
         break;
      case SCRIPT_STRING: {
         char* param = script_get_string(env);
         arg = rb_str_new2(param); 
         free(param);
         break;
      }
      case SCRIPT_BOOL:
         if (script_get_bool(env))
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

INLINE static script_ruby_state* script_ruby_get_state_from_class(VALUE klass) {
   /* assumes a void* fits in long */
   return (script_ruby_state*) NUM2LONG(rb_cv_get(klass, "@@_state"));
}

static VALUE script_ruby_call(VALUE self, VALUE fn_value, VALUE args) {
   script_ruby_state* state = script_ruby_get_state_from_class(self);
   script_fn fn = (script_fn) NUM2LONG(fn_value);
   script_ruby_get_params(state->env, args);
   fn(state->env);
   return script_ruby_params_to_value(state->env);
}

static VALUE script_ruby_method_missing(int argc, VALUE *argv, VALUE self) {
   script_ruby_state* state = script_ruby_get_state_from_class(self);
   char *name = rb_id2name(SYM2ID(argv[0]));
   script_fn fn = script_get_function(state->env, name);
   if (fn) {
      char fn_code[1024];
      VALUE args;
      snprintf(fn_code, 1023, "def %s.%s(*args); %s.call(%ld, args); end;",
         state->name, name, state->name, (long int) fn);
      rb_eval_string(fn_code);
      args = rb_ary_new4(argc - 1, argv+1);
      return script_ruby_call(self, rb_int_new((long int) fn), args);
   } else {
      script_err err;
      int i;
      script_params(state->env);
      for (i = 0; i < argc; i++)
         script_ruby_get_param(state->env, argv[i]);
      err = script_call(state->env, name);
      if (err != SCRIPT_OK) {
         /* FIXME: I'm getting a Ruby segfault when Ruby raises exceptions. */
         rb_raise(rb_eRuntimeError, "No such function: '%s'.", name);
         return Qnil;
      }
      return script_ruby_params_to_value(state->env);
   }
   return Qnil;
}

script_plugin_state script_plugin_init_ruby(script_env* env) {
   script_ruby_state* state;
   const char* name;
   int name_size;

   name = script_get_namespace(env);
   if (script_ruby_state_count == 0) {
      ruby_init();
      ruby_script("libscript");
   }
   script_ruby_state_count++;
   state = malloc(sizeof(script_ruby_state));
   name_size = strlen(name) + 1;
   state->name = malloc(name_size);
   strncpy(state->name, name, name_size);
   state->env = env;
   state->name[0] = toupper(state->name[0]);
   state->klass = rb_define_class(state->name, rb_cObject);
   /* assuming a void* fits in a long */
   rb_cv_set(state->klass, "@@_state", INT2NUM((long int)state));
   method_id = rb_intern("method");
   rb_define_singleton_method(state->klass, "method_missing", script_ruby_method_missing, -1);
   rb_define_singleton_method(state->klass, "call", script_ruby_call, 2);
   return state;
}

int script_plugin_run_ruby(script_plugin_state state, char* programtext) {
   rb_eval_string(programtext);
   return SCRIPT_OK;
}

INLINE static VALUE script_ruby_pcall(VALUE args) {
   ID fn_id = SYM2ID(rb_ary_pop(args));
   VALUE klass = rb_ary_pop(args);
   return rb_apply(klass, fn_id, args);
}

int script_plugin_call_ruby(script_ruby_state* state, char* fn) {
   script_env* env = state->env;
   VALUE args;
   int error;
   VALUE ret;
   VALUE fn_value = rb_str_new2(fn);
   VALUE method = rb_funcall(state->klass, method_id, 1, fn_value);
   if (method == Qnil)
      return SCRIPT_ERRFNUNDEF;
   args = script_ruby_params_to_array(script_param_count(env), env);
   rb_ary_push(args, state->klass);
   rb_ary_push(args, ID2SYM(rb_intern(fn)));
   ret = rb_protect(script_ruby_pcall, args, &error);
   script_params(state->env);
   script_ruby_get_param(state->env, ret);
   if (error) {
      script_set_error_message(env, StringValuePtr(ruby_errinfo));
      ruby_errinfo = Qnil;
      return SCRIPT_ERRLANGRUN;
   } else
      return SCRIPT_OK;
}

void script_plugin_done_ruby(script_ruby_state* state) {
   script_ruby_state_count--;
   free(state->name);
   free(state);
   if (script_ruby_state_count == 0) {
      ruby_finalize();
   }
}
