/*
 * libuci plugin for Lua
 * Copyright (C) 2008 Felix Fietkau <nbd@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <sys/types.h>
#include <sys/time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include <lauxlib.h>
#include <uci.h>

#define MODNAME        "uci"
#define METANAME       MODNAME ".meta"
//#define DEBUG 1

#ifdef DEBUG
#define DPRINTF(...) fprintf(stderr, __VA_ARGS__)
#else
#define DPRINTF(...) do {} while (0)
#endif

static struct uci_context *global_ctx = NULL;

static struct uci_context *
find_context(lua_State *L, int *offset)
{
	struct uci_context **ctx;
	if (!lua_isuserdata(L, 1)) {
		if (!global_ctx) {
			global_ctx = uci_alloc_context();
			if (!global_ctx) {
				luaL_error(L, "failed to allocate UCI context");
				return NULL;
			}
		}
		if (offset)
			*offset = 0;
		return global_ctx;
	}
	if (offset)
		*offset = 1;
	ctx = luaL_checkudata(L, 1, METANAME);
	if (!ctx || !*ctx) {
		luaL_error(L, "failed to get UCI context");
		return NULL;
	}

	return *ctx;
}

static struct uci_package *
find_package(lua_State *L, struct uci_context *ctx, const char *str, bool al)
{
	struct uci_package *p = NULL;
	struct uci_element *e;
	char *sep;
	char *name;

	sep = strchr(str, '.');
	if (sep) {
		name = malloc(1 + sep - str);
		if (!name) {
			luaL_error(L, "out of memory");
			return NULL;
		}
		strncpy(name, str, sep - str);
		name[sep - str] = 0;
	} else
		name = (char *) str;

	uci_foreach_element(&ctx->root, e) {
		if (strcmp(e->name, name) != 0)
			continue;

		p = uci_to_package(e);
		goto done;
	}

	if (al)
		uci_load(ctx, name, &p);

done:
	if (name != str)
		free(name);
	return p;
}

static int
lookup_extended(struct uci_context *ctx, struct uci_ptr *ptr, char *str, bool extended)
{
	int rv;
	struct uci_ptr lookup;

	/* use a copy of the passed ptr since failing lookups will
	 * clobber the state */
	lookup = *ptr;
	lookup.flags |= UCI_LOOKUP_EXTENDED;

	rv = uci_lookup_ptr(ctx, &lookup, str, extended);

	/* copy to passed ptr on success */
	if (!rv)
		*ptr = lookup;

	return rv;
}

static int
lookup_ptr(struct uci_context *ctx, struct uci_ptr *ptr, char *str, bool extended)
{
	if (ptr && !ptr->s && ptr->section && *ptr->section == '@')
		return lookup_extended(ctx, ptr, str, extended);

	return uci_lookup_ptr(ctx, ptr, str, extended);
}

static int
lookup_args(lua_State *L, struct uci_context *ctx, int offset, struct uci_ptr *ptr, char **buf)
{
	char *s = NULL;
	int n;

	n = lua_gettop(L);
	luaL_checkstring(L, 1 + offset);
	s = strdup(lua_tostring(L, 1 + offset));
	if (!s)
		goto error;

	memset(ptr, 0, sizeof(struct uci_ptr));
	if (!find_package(L, ctx, s, true))
		goto error;

	switch (n - offset) {
	case 4:
	case 3:
		ptr->option = luaL_checkstring(L, 3 + offset);
		/* fall through */
	case 2:
		ptr->section = luaL_checkstring(L, 2 + offset);
		ptr->package = luaL_checkstring(L, 1 + offset);
		if (lookup_ptr(ctx, ptr, NULL, true) != UCI_OK)
			goto error;
		break;
	case 1:
		if (lookup_ptr(ctx, ptr, s, true) != UCI_OK)
			goto error;
		break;
	default:
		luaL_error(L, "invalid argument count");
		goto error;
	}

	*buf = s;
	return 0;

error:
	if (s)
		free(s);
	return 1;
}

