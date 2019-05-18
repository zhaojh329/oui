cmake_minimum_required(VERSION 2.6)

project(vwrt-rpc-plugin C)

add_definitions(-Os -Wall -Werror --std=gnu99 -g3 -Wmissing-declarations)

find_library(crypt NAMES crypt)
IF(crypt STREQUAL "LIBS-NOTFOUND")
  set(crypt "")
ENDIF()

add_library(vwrt MODULE vwrt.c)
target_link_libraries(vwrt ubox ubus ${crypt})
set_target_properties(vwrt PROPERTIES OUTPUT_NAME vwrt PREFIX "")

add_library(bwmon MODULE bwmon.c)
target_link_libraries(bwmon ubox ubus ${crypt})
set_target_properties(bwmon PROPERTIES OUTPUT_NAME bwmon-vwrt PREFIX "")

install(TARGETS vwrt bwmon LIBRARY DESTINATION lib)
