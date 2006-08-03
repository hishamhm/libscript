
#include <stdio.h>
#include <libscript.h>
#include <stdlib.h>

#define ON_ERR(s) \
   do { \
      if (err) { \
         printf("%s (error %d: %s).\n", s, err, script_error_message(env)); \
         exit(1); \
      } \
   } while (0)

int main(int argc, char** argv) {
   script_env* env;
   int err;
   
   env = script_init("testcall");
   printf("\n");

   if (!env) {
      printf("Could not init libscript.\n");
      exit(1);
   }
   
   err = script_run(env, "lua", "function testcall.lua_fn(x) print('Hello from Lua, '..x..'!') end");
   ON_ERR("Could not run Lua code");

   err = script_run(env, "py", "def python_fn(x): print('Cheers from Python, ' + x + '!!')\n\ntestcall.python_fn = python_fn\n");
   ON_ERR("Could not run Python code");

   err = script_run(env, "rb", "def Testcall.ruby_fn(x)\n print('Aloha from Ruby, '+x.to_s+'!!!\n')\nend\n");
   ON_ERR("Could not run Ruby code");

   printf("Calling Lua from C...\n");
   script_put_string(env, 0, "C");
   err = script_call(env, "lua_fn");
   ON_ERR("Could not call Lua function");
   printf("\n");

   printf("Calling Python from C...\n");
   script_put_string(env, 0, "C");
   err = script_call(env, "python_fn");
   ON_ERR("Could not call Python function");
   printf("\n");

   printf("Calling Ruby from C...\n");
   script_put_string(env, 0, "C");
   err = script_call(env, "ruby_fn");
   ON_ERR("Could not call Ruby function");
   printf("\n");

   err = script_run(env, "lua",
      "print('Calling Python from Lua...'); "
      "testcall.python_fn('Lua')");
   ON_ERR("Could not run Lua code");
   printf("\n");

   err = script_run(env, "py",
      "print('Calling Lua from Python...'); "
      "testcall.lua_fn('Python')");
   ON_ERR("Could not run Python code");
   printf("\n");

   err = script_run(env, "lua",
      "print('Calling Ruby from Lua...'); "
      "testcall.ruby_fn('Lua')");
   ON_ERR("Could not run Lua code");
   printf("\n");

   err = script_run(env, "rb",
      "print('Calling Lua from Ruby...\n'); "
      "Testcall.lua_fn('Ruby')");
   ON_ERR("Could not run Ruby code");
   printf("\n");

   err = script_run(env, "py", 
      "print('Calling Ruby from Python...'); "
      "testcall.ruby_fn('Python')");
   ON_ERR("Could not run Python code");
   printf("\n");

   err = script_run(env, "rb", 
      "print('Calling Python from Ruby...\n'); "
      "Testcall.python_fn('Ruby')");
   ON_ERR("Could not run Ruby code");
   printf("\n");
   
   script_done(env);
   return 0;
}