static int
uci_push_status(lua_State *L, struct uci_context *ctx, bool hasarg)
{
	char *str = NULL;

	if (!hasarg)
		lua_pushboolean(L, (ctx->err == UCI_OK));
	if (ctx->err) {
		uci_get_errorstr(ctx, &str, MODNAME);
		if (str) {
			lua_pushstring(L, str);
			free(str);
			return 2;
		}
	}
	return 1;
}

static void
uci_push_option(lua_State *L, struct uci_option *o)
{
	struct uci_element *e;
	int i = 0;

	switch(o->type) {
	case UCI_TYPE_STRING:
		lua_pushstring(L, o->v.string);
		break;
	case UCI_TYPE_LIST:
		lua_newtable(L);
		uci_foreach_element(&o->v.list, e) {
			i++;
			lua_pushstring(L, e->name);
			lua_rawseti(L, -2, i);
		}
		break;
	default:
		lua_pushnil(L);
		break;
	}
}

static void
uci_push_section(lua_State *L, struct uci_section *s, int index)
{
	struct uci_element *e;

	lua_newtable(L);
	lua_pushboolean(L, s->anonymous);
	lua_setfield(L, -2, ".anonymous");
	lua_pushstring(L, s->type);
	lua_setfield(L, -2, ".type");
	lua_pushstring(L, s->e.name);
	lua_setfield(L, -2, ".name");
	if (index >= 0) {
		lua_pushinteger(L, index);
		lua_setfield(L, -2, ".index");
	}

	uci_foreach_element(&s->options, e) {
		struct uci_option *o = uci_to_option(e);
		uci_push_option(L, o);
		lua_setfield(L, -2, o->e.name);
	}
}

static void
uci_push_package(lua_State *L, struct uci_package *p)
{
	struct uci_element *e;
	int i = 0;

	lua_newtable(L);
	uci_foreach_element(&p->sections, e) {
		uci_push_section(L, uci_to_section(e), i);
		lua_setfield(L, -2, e->name);
		i++;
	}
}

static int
uci_lua_unload(lua_State *L)
{
	struct uci_context *ctx;
	struct uci_package *p;
	const char *s;
	int offset = 0;

	ctx = find_context(L, &offset);
	luaL_checkstring(L, 1 + offset);
	s = lua_tostring(L, 1 + offset);
	p = find_package(L, ctx, s, false);
	if (p) {
		uci_unload(ctx, p);
		return uci_push_status(L, ctx, false);
	} else {
		lua_pushboolean(L, 0);
	}
	return 1;
}

static int
uci_lua_load(lua_State *L)
{
	struct uci_context *ctx;
	struct uci_package *p = NULL;
	const char *s;
	int offset = 0;

	ctx = find_context(L, &offset);
	uci_lua_unload(L);
	lua_pop(L, 1); /* bool ret value of unload */
	s = lua_tostring(L, -1);

	uci_load(ctx, s, &p);
	return uci_push_status(L, ctx, false);
}


static int
uci_lua_foreach(lua_State *L)
{
	struct uci_context *ctx;
	struct uci_package *p;
	struct uci_element *e, *tmp;
	const char *package, *type;
	bool ret = false;
	int offset = 0;
	int i = 0;

	ctx = find_context(L, &offset);
	package = luaL_checkstring(L, 1 + offset);

	if (lua_isnil(L, 2 + offset))
		type = NULL;
	else
		type = luaL_checkstring(L, 2 + offset);

	if (!lua_isfunction(L, 3 + offset) || !package)
		return luaL_error(L, "Invalid argument");

	p = find_package(L, ctx, package, true);
	if (!p)
		goto done;

	uci_foreach_element_safe(&p->sections, tmp, e) {
		struct uci_section *s = uci_to_section(e);

		i++;

		if (type && (strcmp(s->type, type) != 0))
			continue;

		lua_pushvalue(L, 3 + offset); /* iterator function */
		uci_push_section(L, s, i - 1);
		if (lua_pcall(L, 1, 1, 0) == 0) {
			ret = true;
			if (lua_isboolean(L, -1) && !lua_toboolean(L, -1))
				break;
		}
		else
		{
			lua_error(L);
			break;
		}
	}

done:
	lua_pushboolean(L, ret);
	return 1;
}



