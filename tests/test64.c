
#include <stdio.h>
#include <libscript.h>
#include <stdlib.h>
#include <signal.h>

long double ret[10] = {20.412, 9007199254740995, 9007199254740995213, 3743832098472309478323287435};
int count = 0;

script_err hello_wrap(script_env* env) {
   long long got = script_get_int(env, 0);
   SCRIPT_CHECK_INPUTS(env);

   printf("C took %lld as an integer\n", got);
   printf("C is sending %lld as an integer\n", (long long)ret[count]);

   script_put_int(env, 0, (long long)ret[count]);
   return SCRIPT_OK;
}

script_err hello_wrap2(script_env* env) {
   long double got = script_get_double(env, 0);
   SCRIPT_CHECK_INPUTS(env);

   printf("C took %Lf as a double\n", got);
   printf("C is sending %Lf as a double\n", (long double)ret[count]);

   script_put_double(env, 0, (long double)ret[count]);
   count++;
   return SCRIPT_OK;
}


int main(int argc, char** argv) {
   script_env* env;
   int err;
   
   env = script_init("anirudh");
   if (!env) {
      printf("Could not init libscript.\n");
      exit(1);
   }
   script_new_function(env, hello_wrap, "hello");
   script_new_function(env, hello_wrap2, "hello2");
   
   err = script_run_file(env, argv[1]);
   if (err) {
      printf("Could not run script (error %d: %s).\n", err, script_error_message(env));
      exit(1);
   }
   script_done(env);
   return 0;
}
