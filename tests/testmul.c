
#include <stdio.h>
#include <libscript.h>
#include <stdlib.h>


int main(int argc, char** argv) {
   script_env* env1, *env2;
   int err1, err2, status_code;
   
   env1 = script_init("env1");
   env2 = script_init("env2");
   
   if (!env1 || !env2) {
      printf("Could not init libscript.\n");
      exit(1);
   }
   err1 = script_run_file(env1, argv[1]);
   if (err1) {
      printf("ERROR: Could not start first instance of Perl (error %d: %s).\n", err1, script_error_message(env1));
   }
   err2 = script_run_file(env2, argv[1]);
   if (err2) {
      printf("Could not start second instance of Perl (error %d: %s).\n", err2, script_error_message(env2));
      printf("This is natural if your system doesn't support -Dusemultiplicity.\n");
   }
   script_done(env1);
   script_done(env2);
   printf("\n\nBoth instances of Perl have been terminated.\n\n");
   env1 = script_init("env1");
   env2 = script_init("env2");
   
   if (!env1 || !env2) {
      printf("Could not init libscript again.\n");
      exit(1);
   }
   err1 = script_run_file(env1, argv[1]);
   if (err1) {
      printf("ERROR: Could not start first instance of Perl. (error %d: %s).\n", err1, script_error_message(env1));
   }
   err2 = script_run_file(env2, argv[1]);
   if (err2) {
      printf("Could not start second instance of Perl (error %d: %s).\n", err2, script_error_message(env2));
      printf("This is natural if your system doesn't support -Dusemultiplicity.\n");
   }
   return err1;
}