static int
uci_lua_each_iter(lua_State *L)
{
	const char *type = lua_tostring(L, lua_upvalueindex(1));
	const const struct uci_list *list = lua_topointer(L, lua_upvalueindex(2));
	struct uci_element *e = (struct uci_element *)lua_topointer(L, lua_upvalueindex(3));
	struct uci_element *tmp = (struct uci_element *)lua_topointer(L, lua_upvalueindex(4));
	int i = lua_tointeger(L, lua_upvalueindex(5));
	struct uci_section *s;

again:
	if (i) {
		e = tmp;
		tmp = list_to_element(e->list.next);
	}

	if (&e->list == list)
		return 0;

	s = uci_to_section(e);

	i++;

	if (type && (strcmp(s->type, type) != 0))
		goto again;

	lua_pushlightuserdata(L, tmp);
	lua_replace(L, lua_upvalueindex(4));

	lua_pushlightuserdata(L, e);
	lua_replace(L, lua_upvalueindex(3));

	lua_pushinteger(L, i);
	lua_replace(L, lua_upvalueindex(5));

	uci_push_section(L, s, i - 1);

	return 1;
}

static int
uci_lua_each(lua_State *L)
{
	struct uci_context *ctx;
	struct uci_package *p;
	struct uci_element *e, *tmp;
	const char *package, *type;
	int offset = 0;
	int i = 0;

	ctx = find_context(L, &offset);
	package = luaL_checkstring(L, 1 + offset);

	if (lua_isnil(L, 2 + offset))
		type = NULL;
	else
		type = luaL_checkstring(L, 2 + offset);

	p = find_package(L, ctx, package, true);
	if (!p) {
		lua_pushnil(L);
		return 1;
	}

	e = list_to_element(p->sections.next);
	tmp = list_to_element(e->list.next);

	if (type)
		lua_pushstring(L, type);
	else
		lua_pushnil(L);

	lua_pushlightuserdata(L, &p->sections);
	lua_pushlightuserdata(L, tmp);
	lua_pushlightuserdata(L, e);
	lua_pushinteger(L, i);

	lua_pushcclosure(L, uci_lua_each_iter, 5);
	return 1;
}

static int
uci_lua_get_any(lua_State *L, bool all)
{
	struct uci_context *ctx;
	struct uci_ptr ptr;
	int offset = 0;
	int nret = 1;
	char *s = NULL;

	ctx = find_context(L, &offset);

	if (lookup_args(L, ctx, offset, &ptr, &s))
		goto error;

	if (!all && !ptr.s) {
		ctx->err = UCI_ERR_INVAL;
		goto error;
	}
	if (!(ptr.flags & UCI_LOOKUP_COMPLETE)) {
		ctx->err = UCI_ERR_NOTFOUND;
		goto error;
	}

	if (ptr.o) {
		uci_push_option(L, ptr.o);
	} else if (ptr.s) {
		if (all) {
			uci_push_section(L, ptr.s, -1);
		}
		else {
			lua_pushstring(L, ptr.s->type);
			lua_pushstring(L, ptr.s->e.name);
			nret++;
		}
	} else {
		uci_push_package(L, ptr.p);
	}

	if (s)
		free(s);
	return nret;

error:
	if (s)
		free(s);

	lua_pushnil(L);
	return uci_push_status(L, ctx, true);
}

static int
uci_lua_get(lua_State *L)
{
	return uci_lua_get_any(L, false);
}

static int
uci_lua_get_all(lua_State *L)
{
	return uci_lua_get_any(L, true);
}

