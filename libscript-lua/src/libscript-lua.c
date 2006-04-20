
#include <libscript-plugin.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "libscript-lua.h"

static script_env* script_lua_get_env(lua_State *L) {
   script_env* env;
   lua_pushlightuserdata(L, L);
   lua_gettable(L, LUA_REGISTRYINDEX);
   env = (script_env*) lua_touserdata(L, -1);
   lua_pop(L, 1);
   return env;
}

static int script_lua_call(lua_State *L) {
   script_env* env = script_lua_get_env(L);
   script_fn fn;
   int nargs;
   int i;
   script_start_params(env);
   nargs = lua_gettop(L);
   for (i = 1; i <= nargs; i++) {
      switch(lua_type(L, i)) {
      case LUA_TNUMBER: script_out_double(env, lua_tonumber(L, i)); break;
      case LUA_TSTRING: script_out_string(env, lua_tostring(L, i)); break; /* TODO: zero-term */
      /* TODO: other types */
      }
   }
   fn = (script_fn) lua_touserdata(L, lua_upvalueindex(1));
   script_reset_outs(env);
   fn(env);
   /* TODO: return values */
   return 0;
}

static int script_lua_find_function(lua_State *L) {
   script_env* env = script_lua_get_env(L);
   const char* name;
   script_fn fn;

   name = lua_tostring(L, 2);

   fn = script_get_function(env, name);
   if (!fn) {
      lua_settop(L, 0);
      lua_pushnil(L);
      return 1;
   }
   lua_pushlightuserdata(L, fn);
   lua_pushcclosure(L, script_lua_call, 1);
   lua_pushvalue(L, 1);
   lua_pushvalue(L, 2);
   lua_pushvalue(L, -3);
   lua_settable(L, -3);
   lua_pop(L, 1);

   return 1;
}

script_plugin_state script_plugin_init_lua(script_env* env) {
   lua_State* L;
   const char* namespace;
   
   namespace = script_get_namespace(env);
   L = luaL_newstate();
   if (!L)
      return NULL;
   luaL_openlibs(L);

   /* Store handle to env */
   lua_pushlightuserdata(L, L);
   lua_pushlightuserdata(L, env);
   lua_settable(L, LUA_REGISTRYINDEX);
   
   lua_newtable(L);
   lua_newtable(L);
   lua_pushstring(L, "__index");
   lua_pushcfunction(L, script_lua_find_function);
   lua_settable(L, -3);
   lua_setmetatable(L, -2);
   lua_setglobal(L, namespace);
  
   return (script_plugin_state) L;
}

int script_plugin_run_lua(script_plugin_state state, char* programtext) {
   lua_State* L = (lua_State*) state;
   int err;

   err = luaL_loadstring(L, programtext);
   if (err)
      return SCRIPT_ERRLANGCOMP;
   err = lua_pcall(L, 0, 0, 0);
   if (err) {
      script_env* env = script_lua_get_env(L);
      script_set_error_message(env, lua_tostring(L, -1));
      return SCRIPT_ERRLANGRUN;
   }
   return SCRIPT_OK;
}

script_err script_plugin_call_lua(script_plugin_state state, char* fn) {
   lua_State* L = (lua_State*) state;
   script_env* env;
   const char* namespace;
   script_type type;
   int args;
   int err;

   lua_pushlightuserdata(L, L);
   lua_gettable(L, LUA_REGISTRYINDEX);
   env = (script_env*) lua_touserdata(L, -1);
   namespace = script_get_namespace(env);

   lua_getfield(L, LUA_GLOBALSINDEX, namespace);
   lua_pushstring(L, fn);
   lua_gettable(L, -2);
   if (!lua_isfunction(L, -1))
      return SCRIPT_ERRFNUNDEF;
   args = 0;
   while ( (type = script_in_type(env)) != SCRIPT_NONE ) {
      args++;
      switch (type) {
      case SCRIPT_DOUBLE: lua_pushnumber(L, script_in_double(env)); break;
      case SCRIPT_STRING: lua_pushstring(L, script_in_string(env)); break;
      case SCRIPT_NONE: /* pacify gcc warning */ break;
      }
   }
   /* TODO: returns */
   err = lua_pcall(L, args, 0, 0);
   if (err) {
      script_set_error_message(env, lua_tostring(L, -1));
      return SCRIPT_ERRLANGRUN;
   }
   return SCRIPT_OK;
}

void script_plugin_done_lua(script_plugin_state state) {
   lua_State* L = (lua_State*) state;

   lua_close(L);   
}
