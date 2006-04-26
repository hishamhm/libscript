
#include <stdio.h>
#include <libscript.h>
#include <stdlib.h>

script_err hello_wrap(script_env* env) {
   const char* name = script_in_string(env);
   int year = script_in_int(env);
   const char* excl = script_in_string(env);
   int ret;
   SCRIPT_CHECK_INPUTS(env);

   ret = hello(name, year, excl);

   script_out_int(env, ret);
   return SCRIPT_OK;
}

int hello(char* name, int year, char* excl) {
   int age = 2006 - year;
   printf("Hello, %s! You are %d years old%s\n", name, age, excl);
   return age;
}

int main(int argc, char** argv) {
   script_env* env;
   int err;
   
   env = script_init("hisham");
   if (!env) {
      printf("Could not init libscript.\n");
      exit(1);
   }
   script_new_function(env, hello_wrap, "hello");
   err = script_run_file(env, argv[1]);
   if (err) {
      printf("Could not run script (error %d: %s).\n", err, script_error_message(env));
      exit(1);
   }
   script_done(env);
   return 0;
}
