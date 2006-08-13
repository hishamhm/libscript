
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
static ID env_id;

static int script_ruby_state_count = 0;

INLINE static void script_ruby_put_value(script_env* env, int i, VALUE arg) {
   switch (TYPE(arg)) {
   case T_FLOAT:
   case T_FIXNUM:
   case T_BIGNUM:
      script_put_double(env, i, NUM2DBL(arg)); break;
   case T_STRING:
      script_put_string(env, i, StringValuePtr(arg)); break;
   case T_TRUE:
      script_put_bool(env, i, 1); break;
   case T_FALSE:
      script_put_bool(env, i, 0); break;
   default:
      script_put_int(env, i, 0); break;
   }
}

INLINE static void script_ruby_array_to_buffer(script_env* env, VALUE array) {
   int i;
   int len = RARRAY(array)->len;
   script_reset_buffer(env);
   for (i = 0; i < len; i++) {
      VALUE o = rb_ary_entry(array, i);
      script_ruby_put_value(env, i, o);
   }
}

INLINE static VALUE script_ruby_get_value(script_env* env, int i) {
   VALUE ret; char* s;
   switch (script_get_type(env, i)) {
   case SCRIPT_DOUBLE:
      return rb_float_new(script_get_double(env, i)); 
   case SCRIPT_STRING:
      s = script_get_string(env, i);
      ret = rb_str_new2(s);
      free(s);
      return ret;
   case SCRIPT_BOOL:
      return script_get_bool(env, i) ? Qtrue : Qfalse;
   default:
      /* pacify gcc warnings */
      assert(0);
      return Qnil;
   }
}

INLINE static VALUE script_ruby_buffer_to_array(script_env* env) {
   int i;
   int len = script_buffer_size(env);
   VALUE ret = rb_ary_new2(len);
   for (i = 0; i < len; i++) {
      VALUE o = script_ruby_get_value(env, i);
      rb_ary_store(ret, i, o);
   }
   return ret;
}

INLINE static script_env* script_ruby_get_env(VALUE state) {
   /* assumes a void* fits in long */
   return (script_env*) NUM2LONG(rb_const_get(state, env_id));
}

static VALUE script_ruby_method_missing(int argc, VALUE *argv, VALUE self) {
   script_env* env = script_ruby_get_env(self);
   char *method_name = rb_id2name(SYM2ID(argv[0]));
   script_err err;
   int i;
   script_reset_buffer(env);
   for (i = 1; i < argc; i++)
      script_ruby_put_value(env, i-1, argv[i]);
   err = script_call(env, method_name);
   if (err != SCRIPT_OK)
      /* Raises exception and longjmps away from this function. */
      rb_raise(rb_eRuntimeError, script_error_message(env));
   switch (script_buffer_size(env)) {
   case 0:
      return Qnil;
   case 1:
      return script_ruby_get_value(env, 0);
   default:
      return script_ruby_buffer_to_array(env);
   }
}

INLINE char* script_ruby_make_class_name(const char* name) {
   int name_size = strlen(name) + 1;
   char* class_name = malloc(name_size);
   strncpy(class_name, name, name_size);
   class_name[0] = toupper(class_name[0]);
   return class_name;
}

script_plugin_state script_plugin_init_ruby(script_env* env) {
   VALUE state;
   char *class_name;

   if (script_ruby_state_count == 0) {
      ruby_init();
      ruby_script("libscript");
   }
   env_id = rb_intern("@@LibScriptEnv");
   method_id = rb_intern("method");
   script_ruby_state_count++;
   class_name = script_ruby_make_class_name(script_namespace(env));
   state = rb_define_class(class_name, rb_cObject);
   free(class_name);
   /* assuming a void* fits in a long */
   rb_const_set(state, env_id, INT2NUM((long int)env));
   rb_define_singleton_method(state, "method_missing", script_ruby_method_missing, -1);
   return (script_plugin_state) state;
}

INLINE int script_ruby_error(script_env* env) {
   script_reset_buffer(env);
   script_set_error_message(env, StringValuePtr(ruby_errinfo));
   ruby_errinfo = Qnil;
   return SCRIPT_ERRLANGRUN;
}

int script_plugin_run_ruby(script_ruby_state state, char* programtext) {
   script_env* env = script_ruby_get_env(state);
   int error;
   rb_eval_string_protect(programtext, &error);
   if (error)
      return script_ruby_error(env);
   else
      return SCRIPT_OK;
}

INLINE static VALUE script_ruby_pcall(VALUE args) {
   ID fn_id = SYM2ID(rb_ary_pop(args));
   VALUE klass = rb_ary_pop(args);
   return rb_apply(klass, fn_id, args);
}

int script_plugin_call_ruby(script_ruby_state state, char* fn) {
   script_env* env = script_ruby_get_env(state);
   int error;
   VALUE ret;
   VALUE args;
   VALUE fn_value = rb_str_new2(fn);
   VALUE method = rb_funcall(state, method_id, 1, fn_value);
   if (method == Qnil)
      return SCRIPT_ERRFNUNDEF;
   args = script_ruby_buffer_to_array(env);
   rb_ary_push(args, state);
   rb_ary_push(args, ID2SYM(rb_intern(fn)));
   ret = rb_protect(script_ruby_pcall, args, &error);
   if (error)
      return script_ruby_error(env);
   if (ret == Qnil)
      script_reset_buffer(env);
   else if (TYPE(ret) == T_ARRAY)
      script_ruby_array_to_buffer(env, ret);
   else
      script_ruby_put_value(env, 0, ret);
   return SCRIPT_OK;
}

void script_plugin_done_ruby(script_ruby_state state) {
   script_ruby_state_count--;
   if (script_ruby_state_count == 0)
      ruby_finalize();
}
