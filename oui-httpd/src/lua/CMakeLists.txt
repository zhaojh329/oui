add_library(ljson MODULE ljson.c ../lua2json.c)
target_link_libraries(ljson ${JANSSON_LIBRARIES})
set_target_properties(ljson PROPERTIES OUTPUT_NAME json PREFIX "")

add_library(lutils MODULE lutils.c md5.c base64.c)
set_target_properties(lutils PROPERTIES OUTPUT_NAME utils PREFIX "")

add_library(lnetwork MODULE lnetwork.c)
set_target_properties(lnetwork PROPERTIES OUTPUT_NAME network PREFIX "")

if (NOT LUAPATH)
	execute_process(
		COMMAND  lua -e "for k in string.gmatch(package.cpath .. \";\", \"([^;]+)/..so;\") do if k:sub(1,1) == \"/\" then print(k) break end end"
		OUTPUT_VARIABLE LUAPATH
		RESULT_VARIABLE LUA_CHECK_RES
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
endif()

install(
	TARGETS ljson
	DESTINATION ${LUAPATH}/oui
)

install(
	TARGETS lutils
	DESTINATION ${LUAPATH}/oui/utils
)

install(
	FILES utils.lua
	DESTINATION ${LUAPATH}/oui
)

install(
	TARGETS lnetwork
	DESTINATION ${LUAPATH}/oui
)