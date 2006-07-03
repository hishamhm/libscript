
#include <ruby.h>

ID a_ruby_function_ID;

VALUE a_C_block() {
   fprintf(stderr, "a_C_block is running.\n");
}

VALUE call_a_ruby_function() {
   return rb_funcall(Qnil, a_ruby_function_ID, 0);
}

int main(int argc, char** argv) {
   VALUE a_proc;
   VALUE result;

   ruby_init();
   
   a_ruby_function_ID = rb_intern("a_ruby_function");
   
   /* This doesn't work: */
   /*

   rb_eval_string(
   "   def a_ruby_function() \n"
   "      print(\"a_ruby_function will yield.\\n\") \n"
   "      yield \n"
   "      print(\"a_ruby_function is done.\n\") \n"
   "   end \n"
   );

   a_proc = rb_proc_new(a_C_block, Qnil);
   rb_funcall(Qnil, a_ruby_function_ID, 1, a_proc);

   */

   /* This works: */
   /*

   rb_eval_string(
   "   def a_ruby_function(block) \n"
   "      print(\"a_ruby_function will yield.\\n\") \n"
   "      block.call \n"
   "      print(\"a_ruby_function is done.\n\") \n"
   "   end \n"
   );

   a_proc = rb_proc_new(a_C_block, Qnil);
   rb_funcall(Qnil, a_ruby_function_ID, 1, a_proc);
   
   */

   /* To handle yielding functions from C we have to do this: */

   rb_eval_string(
   "   def a_ruby_function() \n"
   "      print(\"\\na_ruby_function will yield.\\n\") \n"
   "      yield \n"
   "      print(\"a_ruby_function is done.\n\") \n"
   "      return 42 \n"
   "   end \n"
   );

   result = rb_iterate(call_a_ruby_function, Qnil, a_C_block, Qnil);
   fprintf(stderr, "Result = %d\n", NUM2INT(result));

   /* Or this... */
   
   a_proc = rb_proc_new(a_C_block, Qnil);
   rb_gv_set("$a_proc", a_proc);
   rb_eval_string("$result = a_ruby_function(&$a_proc)");
   rb_gv_get("$result");
   fprintf(stderr, "Result = %d\n", NUM2INT(result));

   /* Or this... */
   
   rb_define_global_function("a_C_block", a_C_block, 0);
   rb_eval_string("$result = a_ruby_function { a_C_block }");
   rb_gv_get("$result");
   fprintf(stderr, "Result = %d\n", NUM2INT(result));
   
   ruby_finalize();
   
   return 0;
}
