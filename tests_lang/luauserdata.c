
#include <lua.h>
#include <lauxlib.h>

int main() {
   lua_State* L;
   void* buffer;

   L = luaL_newstate();
   luaL_openlibs(L);
   
   buffer = lua_newuserdata(L, 100);
   lua_newtable(L);
   lua_setmetatable(L, -2);
   lua_setglobal(L, "buffer");
   
   luaL_dostring(L, "getmetatable(buffer).__gc = function(ud) "
                    "   print('destroying udata') "
                    "end "
                    "buffer = nil "
                    "collectgarbage('collect') "
   );
   
   lua_close(L);
}
