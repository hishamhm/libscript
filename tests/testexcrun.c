
#include <stdio.h>
#include <libscript.h>
#include <stdlib.h>

int main(int argc, char** argv) {
   script_env* env;
   int err;
   
   if (argc < 2) {
      printf("Missing argument.\n");
      exit(1);
   }
   env = script_init("testexcrun");
   if (!env) {
      printf("Could not init libscript.\n");
      exit(1);
   }
   err = script_run_file(env, argv[1]);
   if (err == SCRIPT_ERRLANGRUN) {
      printf("Ok, got the expected error: %d: %s.\n", err, script_error_message(env));
   } else {
      printf("Oops, did not get the expected error. Got %d: %s.\n", err, script_error_message(env));
      exit(1);
   }
   script_done(env);
   return 0;
}
