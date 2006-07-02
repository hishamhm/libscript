
#include <ruby.h>

ID a_ruby_function_ID;

VALUE a_C_block() {
   fprintf(stderr, "a_C_block is running.\n");
}

VALUE call_a_ruby_function() {
   rb_funcall(Qnil, a_ruby_function_ID, 0);
}

int main(int argc, char** argv) {
   VALUE a_proc;

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
   "      print(\"a_ruby_function will yield.\\n\") \n"
   "      yield \n"
   "      print(\"a_ruby_function is done.\n\") \n"
   "   end \n"
   );

   rb_iterate(call_a_ruby_function, Qnil, a_C_block, Qnil);

   ruby_finalize();
   
   return 0;
}
