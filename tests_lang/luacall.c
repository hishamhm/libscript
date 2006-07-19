
#include <lua.h>
#include <lauxlib.h>

int main() {
   lua_State* L;

   L = luaL_newstate();
   luaL_openlibs(L);
   luaL_dostring(L, "function teste(a, b) \n"
                    "   print (a, b) \n"
                    "   return 42 \n"
                    "end \n"
   );
   lua_getglobal(L, "teste");
   lua_pushstring(L, "entrada");
   lua_pushinteger(L, 2);
   lua_call(L, 2, 1);
   long result = lua_tointeger(L, -1);
   lua_pop(L, 1);
   printf("The result is %ld \n", result);
   
   lua_close(L);
}
