/*
 * MIT License
 *
 * Copyright (c) 2020 Jianhui Zhao <zhaojh329@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <math.h>

#include "lua2json.h"

static int lua_test_array(lua_State *L)
{
    lua_Number idx;
    int max = 0;

    lua_pushnil(L);

    /* check for non-integer keys */
    while (lua_next(L, -2)) {
        if (lua_type(L, -2) != LUA_TNUMBER)
            goto out;
        idx = lua_tonumber(L, -2);
        if (floor(idx) != idx || idx <= 0)
            goto out;

        if (idx > max)
            max = idx;

        lua_pop(L, 1);
        continue;

out:
        lua_pop(L, 2);
        return -1;
    }

    return max;
}

static void encode(lua_State *L, json_t *ret, bool encode_empty_table_as_array);

static json_t *append_data(lua_State *L, bool encode_empty_table_as_array)
{
    lua_Number nd, ni;
    int array_size;

    switch (lua_type(L, -1)) {
    case LUA_TNUMBER:
        nd = lua_tonumber(L, -1);
        ni = lua_tointeger(L, -1);

        if (nd == ni)
            return json_integer(lua_tointeger(L, -1));
        else
            return json_real(lua_tonumber(L, -1));
    case LUA_TSTRING:
        return json_string(lua_tostring(L, -1));
    case LUA_TBOOLEAN:
        return json_boolean(lua_toboolean(L, -1));
    case LUA_TNIL:
        return json_null();
    case LUA_TTABLE:
        array_size = lua_test_array(L);

        if (array_size > 0 || (array_size == 0 && encode_empty_table_as_array)) {
            json_t *a = json_array();
            int i;

            for (i = 1; i <= array_size; i++) {
                lua_rawgeti(L, -1, i);
                json_array_append_new(a, append_data(L, encode_empty_table_as_array));
                lua_pop(L, 1);
            }
            return a;
        } else {
            json_t *o = json_object();
            encode(L, o, encode_empty_table_as_array);
            return o;
        }
    default:
        luaL_error(L, "type not supported");
        return NULL;
    }
}

static void encode(lua_State *L, json_t *ret, bool encode_empty_table_as_array)
{
    lua_pushnil(L);

    while (lua_next(L, -2) != 0) {
        const char *key;

        switch (lua_type(L, -2)) {
        case LUA_TSTRING:
            key = lua_tostring(L, -2);
            break;
        case LUA_TNUMBER:
            lua_pushvalue(L, -2);
            key = lua_tostring(L, -1);
            lua_pop(L, 1);
            break;
        default:
            luaL_error(L, "type not supported");
            return;
        }

        json_object_set_new(ret, key, append_data(L, encode_empty_table_as_array));

        lua_pop(L, 1);
    }
}

json_t *lua_to_json(lua_State *L, int i, bool encode_empty_table_as_array)
{
    json_t *ret;

    lua_pushvalue(L, i);

    if (lua_istable(L, -1)) {
        int array_size = lua_test_array(L);

        if (array_size > 0 || (array_size == 0 && encode_empty_table_as_array)) {
            ret = json_array();

            for (i = 1; i <= array_size; i++) {
                lua_rawgeti(L, -1, i);
                json_array_append_new(ret, append_data(L, encode_empty_table_as_array));
                lua_pop(L, 1);
            }
        } else {
            ret = json_object();
            encode(L, ret, encode_empty_table_as_array);
        }
    } else {
        ret = json_object();
    }

    lua_pop(L, 1);

    return ret;
}

static void decode_append(lua_State *L, json_t *val)
{
    json_t *aval;
    size_t i;

    switch (json_typeof(val)) {
    case JSON_INTEGER:
        lua_pushinteger(L, json_integer_value(val));
        break;
    case JSON_REAL:
        lua_pushnumber(L, json_number_value(val));
        break;
    case JSON_STRING:
        lua_pushstring(L, json_string_value(val));
        break;
    case JSON_TRUE:
    case JSON_FALSE:
        lua_pushboolean(L, json_boolean_value(val));
        break;
    case JSON_NULL:
        lua_pushnil(L);
    case JSON_OBJECT:
        json_to_lua(val, L);
        break;
    case JSON_ARRAY:
        lua_newtable(L);
        json_array_foreach(val, i, aval) {
            decode_append(L, aval);
            lua_rawseti(L, -2, i + 1);
        }
        break;
    }
}

void json_to_lua(json_t *root, lua_State *L)
{
    const char *key;
    json_t *val;

    lua_newtable(L);

    json_object_foreach(root, key, val) {
        decode_append(L, val);
        lua_setfield(L, -2, key);
    }
}
