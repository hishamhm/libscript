#include <stdio.h>
#include <libscript.h>
#include <stdlib.h>

#define ON_ERR(s,e) \
   do { \
      if (err) { \
         printf("%s (error %d: %s).\n", s, err, script_error_message(e)); \
         exit(1); \
      } \
   } while (0)


int main(int argc, char** argv) {
   script_env* env, *env2;
   int err;
   
   env = script_init("t1");
   env2 = script_init("t2");
   printf("\n");

   if (!env || !env2) {
      printf("Could not init libscript.\n");
      exit(1);
   }

   err = script_run(env, "perl",
      "sub T1::perl_fn {"
         "my ($x) = @_;"
         "print 'Salut from Perl, '.$x.'!!!!\n';"
      "}"
      );
   ON_ERR("Could not run Perl code on env1",env);

   err = script_run(env2, "perl",
      "sub T2::perl_fn {"
         "my ($x) = @_;"
         "print 'Salut from Perl, '.$x.'!!!!\n';"
      "}"
      );
   ON_ERR("Could not run Perl code on env2",env2);

   
   script_done(env);
   script_done(env2);
   return 0;
}
