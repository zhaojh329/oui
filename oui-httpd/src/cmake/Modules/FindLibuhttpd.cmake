# - Try to find libuhttpd
# Once done this will define
#  LIBUHTTPD_FOUND          - System has libuhttpd
#  LIBUHTTPD_INCLUDE_DIR    - The libuhttpd include directories
#  LIBUHTTPD_LIBRARY        - The libraries needed to use libuhttpd

find_path(LIBUHTTPD_INCLUDE_DIR uhttpd)
find_library(LIBUHTTPD_LIBRARY uhttpd PATH_SUFFIXES lib64)

if(LIBUHTTPD_INCLUDE_DIR)
  file(STRINGS "${LIBUHTTPD_INCLUDE_DIR}/uhttpd/config.h"
      LIBUHTTPD_VERSION_MAJOR REGEX "^#define[ \t]+UHTTPD_VERSION_MAJOR[ \t]+[0-9]+")
  file(STRINGS "${LIBUHTTPD_INCLUDE_DIR}/uhttpd/config.h"
      LIBUHTTPD_VERSION_MINOR REGEX "^#define[ \t]+UHTTPD_VERSION_MINOR[ \t]+[0-9]+")
  string(REGEX REPLACE "[^0-9]+" "" LIBUHTTPD_VERSION_MAJOR "${LIBUHTTPD_VERSION_MAJOR}")
  string(REGEX REPLACE "[^0-9]+" "" LIBUHTTPD_VERSION_MINOR "${LIBUHTTPD_VERSION_MINOR}")
  set(LIBUHTTPD_VERSION "${LIBUHTTPD_VERSION_MAJOR}.${LIBUHTTPD_VERSION_MINOR}")
  unset(LIBUHTTPD_VERSION_MINOR)
  unset(LIBUHTTPD_VERSION_MAJOR)
endif()

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBUHTTPD_FOUND to TRUE
# if all listed variables are TRUE and the requested version matches.
find_package_handle_standard_args(Libuhttpd REQUIRED_VARS
                                  LIBUHTTPD_LIBRARY LIBUHTTPD_INCLUDE_DIR
                                  VERSION_VAR LIBUHTTPD_VERSION)

mark_as_advanced(LIBUHTTPD_INCLUDE_DIR LIBUHTTPD_LIBRARY)
