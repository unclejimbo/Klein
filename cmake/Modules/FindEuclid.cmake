# - Try to find euclid
#
# Once done this will define
#
#  EUCLID_FOUND
#  Euclid_INCLUDE_DIRS

set(Euclid_INCLUDE_SEARCH_PATHS
  /usr/include
  /usr/local/include
  $ENV{Euclid_HOME}/include
)

find_path(Euclid_INCLUDE_DIRS Euclid/Polyhedron_3.h PATHS ${Euclid_INCLUDE_SEARCH_PATHS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Euclid DEFAULT_MSG Euclid_INCLUDE_DIRS)

if(EUCLID_FOUND)
  mark_as_advanced(EUCLID_INCLUD_DIRS)
endif(EUCLID_FOUND)
