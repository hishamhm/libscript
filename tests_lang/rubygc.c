
#include <ruby.h>

static VALUE a_class;
static ID a_method_ID;

static VALUE a_method(VALUE self) {
   VALUE an_object;
   VALUE will_i_get_this;
   int i;
   
   an_object = rb_class_new_instance(0, NULL, a_class);
   rb_iv_set(an_object, "@an_attribute", rb_str_new2(strdup("42")));
   for (i = 0; i < 10000; i++) {
      VALUE some_garbage = rb_ary_new();
      rb_ary_push(some_garbage, rb_str_new2(strdup("666")));
      /* rb_eval_string("print 'objects: ', ObjectSpace.each_object {}, '\n'"); */
   }
   
   /*
   This is still valid because Ruby scans the C stack to protect its VALUES.
   See [ruby-core:7983]
   */
   will_i_get_this = rb_iv_get(an_object, "@an_attribute");
   fprintf(stderr, "The answer is %s.\n", RSTRING(will_i_get_this)->ptr);
}

int main(int argc, char** argv) {
   VALUE an_instance;

   ruby_init();
   
   a_class = rb_define_class("RubyGcTest", rb_cObject);
   rb_define_method(a_class, "a_method", a_method, 0);
   an_instance = rb_class_new_instance(0, NULL, a_class);
   a_method_ID = rb_intern("a_method");
   rb_funcall2(an_instance, a_method_ID, 0, NULL);
   ruby_finalize();
   
   return 0;
}