static int
uci_lua_add(lua_State *L)
{
	struct uci_context *ctx;
	struct uci_section *s = NULL;
	struct uci_package *p;
	const char *package;
	const char *type;
	const char *name = NULL;
	int offset = 0;

	ctx = find_context(L, &offset);
	package = luaL_checkstring(L, 1 + offset);
	type = luaL_checkstring(L, 2 + offset);
	p = find_package(L, ctx, package, true);
	if (!p)
		goto fail;

	if (uci_add_section(ctx, p, type, &s) || !s)
		goto fail;

	name = s->e.name;
	lua_pushstring(L, name);
	return 1;

fail:
	lua_pushnil(L);
	return uci_push_status(L, ctx, true);
}

static int
uci_lua_delete(lua_State *L)
{
	struct uci_context *ctx;
	struct uci_ptr ptr;
	int offset = 0;
	char *s = NULL;

	ctx = find_context(L, &offset);

	if (lookup_args(L, ctx, offset, &ptr, &s))
		goto error;

	uci_delete(ctx, &ptr);

error:
	if (s)
		free(s);
	return uci_push_status(L, ctx, false);
}

static int
uci_lua_rename(lua_State *L)
{
	struct uci_context *ctx;
	struct uci_ptr ptr;
	int err = UCI_ERR_MEM;
	char *s = NULL;
	int nargs, offset = 0;

	ctx = find_context(L, &offset);
	nargs = lua_gettop(L);
	if (lookup_args(L, ctx, offset, &ptr, &s))
		goto error;

	switch(nargs - offset) {
	case 1:
		/* Format: uci.set("p.s.o=v") or uci.set("p.s=v") */
		break;
	case 4:
		/* Format: uci.set("p", "s", "o", "v") */
		ptr.value = luaL_checkstring(L, nargs);
		break;
	case 3:
		/* Format: uci.set("p", "s", "v") */
		ptr.value = ptr.option;
		ptr.option = NULL;
		break;
	default:
		ctx->err = UCI_ERR_INVAL;
		goto error;
	}

	err = lookup_ptr(ctx, &ptr, NULL, true);
	if (err)
		goto error;

	if (((ptr.s == NULL) && (ptr.option != NULL)) || (ptr.value == NULL)) {
		ctx->err = UCI_ERR_INVAL;
		goto error;
	}

	err = uci_rename(ctx, &ptr);
	if (err)
		goto error;

error:
	if (s)
		free(s);
	return uci_push_status(L, ctx, false);
}

static int
uci_lua_reorder(lua_State *L)
{
	struct uci_context *ctx;
	struct uci_ptr ptr;
	int err = UCI_ERR_MEM;
	char *s = NULL;
	int nargs, offset = 0;

	ctx = find_context(L, &offset);
	nargs = lua_gettop(L);
	if (lookup_args(L, ctx, offset, &ptr, &s))
		goto error;

	switch(nargs - offset) {
	case 1:
		/* Format: uci.set("p.s=v") or uci.set("p.s=v") */
		if (ptr.option) {
			ctx->err = UCI_ERR_INVAL;
			goto error;
		}
		break;
	case 3:
		/* Format: uci.set("p", "s", "v") */
		ptr.value = ptr.option;
		ptr.option = NULL;
		break;
	default:
		ctx->err = UCI_ERR_INVAL;
		goto error;
	}

	err = lookup_ptr(ctx, &ptr, NULL, true);
	if (err)
		goto error;

	if ((ptr.s == NULL) || (ptr.value == NULL)) {
		ctx->err = UCI_ERR_INVAL;
		goto error;
	}

	err = uci_reorder_section(ctx, ptr.s, strtoul(ptr.value, NULL, 10));
	if (err)
		goto error;

error:
	if (s)
		free(s);
	return uci_push_status(L, ctx, false);
}


