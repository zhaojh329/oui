cmake_minimum_required(VERSION 2.6)

project(vuci-rpc-plugin C)

add_definitions(-Os -Wall -Werror --std=gnu99 -g3 -Wmissing-declarations)

find_library(crypt NAMES crypt)
IF(crypt STREQUAL "LIBS-NOTFOUND")
  set(crypt "")
ENDIF()

add_library(vuci MODULE vuci.c)
target_link_libraries(vuci ubox ubus ${crypt})
set_target_properties(vuci PROPERTIES OUTPUT_NAME vuci PREFIX "")

add_library(bwmon MODULE bwmon.c)
target_link_libraries(bwmon ubox ubus ${crypt})
set_target_properties(bwmon PROPERTIES OUTPUT_NAME bwmon-vuci PREFIX "")

install(TARGETS vuci bwmon LIBRARY DESTINATION lib)
