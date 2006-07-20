
#include <unistd.h>
#include <ruby.h>

int main(int argc, char** argv) {
   int i, j;
   if (argc < 2)
      return 0;
   for (i = 0; i < atoi(argv[1]); i++) {
      ruby_init();
      for (j = 0; j < 1000000; j++) {
         char name[20];
         sprintf(name, "foo_%d_bar_%d", i, j);
         rb_intern(name);
      }
      ruby_finalize();
   }
   sleep(30.0);
   return 0;
}