static int
uci_lua_set(lua_State *L)
{
	struct uci_context *ctx;
	struct uci_ptr ptr;
	bool istable = false;
	int err = UCI_ERR_MEM;
	char *s = NULL;
	const char *v;
	unsigned int i;
	int nargs, offset = 0;

	ctx = find_context(L, &offset);
	nargs = lua_gettop(L);
	if (lookup_args(L, ctx, offset, &ptr, &s))
		goto error;

	switch(nargs - offset) {
	case 1:
		/* Format: uci.set("p.s.o=v") or uci.set("p.s=v") */
		break;
	case 4:
		/* Format: uci.set("p", "s", "o", "v") */
		if (lua_istable(L, nargs)) {
			if (lua_rawlen(L, nargs) < 1) {
				free(s);
				return luaL_error(L, "Cannot set an uci option to an empty table value");
			}
			lua_rawgeti(L, nargs, 1);
			ptr.value = luaL_checkstring(L, -1);
			lua_pop(L, 1);
			istable = true;
		} else {
			ptr.value = luaL_checkstring(L, nargs);
		}
		break;
	case 3:
		/* Format: uci.set("p", "s", "v") */
		ptr.value = ptr.option;
		ptr.option = NULL;
		break;
	default:
		ctx->err = UCI_ERR_INVAL;
		goto error;
	}

	err = lookup_ptr(ctx, &ptr, NULL, true);
	if (err)
		goto error;

	if (((ptr.s == NULL) && (ptr.option != NULL)) || (ptr.value == NULL)) {
		ctx->err = UCI_ERR_INVAL;
		goto error;
	}

	if (istable) {
		if (lua_rawlen(L, nargs) == 1) {
			i = 1;
			if (ptr.o) {
				v = ptr.value;
				ptr.value = NULL;
				err = uci_delete(ctx, &ptr);
				if (err)
					goto error;
				ptr.value = v;
			}
		} else {
			i = 2;
			err = uci_set(ctx, &ptr);
			if (err)
				goto error;
		}

		for (; i <= lua_rawlen(L, nargs); i++) {
			lua_rawgeti(L, nargs, i);
			ptr.value = luaL_checkstring(L, -1);
			err = uci_add_list(ctx, &ptr);
			lua_pop(L, 1);
			if (err)
				goto error;
		}
	} else {
		err = uci_set(ctx, &ptr);
		if (err)
			goto error;
	}


error:
	if (s)
		free(s);
	return uci_push_status(L, ctx, false);
}

enum pkg_cmd {
	CMD_SAVE,
	CMD_COMMIT,
	CMD_REVERT
};

static int
uci_lua_package_cmd(lua_State *L, enum pkg_cmd cmd)
{
	struct uci_context *ctx;
	struct uci_element *e, *tmp;
	struct uci_ptr ptr;
	char *s = NULL;
	int nargs, offset = 0;

	ctx = find_context(L, &offset);
	nargs = lua_gettop(L);
	if ((cmd != CMD_REVERT) && (nargs - offset > 1))
		goto err;

	if (lookup_args(L, ctx, offset, &ptr, &s))
		goto err;

	lookup_ptr(ctx, &ptr, NULL, true);

	uci_foreach_element_safe(&ctx->root, tmp, e) {
		struct uci_package *p = uci_to_package(e);

		if (ptr.p && (ptr.p != p))
			continue;

		ptr.p = p;
		switch(cmd) {
		case CMD_COMMIT:
			uci_commit(ctx, &p, false);
			break;
		case CMD_SAVE:
			uci_save(ctx, p);
			break;
		case CMD_REVERT:
			uci_revert(ctx, &ptr);
			break;
		}
	}

err:
	if (s)
		free(s);
	return uci_push_status(L, ctx, false);
}

static int
uci_lua_save(lua_State *L)
{
	return uci_lua_package_cmd(L, CMD_SAVE);
}

static int
uci_lua_commit(lua_State *L)
{
	return uci_lua_package_cmd(L, CMD_COMMIT);
}

static int
uci_lua_revert(lua_State *L)
{
	return uci_lua_package_cmd(L, CMD_REVERT);
}

