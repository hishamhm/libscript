
#include <libscript-plugin.h>

/* Ouch! Ruby's headers are not ANSI C! */
#define inline 

/* Since we're not ANSI C anyway, use snprintf */
#define _GNU_SOURCE
#include <stdio.h>
#include <ctype.h>

#include <ruby.h>


#include "libscript-rb.h"

static VALUE script_rb_class;
static script_env* script_rb_env;
static char* script_rb_namespace;
static ID method_id;

static VALUE script_rb_call(VALUE self, VALUE fn_value, VALUE args) {
   script_fn fn;
   VALUE arg;

   script_flush_params(script_rb_env);
   
   fn = (script_fn) NUM2LONG(fn_value);
   while ( (arg = rb_ary_shift(args)) != Qnil ) {
      switch (TYPE(arg)) {
      case T_FLOAT:
      case T_FIXNUM:
      case T_BIGNUM:
         script_out_double(script_rb_env, NUM2DBL(arg));
         break;
      case T_STRING:
         script_out_string(script_rb_env, STR2CSTR(arg));
         break;
      default:;
         /* TODO: other types */
      }
   }
   script_reset_outs(script_rb_env);

   fn(script_rb_env);

   return Qnil;
}

static VALUE script_rb_get(VALUE klass, VALUE name_value) {
   char* name;
   script_fn fn;
   VALUE method;
   
   name = STR2CSTR(name_value);
   fn = script_get_function(script_rb_env, name);
   if (fn) {
      char fn_code[1024];
      snprintf(fn_code, 1023, "def %s.%s(*args); %s.call(%ld, args); end;", script_rb_namespace, name, script_rb_namespace, (long int) fn);
      rb_eval_string(fn_code);
      method = rb_funcall(script_rb_class, method_id, 1, name_value);
      return method;
   }
   return Qnil;
}

script_plugin_state script_plugin_init_rb(script_env* env, char* namespace) {
   int namespace_size;

   ruby_init();
   ruby_script(namespace);
   namespace_size = strlen(namespace) + 1;
   script_rb_namespace = malloc(namespace_size);
   strncpy(script_rb_namespace, namespace, namespace_size);
   script_rb_env = env;
   script_rb_namespace[0] = toupper(script_rb_namespace[0]);
   script_rb_class = rb_define_class(script_rb_namespace, rb_cObject);
   method_id = rb_intern("method");
   rb_define_singleton_method(script_rb_class, "get", script_rb_get, 1);
   rb_define_singleton_method(script_rb_class, "call", script_rb_call, 2);
   return SCRIPT_GLOBAL_STATE;
}

int script_plugin_run_rb(script_plugin_state state, char* programtext) {
   rb_eval_string(programtext);
   return SCRIPT_OK;
}

void script_plugin_done_rb(script_plugin_state state) {
}
