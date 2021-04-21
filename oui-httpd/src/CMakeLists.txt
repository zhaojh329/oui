cmake_minimum_required(VERSION 3.10)

project(oui-httpd C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules/")

include(FindPkgConfig)

find_package(Libev REQUIRED)
find_package(Libuhttpd 3.11 REQUIRED)
find_package(Lua51 REQUIRED)

pkg_search_module(JANSSON REQUIRED jansson)
pkg_search_module(SQLITE3 REQUIRED sqlite3)

add_definitions(-O -Wall -Werror --std=gnu99 -D_GNU_SOURCE)

include_directories(${LIBUHTTPD_INCLUDE_DIR} ${LIBEV_INCLUDE_DIR} ${LUA_INCLUDE_DIR} ${JANSSON_INCLUDE_DIRS} ${SQLITE3_INCLUDE_DIRS})

add_executable(oui-httpd main.c rpc.c lua2json.c multipart_parser.c upload.c download.c session.c utils.c db.c)
target_link_libraries(oui-httpd ${LIBUHTTPD_LIBRARY} ${LIBEV_LIBRARY} ${LUA_LIBRARY} ${JANSSON_LIBRARIES} ${SQLITE3_LIBRARIES} ubox uci m pthread)

install(
	TARGETS oui-httpd
	RUNTIME DESTINATION bin
)

add_subdirectory(lua)