static void
uci_lua_add_change(lua_State *L, struct uci_element *e)
{
	struct uci_delta *h;
	const char *name;
	const char *value;

	h = uci_to_delta(e);
	if (!h->section)
		return;

	lua_getfield(L, -1, h->section);
	if (lua_isnil(L, -1)) {
		lua_pop(L, 1);
		lua_newtable(L);
		lua_pushvalue(L, -1); /* copy for setfield */
		lua_setfield(L, -3, h->section);
	}

	name = h->e.name;
	value = h->value ? h->value : "";

	if (name) {
		lua_getfield(L, -1, name);

		/* this delta is a list add operation */
		if (h->cmd == UCI_CMD_LIST_ADD) {
			/* there seems to be no table yet */
			if (!lua_istable(L, -1)) {
				lua_newtable(L);

				/* if there is a value on the stack already, add */
				if (!lua_isnil(L, -2)) {
					lua_pushvalue(L, -2);
					lua_rawseti(L, -2, 1);
					lua_pushstring(L, value);
					lua_rawseti(L, -2, 2);

				/* this is the first table item */
				} else {
					lua_pushstring(L, value);
					lua_rawseti(L, -2, 1);
				}

				lua_setfield(L, -3, name);

			/* a table is on the top of the stack and this is a subsequent,
			 * list_add, append this value to table */
			} else {
				lua_pushstring(L, value);
				lua_rawseti(L, -2, lua_rawlen(L, -2) + 1);
			}

		/* non-list change, simply set/replace field */
		} else {
			lua_pushstring(L, value);
			lua_setfield(L, -3, name);
		}

		lua_pop(L, 1);
	} else {
		lua_pushstring(L, value);
		lua_setfield(L, -2, ".type");
	}

	lua_pop(L, 1);
}

static void
uci_lua_changes_pkg(lua_State *L, struct uci_context *ctx, const char *package)
{
	struct uci_package *p = NULL;
	struct uci_element *e;
	bool autoload = false;

	p = find_package(L, ctx, package, false);
	if (!p) {
		autoload = true;
		p = find_package(L, ctx, package, true);
		if (!p)
			return;
	}

	if (uci_list_empty(&p->delta) && uci_list_empty(&p->saved_delta))
		goto done;

	lua_newtable(L);
	uci_foreach_element(&p->saved_delta, e) {
		uci_lua_add_change(L, e);
	}
	uci_foreach_element(&p->delta, e) {
		uci_lua_add_change(L, e);
	}
	lua_setfield(L, -2, p->e.name);

done:
	if (autoload)
		uci_unload(ctx, p);
}

static int
uci_lua_changes(lua_State *L)
{
	struct uci_context *ctx;
	const char *package = NULL;
	char **config = NULL;
	int nargs;
	int i, offset = 0;

	ctx = find_context(L, &offset);
	nargs = lua_gettop(L);
	switch(nargs - offset) {
	case 1:
		package = luaL_checkstring(L, 1 + offset);
	case 0:
		break;
	default:
		return luaL_error(L, "invalid argument count");
	}

	lua_newtable(L);
	if (package) {
		uci_lua_changes_pkg(L, ctx, package);
		return 1;
	}

	if ((uci_list_configs(ctx, &config) != UCI_OK) || !config)
		return 1;

	for (i = 0; config[i] != NULL; i++) {
		uci_lua_changes_pkg(L, ctx, config[i]);
	}

	free(config);
	return 1;
}

static int
uci_lua_get_confdir(lua_State *L)
{
	struct uci_context *ctx = find_context(L, NULL);
	lua_pushstring(L, ctx->confdir);
	return 1;
}

static int
uci_lua_set_confdir(lua_State *L)
{
	struct uci_context *ctx;
	int offset = 0;

	ctx = find_context(L, &offset);
	luaL_checkstring(L, 1 + offset);
	uci_set_confdir(ctx, lua_tostring(L, -1));
	return uci_push_status(L, ctx, false);
}

static int
uci_lua_get_savedir(lua_State *L)
{
	struct uci_context *ctx = find_context(L, NULL);
	lua_pushstring(L, ctx->savedir);
	return 1;
}

static int
uci_lua_add_delta(lua_State *L)
{
	struct uci_context *ctx;
	int offset = 0;

	ctx = find_context(L, &offset);
	luaL_checkstring(L, 1 + offset);
	uci_add_delta_path(ctx, lua_tostring(L, -1));
	return uci_push_status(L, ctx, false);
}

