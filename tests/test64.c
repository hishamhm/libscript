#include <stdio.h>
#include <libscript.h>
#include <stdlib.h>
#include <signal.h>

#define ON_ERR(s) \
   do { \
      if (err) { \
         printf("%s (error %d: %s).\n", s, err, script_error_message(env)); \
         exit(1); \
      } \
   } while (0)

script_env* env;
double128 yd;
int64 yi;

script_err return_val_d(script_env* env)
{
   yd = script_get_double(env, 0);
   SCRIPT_CHECK_INPUTS(env);
   script_reset_buffer(env);
   return SCRIPT_OK;
}


script_err return_val_i(script_env* env)
{
   yi = script_get_int(env, 0);
   SCRIPT_CHECK_INPUTS(env);
   script_reset_buffer(env);
   return SCRIPT_OK;
}

void test(int64 x)
{
   script_put_int(env, 0, x);
   script_put_int(env, 1, 0);
   int64 prev = yi;
   script_err err = script_call(env, "plugin_test");
   
   script_reset_buffer(env);
   char* res = "Daijobu";
   float delta = x-yi;
   if (delta > 0.001 || delta < -0.001)
      res = "!!ERROR!!";
   printf("%s: (int) Sent %lld got %lld back.\n", res, x, yi);
}

void test_d(double128 x)
{
   script_put_double(env, 0, x);
   script_put_int(env, 1, 1);
   double128 prev = yd;
   script_err err = script_call(env, "plugin_test");
   
   script_reset_buffer(env);
   char* res = "Daijobu";
   float delta = x-yd;
   if (delta > 0.001 || delta < -0.001)
      res = "!!ERROR!!";
   printf("%s: (double) Sent %llF got %llF back.\n", res, x, yd);
}

int main(int argc, char** argv) {
   int err;
   
   env = script_init("anirudh");
   if (!env) {
      printf("Could not init libscript.\n");
      exit(1);
   }

   script_new_function(env, return_val_i, "return_val_i");
   script_new_function(env, return_val_d, "return_val_d");
   
   err = script_run_file(env, argv[1]);
   if (err) {
      printf("Could not run script (error %d: %s).\n", err, script_error_message(env));
      exit(1);
   }

   test(32768);                                         /*  2**15  */
   test(1073741824L);                                   /*2**30  */
   test(9007199254740995LL);                            /* ~2**53  */
   test(1152921504606846976LL);                         /*2**60  */
   test_d(123.123L);
   test_d(1180591620717411303424.0L);                   /*2**112  */
   test_d(1361129467683753853853498429727072845824.0L); /*2**130  */

   script_done(env);
   return 0;
}
