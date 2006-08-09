
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
static ID state_id;
static ID name_id;

static int script_ruby_state_count = 0;

INLINE static void script_ruby_put_value(script_env* env, int i, VALUE arg) {
   if (arg == Qtrue) {
      script_put_bool(env, i, 1);
      return;
   } else if (arg == Qfalse || arg == Qnil) {
      script_put_bool(env, i, 0);
      return;
   }
   switch (TYPE(arg)) {
   case T_FLOAT:
   case T_FIXNUM:
   case T_BIGNUM:
      script_put_double(env, i, NUM2DBL(arg));
      break;
   case T_STRING:
      script_put_string(env, i, StringValuePtr(arg));
      break;
   default:;
      /* TODO: other types */
      assert(0);
   }
}

INLINE static VALUE script_ruby_params_to_array(int nargs, script_env* env) {
   VALUE ret;
   int i;
   VALUE* args = malloc(sizeof(VALUE) * nargs);
   for (i = 0; i < nargs; i++) {
      VALUE arg = (VALUE) NULL;
      switch (script_get_type(env, i)) {
      case SCRIPT_DOUBLE:
         arg = rb_float_new(script_get_double(env, i)); 
         break;
      case SCRIPT_STRING: {
         char* param = script_get_string(env, i);
         arg = rb_str_new2(param); 
         free(param);
         break;
      }
      case SCRIPT_BOOL:
         if (script_get_bool(env, i))
            arg = Qtrue; 
         else
            arg = Qfalse;
         break;
      default:
         /* pacify gcc warnings */
         assert(0);
      }
      args[i] = arg;
   }
   ret = rb_ary_new4(nargs, args);
   free(args);
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
   return (script_ruby_state*) NUM2LONG(rb_const_get(klass, state_id));
}

static VALUE script_ruby_caller(VALUE self, VALUE fn_value, VALUE args) {
   int i, len;
   script_ruby_state* state = script_ruby_get_state_from_class(self);
   script_fn fn = (script_fn) NUM2LONG(fn_value);
   len = RARRAY(args)->len;
   script_reset_params(state->env);
   for (i = 0; i < len; i++)
      script_ruby_put_value(state->env, i, RARRAY(args)->ptr[i]);
   fn(state->env);
   return script_ruby_params_to_value(state->env);
}

static VALUE script_ruby_method_missing(int argc, VALUE *argv, VALUE self) {
   script_ruby_state* state = script_ruby_get_state_from_class(self);
   char *method_name = rb_id2name(SYM2ID(argv[0]));
   VALUE name_value = rb_funcall(self, name_id, 0);
   char *class_name = StringValuePtr(name_value);
   script_fn fn = script_function(state->env, method_name);
   if (fn) {
      char fn_code[1024];
      VALUE args;
      snprintf(fn_code, 1023, "def %s.%s(*args); %s.caller(%ld, args); end;",
         class_name, method_name, class_name, (long int) fn);
      rb_eval_string(fn_code);
      args = rb_ary_new4(argc - 1, argv+1);
      return script_ruby_caller(self, rb_int_new((long int) fn), args);
   } else {
      script_err err;
      int i;
      script_reset_params(state->env);
      for (i = 1; i < argc; i++)
         script_ruby_put_value(state->env, i-1, argv[i]);
      err = script_call(state->env, method_name);
      if (err != SCRIPT_OK) {
         /* FIXME: I'm getting a Ruby segfault when Ruby raises exceptions. */
         rb_raise(rb_eRuntimeError, script_error_message(state->env));
         return Qnil;
      }
      return script_ruby_params_to_value(state->env);
   }
   return Qnil;
}

script_plugin_state script_plugin_init_ruby(script_env* env) {
   script_ruby_state* state;
   const char *name;
   char *ruby_name;
   int name_size;

   name = script_namespace(env);
   if (script_ruby_state_count == 0) {
      ruby_init();
      ruby_script("libscript");
   }
   state_id = rb_intern("@@State");
   name_id = rb_intern("name");
   method_id = rb_intern("method");
   script_ruby_state_count++;
   state = malloc(sizeof(script_ruby_state));
   name_size = strlen(name) + 1;
   ruby_name = malloc(name_size);
   strncpy(ruby_name, name, name_size);
   state->env = env;
   ruby_name[0] = toupper(ruby_name[0]);
   state->klass = rb_define_class(ruby_name, rb_cObject);
   /* assuming a void* fits in a long */
   rb_const_set(state->klass, state_id, INT2NUM((long int)state));
   rb_define_singleton_method(state->klass, "method_missing", script_ruby_method_missing, -1);
   rb_define_singleton_method(state->klass, "caller", script_ruby_caller, 2);
   free(ruby_name);
   return state;
}

INLINE int script_ruby_return(script_ruby_state* state, int error) {
   if (error) {
      script_set_error_message(state->env, StringValuePtr(ruby_errinfo));
      ruby_errinfo = Qnil;
      return SCRIPT_ERRLANGRUN;
   } else
      return SCRIPT_OK;
}

int script_plugin_run_ruby(script_ruby_state* state, char* programtext) {
   int error;
   rb_eval_string_protect(programtext, &error);
   return script_ruby_return(state, error);
}

INLINE static VALUE script_ruby_pcall(VALUE args) {
   ID fn_id = SYM2ID(rb_ary_pop(args));
   VALUE klass = rb_ary_pop(args);
   return rb_apply(klass, fn_id, args);
}

int script_plugin_call_ruby(script_ruby_state* state, char* fn) {
   int error;
   VALUE ret;
   script_env* env = state->env;
   VALUE args;
   VALUE fn_value = rb_str_new2(fn);
   VALUE method = rb_funcall(state->klass, method_id, 1, fn_value);
   if (method == Qnil)
      return SCRIPT_ERRFNUNDEF;
   args = script_ruby_params_to_array(script_param_count(env), env);
   rb_ary_push(args, state->klass);
   rb_ary_push(args, ID2SYM(rb_intern(fn)));
   ret = rb_protect(script_ruby_pcall, args, &error);
   script_reset_params(env);
   if (!error && ret != Qnil)
      script_ruby_put_value(state->env, 0, ret);
   return script_ruby_return(state, error);
}

void script_plugin_done_ruby(script_ruby_state* state) {
   script_ruby_state_count--;
   if (script_ruby_state_count == 0)
      ruby_finalize();
   free(state);
}