static int
uci_lua_set_savedir(lua_State *L)
{
	struct uci_context *ctx;
	int offset = 0;

	ctx = find_context(L, &offset);
	luaL_checkstring(L, 1 + offset);
	uci_set_savedir(ctx, lua_tostring(L, -1));
	return uci_push_status(L, ctx, false);
}

static int
uci_lua_list_configs(lua_State *L)
{
	struct uci_context *ctx;
	char **configs = NULL;
	char **ptr;
	int i = 1;

	ctx = find_context(L, NULL);
	if ((uci_list_configs(ctx, &configs) != UCI_OK) || !configs)
		return uci_push_status(L, ctx, false);
	lua_newtable(L);
	for (ptr = configs; *ptr; ptr++) {
		lua_pushstring(L, *ptr);
		lua_rawseti(L, -2, i++);
	}
	free(configs);
	return 1;
}

static int
uci_lua_gc(lua_State *L)
{
	struct uci_context **ctx;

	if (!lua_isuserdata(L, 1)) {
		if (!global_ctx)
			return 0;
		ctx = &global_ctx;
	} else {
		ctx = luaL_checkudata(L, 1, METANAME);
		if (!*ctx)
			return 0;
	}
	uci_free_context(*ctx);
	*ctx = NULL;
	return 0;
}

static int
uci_lua_cursor(lua_State *L)
{
	struct uci_context **u;
	int argc = lua_gettop(L);

	u = lua_newuserdata(L, sizeof(struct uci_context *));
	luaL_getmetatable(L, METANAME);
	lua_setmetatable(L, -2);

	*u = uci_alloc_context();
	if (!*u)
		return luaL_error(L, "Cannot allocate UCI context");
	switch (argc) {
		case 2:
			if (lua_isstring(L, 2) &&
				(uci_set_savedir(*u, luaL_checkstring(L, 2)) != UCI_OK))
				return luaL_error(L, "Unable to set savedir");
			/* fall through */
		case 1:
			if (lua_isstring(L, 1) &&
				(uci_set_confdir(*u, luaL_checkstring(L, 1)) != UCI_OK))
				return luaL_error(L, "Unable to set confdir");
			break;
		default:
			break;
	}
	return 1;
}

static const luaL_Reg uci[] = {
	{ "__gc", uci_lua_gc },
	{ "close", uci_lua_gc },
	{ "cursor", uci_lua_cursor },
	{ "load", uci_lua_load },
	{ "unload", uci_lua_unload },
	{ "get", uci_lua_get },
	{ "get_all", uci_lua_get_all },
	{ "add", uci_lua_add },
	{ "set", uci_lua_set },
	{ "rename", uci_lua_rename },
	{ "save", uci_lua_save },
	{ "delete", uci_lua_delete },
	{ "commit", uci_lua_commit },
	{ "revert", uci_lua_revert },
	{ "reorder", uci_lua_reorder },
	{ "changes", uci_lua_changes },
	{ "foreach", uci_lua_foreach },
	{ "each", uci_lua_each },
	{ "add_history", uci_lua_add_delta },
	{ "add_delta", uci_lua_add_delta },
	{ "get_confdir", uci_lua_get_confdir },
	{ "set_confdir", uci_lua_set_confdir },
	{ "get_savedir", uci_lua_get_savedir },
	{ "set_savedir", uci_lua_set_savedir },
	{ "list_configs", uci_lua_list_configs },
	{ NULL, NULL },
};


int
luaopen_uci(lua_State *L)
{
	/* create metatable */
	luaL_newmetatable(L, METANAME);

	/* metatable.__index = metatable */
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	/* fill metatable */
	luaL_setfuncs(L, uci, 0);
	lua_pop(L, 1);

	/* create module */
	lua_newtable(L);
	lua_pushvalue(L, -1);
	luaL_setfuncs(L, uci, 0);
	lua_setglobal(L, MODNAME);

	return 1;
}
