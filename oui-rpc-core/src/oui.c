#include <stdio.h>
#include <string.h>
#include <lauxlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <arpa/inet.h>
#include <sys/statvfs.h>

#if LUA_VERSION_NUM == 501
/* Adapted from Lua 5.2 */
void luaL_setfuncs (lua_State *L, const luaL_Reg *l, int nup)
{
    luaL_checkstack(L, nup+1, "too many upvalues");
    for (; l->name != NULL; l++) {  /* fill the table with given functions */
        int i;
        lua_pushstring(L, l->name);
        for (i = 0; i < nup; i++)  /* copy upvalues to the top */
            lua_pushvalue(L, -(nup + 1));
        lua_pushcclosure(L, l->func, nup);  /* closure with those upvalues */
        lua_settable(L, -(nup + 3));
    }
    lua_pop(L, nup);  /* remove upvalues */
}
#endif

static int l_crypt(lua_State *L)
{
    const char *key = lua_tostring(L, 1);
    const char *salt = lua_tostring(L, 2);
    char *hash;

    if (!key || !salt) {
        lua_pushnil(L);
        return 1;
    }

    hash = crypt(key, salt);
    if (hash)
        lua_pushstring(L, hash);
    else
        lua_pushnil(L);

    return 1;
}

static int l_parse_route_addr(lua_State *L)
{
    const char *addr = lua_tostring(L, 1);
    const char *mask = lua_tostring(L, 2);
    char as[sizeof("255.255.255.255/32\0")];
	struct in_addr a;
	int bits;

    if (!addr) {
        lua_pushnil(L);
        return 1;
    }

	a.s_addr = strtoul(addr, NULL, 16);
	inet_ntop(AF_INET, &a, as, sizeof(as));

	if (mask) {
		for (a.s_addr = ntohl(strtoul(mask, NULL, 16)), bits = 0;
		     a.s_addr & 0x80000000;
		     a.s_addr <<= 1)
			bits++;

		sprintf(as + strlen(as), "/%u", bits);
	}

    lua_pushstring(L, as);
    return 1;
}

static int l_parse_route6_addr(lua_State *L)
{
    const char *addr = lua_tostring(L, 1);
    const char *mask = lua_tostring(L, 2);
    char as[INET6_ADDRSTRLEN + sizeof("/128")];
    struct in6_addr a;
    int i;

#define hex(x) \
	(((x) <= '9') ? ((x) - '0') : \
		(((x) <= 'F') ? ((x) - 'A' + 10) : \
			((x) - 'a' + 10)))

    if (!addr) {
        lua_pushnil(L);
        return 1;
    }


	for (i = 0; i < 16; i++, addr += 2)
		a.s6_addr[i] = (16 * hex(*addr)) + hex(*(addr + 1));

	inet_ntop(AF_INET6, &a, as, sizeof(as));

	if (mask)
		sprintf(as + strlen(as), "/%lu", strtoul(mask, NULL, 16));

	lua_pushstring(L, as);
    return 1;
}

static int l_statvfs(lua_State *L)
{
    const char *path = lua_tostring(L, 1);
    struct statvfs s;

    if (!path) {
        lua_pushnil(L);
        return 1;
    }

    if (statvfs(path, &s)) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushinteger(L, s.f_blocks * s.f_frsize);
    lua_pushinteger(L, s.f_bfree  * s.f_frsize);
    lua_pushinteger(L, (s.f_blocks - s.f_bfree) * s.f_frsize);

    return 3;
}

static int l_parse_flow(lua_State *L)
{
    const char *str = lua_tostring(L, 1);
    static uint64_t G = 1000 * 1000 * 1000;
    static uint64_t M = 1000 * 1000;
    static uint64_t K = 1000;
    uint64_t val = 0;
    int g, m, k;

    if (str)
        val = strtoull(str, NULL, 10);

    g = val / G;
    val %= G;

    m = val / M;
    val %= M;

    k = val / K;
    val %= K;

    lua_pushinteger(L, g);
    lua_pushinteger(L, m);
    lua_pushinteger(L, k);
    lua_pushinteger(L, val);

    return 4;
}

static int l_readdir(lua_State *L)
{
    const char *path = lua_tostring(L, 1);
    struct dirent *dirp;
    DIR *dir;
    int i = 1;

    lua_newtable(L);

    if (!path)
        return 1;

    dir = opendir(path);
    if (!dir)
        return 1;

    while ((dirp = readdir(dir))) {
        int type = dirp->d_type;

        if (dirp->d_name[0] == '.')
            continue;

        if (type !=  DT_REG && type != DT_DIR)
            continue;

        lua_newtable(L);

        lua_pushstring(L, dirp->d_name);
        lua_setfield(L, -2, "name");

        if (dirp->d_type == DT_DIR) {
            lua_pushboolean(L, 1);
            lua_setfield(L, -2, "dir");
        }

        lua_rawseti(L, -2, i++);
    }

    return 1;
}

static const struct luaL_Reg func[] =
{
    {"crypt", l_crypt},
    {"parse_route_addr", l_parse_route_addr},
    {"parse_route6_addr", l_parse_route6_addr},
    {"statvfs", l_statvfs},
    {"parse_flow", l_parse_flow},
    {"readdir", l_readdir},
    {NULL, NULL}
};

int luaopen_oui_c(lua_State *L)
{
    lua_newtable(L);
    luaL_setfuncs(L, func, 0);

    return 1;
}
