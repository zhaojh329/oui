cmake_minimum_required(VERSION 3.10)

project(oui-httpd C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules/")

include(FindPkgConfig)

find_package(Libev REQUIRED)
find_package(Libuhttpd 3.5 REQUIRED)
find_package(Lua51 REQUIRED)
find_package(SQLite3 REQUIRED)

pkg_search_module(JANSSON REQUIRED jansson)

add_definitions(-O -Wall -Werror --std=gnu99 -D_GNU_SOURCE)

include_directories(${LIBUHTTPD_INCLUDE_DIR} ${LIBEV_INCLUDE_DIR} ${LUA_INCLUDE_DIR} ${JANSSON_INCLUDE_DIRS} ${SQLite3_INCLUDE_DIRS})

add_executable(oui-httpd main.c rpc.c lua2json.c multipart_parser.c upload.c download.c session.c utils.c db.c)
target_link_libraries(oui-httpd ${LIBUHTTPD_LIBRARY} ${LIBEV_LIBRARY} ${LUA_LIBRARY} ${JANSSON_LIBRARIES} ${SQLite3_LIBRARIES} ubox uci m)

add_library(lua-json MODULE lua_json.c lua2json.c)
target_link_libraries(lua-json ${JANSSON_LIBRARIES} m)
set_target_properties(lua-json PROPERTIES OUTPUT_NAME json PREFIX "")

add_library(lua-utils MODULE lua_utils.c)
target_link_libraries(lua-utils ubox)
set_target_properties(lua-utils PROPERTIES OUTPUT_NAME utils PREFIX "")

if (NOT LUAPATH)
	execute_process(
		COMMAND  lua -e "for k in string.gmatch(package.cpath .. \";\", \"([^;]+)/..so;\") do if k:sub(1,1) == \"/\" then print(k) break end end"
		OUTPUT_VARIABLE LUAPATH
		RESULT_VARIABLE LUA_CHECK_RES
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
endif()

install(
	TARGETS oui-httpd
	RUNTIME DESTINATION bin
)

install(
	TARGETS lua-json
	LIBRARY DESTINATION ${LUAPATH}/oui
)

install(
	TARGETS lua-utils
	LIBRARY DESTINATION ${LUAPATH}/oui
)
