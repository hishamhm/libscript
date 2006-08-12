
#include <libscript-plugin.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <stdlib.h>

#include "libscript-lua.h"

INLINE static script_env* script_lua_get_env(lua_State *L) {
   script_env* env;
   lua_pushlightuserdata(L, L);
   lua_gettable(L, LUA_REGISTRYINDEX);
   env = (script_env*) lua_touserdata(L, -1);
   lua_pop(L, 1);
   return env;
}

INLINE static void script_lua_stack_to_buffer(script_env* env, lua_State *L) {
   int nargs;
   int i;
   nargs = lua_gettop(L);
   script_reset_buffer(env);
   for (i = 1; i <= nargs; i++) {
      switch(lua_type(L, i)) {
      case LUA_TNUMBER: script_put_double(env, i-1, lua_tonumber(L, i)); break;
      case LUA_TSTRING: script_put_string(env, i-1, lua_tostring(L, i)); break; /* TODO: zero-term */
      case LUA_TBOOLEAN: script_put_bool(env, i-1, lua_toboolean(L, i)); break;
      /* TODO: other types */
      }
   }
}

INLINE static int script_lua_buffer_to_stack(script_env* env, lua_State *L) {
   int i;
   int len = script_buffer_size(env);
   for (i = 0; i < len; i++) {
      script_type type = script_get_type(env, i);
      switch (type) {
      case SCRIPT_DOUBLE:
         lua_pushnumber(L, script_get_double(env, i));
         break;
      case SCRIPT_STRING: {
         char* s = script_get_string(env, i);
         lua_pushstring(L, s);
         free(s);
         break;
      }
      case SCRIPT_BOOL: lua_pushboolean(L, script_get_bool(env, i)); break;
      case SCRIPT_NONE: /* pacify gcc warning */ break;
      /* TODO: other types */
      }
   }
   return len;
}

static int script_lua_caller(lua_State *L) {
   script_env* env = script_lua_get_env(L);
   const char* name;
   script_err err;
   script_lua_stack_to_buffer(env, L);
   name = lua_tostring(L, lua_upvalueindex(1));
   err = script_call(env, name);
   if (err != SCRIPT_OK) {
      lua_pushstring(L, "No such function.");
      lua_error(L);
   }
   return script_lua_buffer_to_stack(env, L);
}

static int script_lua_make_caller(lua_State *L) {
   lua_pushcclosure(L, script_lua_caller, 1);
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
   
   namespace = script_namespace(env);
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
   lua_pushcfunction(L, script_lua_make_caller);
   lua_settable(L, -3);
   lua_setmetatable(L, -2);
   lua_setglobal(L, namespace);
  
   return (script_plugin_state) L;
}

script_err script_plugin_run_lua(script_plugin_state state, char* programtext) {
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
   int args;
   int err;

   env = script_lua_get_env(L);
   namespace = script_namespace(env);

   lua_getfield(L, LUA_GLOBALSINDEX, namespace);
   lua_pushstring(L, fn);
   lua_gettable(L, -2);
   if (!lua_isfunction(L, -1) || lua_iscfunction(L, -1))
      return SCRIPT_ERRFNUNDEF;
   args = script_lua_buffer_to_stack(env, L);
   err = lua_pcall(L, args, 0, 0);
   if (err) {
      script_set_error_message(env, lua_tostring(L, -1));
      return SCRIPT_ERRLANGRUN;
   }
   script_lua_stack_to_buffer(env, L);
   return SCRIPT_OK;
}

void script_plugin_done_lua(script_plugin_state state) {
   lua_State* L = (lua_State*) state;

   lua_close(L);   
}
