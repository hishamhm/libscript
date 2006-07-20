
#include <lua.h>
#include <lauxlib.h>

int main() {
   lua_State* L;

   L = luaL_newstate();
   luaL_openlibs(L);
   luaL_dostring(L, "f = function(k) return k / 2 end "
                    "g = function(k) return k + 100 end "
                    "mt = { __concat = function(f, g) "
                    "                     return function(...) "
                    "                               return f(g(...)) "
                    "                            end "
                    "                  end, "
                    "       __index = function(f, op) "
                    "                    return function(...) "
                    "                              return f(op, ...) "
                    "                           end "
                    "                  end, "
                    "       __metatable = false "
                    "     } ");
   lua_getglobal(L, "f");
   lua_getglobal(L, "mt");
   lua_setmetatable(L, -2);
   luaL_dostring(L, "two_to_x = print .. f .. g .. math.pow[2]");
   luaL_dostring(L, "two_to_x(8)");
   
   luaL_dostring(L, "print(getmetatable(print))");
   
   lua_close(L);
}
