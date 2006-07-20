
#include <lua.h>
#include <lauxlib.h>

int teste_lua(lua_State* L) {
   const char* entrada = luaL_checkstring(L, 1);
   long n = luaL_checkinteger(L, 2);
   printf("Recebi %s e %ld \n", entrada, n);
   lua_pushinteger(L, 42);
   return 1;
}

int main() {
   lua_State* L;

   L = luaL_newstate();
   luaL_openlibs(L);
   
   lua_register(L, "teste", teste_lua);

   lua_getglobal(L, "teste");
   lua_pushstring(L, "entrada");
   lua_pushinteger(L, 2);
   lua_call(L, 2, 1);
   long result = lua_tointeger(L, -1);
   lua_pop(L, 1);
   printf("The result is %ld \n", result);
   
   lua_close(L);
}
