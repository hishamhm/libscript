
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

script_err c_fn(script_env* env) {
   char* name = script_get_string(env, 0);
   SCRIPT_CHECK_INPUTS(env);
   printf("Ciao from C, %s.\n", name);
   script_reset_buffer(env);
   return SCRIPT_OK;
}

int main(int argc, char** argv) {
   script_env* env;
   int err;
   
   env = script_init("testcall");
   printf("\n");

   if (!env) {
      printf("Could not init libscript.\n");
      exit(1);
   }

   script_new_function(env, c_fn, "c_fn");
   
   err = script_run(env, "lua",
      "function testcall.lua_fn(x) "
         "print('Hello from Lua, '..x..'!') "
      "end");
   ON_ERR("Could not run Lua code");

   err = script_run(env, "python",
      "def python_fn(x): "
      "   print('Cheers from Python, ' + x + '!!')\n"
      "\n"
      "testcall.python_fn = python_fn\n");
   ON_ERR("Could not run Python code");

   err = script_run(env, "ruby",
      "def Testcall.ruby_fn(x)\n"
         "print('Aloha from Ruby, '+x.to_s+'!!!\n')\n"
      "end\n");
   ON_ERR("Could not run Ruby code");

   err = script_run(env, "perl",
      "sub Testcall::perl_fn {"
         "my ($x) = @_;"
         "print 'Salut from Perl, '.$x.'!!!!\n';"
      "}"
      );
   ON_ERR("Could not run Perl code");

   printf("Calling Lua from C...\n");
   script_put_string(env, 0, "C");
   err = script_call(env, "lua_fn");
   ON_ERR("Could not call Lua function");
   printf("\n");

   err = script_run(env, "lua", 
      "print('Calling C from Lua...') "
      "testcall.c_fn('Lua')");
   ON_ERR("Could not run Lua code");
   printf("\n");

   printf("Calling Python from C...\n");
   script_put_string(env, 0, "C");
   err = script_call(env, "python_fn");
   ON_ERR("Could not call Python function");
   printf("\n");

   err = script_run(env, "py", 
      "print('Calling C from Python...'); "
      "testcall.c_fn('Python')");
   ON_ERR("Could not run Python code");
   printf("\n");

   printf("Calling Ruby from C...\n");
   script_put_string(env, 0, "C");
   err = script_call(env, "ruby_fn");
   ON_ERR("Could not call Ruby function");
   printf("\n");

   err = script_run(env, "perl", 
      "print 'Calling C from Perl...\n'; "
      "Testcall::c_fn('Perl');");
   ON_ERR("Could not run Perl code");
   printf("\n");

   printf("Calling Perl from C...\n");
   script_put_string(env, 0, "C");
   err = script_call(env, "perl_fn");
   ON_ERR("Could not call Perl function");
   printf("\n");

   err = script_run(env, "rb", 
      "print('Calling C from Ruby...\n'); "
      "Testcall.c_fn('Ruby')");
   ON_ERR("Could not run Ruby code");
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

   err = script_run(env, "perl", 
      "print 'Calling Python from Perl...\n'; "
      "Testcall::python_fn('Perl');");
   ON_ERR("Could not run Perl code");
   printf("\n");

   err = script_run(env, "py", 
      "print('Calling Perl from Python...'); "
      "testcall.perl_fn('Python')");
   ON_ERR("Could not run Python code");
   printf("\n");

   err = script_run(env, "perl", 
      "print 'Calling Ruby from Perl...\n'; "
      "Testcall::ruby_fn('Perl');");
   ON_ERR("Could not run Perl code");
   printf("\n");

   err = script_run(env, "rb", 
      "print('Calling Perl from Ruby...\n'); "
      "Testcall.perl_fn('Ruby')");
   ON_ERR("Could not run Ruby code");
   printf("\n");

   err = script_run(env, "perl", 
      "print 'Calling Lua from Perl...\n'; "
      "Testcall::lua_fn('Perl');");
   ON_ERR("Could not run Perl code");
   printf("\n");

   err = script_run(env, "lua", 
      "print('Calling Perl from Lua...') "
      "testcall.perl_fn('Lua')");
   ON_ERR("Could not run Lua code");
   printf("\n");
   
   script_done(env);
   return 0;
}
