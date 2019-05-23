cmake_minimum_required(VERSION 2.6)

project(oui-rpc-plugin C)

add_definitions(-Os -Wall -Werror --std=gnu99 -g3 -Wmissing-declarations)

find_library(crypt NAMES crypt)
IF(crypt STREQUAL "LIBS-NOTFOUND")
  set(crypt "")
ENDIF()

add_library(oui MODULE oui.c)
target_link_libraries(oui ubox ubus ${crypt})
set_target_properties(oui PROPERTIES OUTPUT_NAME oui PREFIX "")

add_library(bwmon MODULE bwmon.c)
target_link_libraries(bwmon ubox ubus ${crypt})
set_target_properties(bwmon PROPERTIES OUTPUT_NAME bwmon-oui PREFIX "")

install(TARGETS oui bwmon LIBRARY DESTINATION lib)
