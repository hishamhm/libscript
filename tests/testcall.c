
#include <stdio.h>
#include <libscript.h>
#include <stdlib.h>

int main(int argc, char** argv) {
   script_env* env;
   int err;
   
   env = script_init("hisham");

   if (!env) {
      printf("Could not init libscript.\n");
      exit(1);
   }
   
   err = script_run(env, "lua", "function hisham.say_hello(x) print('Hello, '..x..'!') end");

   if (err) {
      printf("Could not run Lua code (error %d: %s).\n", err, script_error_message(env));
      exit(1);
   }

   err = script_run(env, "py", "def say_goodbye(x): print('Bye, ' + x + '...')\n\nhisham.say_goodbye = say_goodbye\n");

   if (err) {
      printf("Could not run Python code (error %d: %s).\n", err, script_error_message(env));
      exit(1);
   }
   
   script_out_string(env, "Hisham");
   err = script_call(env, "say_hello");
   script_out_string(env, "Jorge");
   err = script_call(env, "say_goodbye");

   if (err) {
      printf("Could not call function (error %d: %s).\n", err, script_error_message(env));
      exit(1);
   }
   
   script_done(env);
   return 0;
}
