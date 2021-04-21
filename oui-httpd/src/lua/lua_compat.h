#ifndef _LUA_COMPAT_H
#define _LUA_COMPAT_H

#include <lua.h>

#if LUA_VERSION_NUM <= 501
/* NOTE: this only works if nups == 0! */
#define luaL_setfuncs(L, fns, nups) luaL_register((L), NULL, (fns))

#define luaL_newlibtable(L,l) lua_createtable(L, 0, sizeof(l)/sizeof((l)[0]) - 1)

#define luaL_newlib(L,l) (luaL_newlibtable(L,l), luaL_setfuncs(L,l,0))
#endif

#endif