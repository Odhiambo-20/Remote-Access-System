# FindTAO.cmake - Find TAO library
# Sets:
#   TAO_FOUND
#   TAO_INCLUDE_DIRS
#   TAO_LIBRARIES

find_path(TAO_INCLUDE_DIR
    NAMES tao/ORB.h
    PATHS /usr/include /usr/local/include
)

find_library(TAO_LIBRARY
    NAMES TAO
    PATHS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(TAO
    REQUIRED_VARS TAO_LIBRARY TAO_INCLUDE_DIR
)

if(TAO_FOUND)
    set(TAO_LIBRARIES ${TAO_LIBRARY})
    set(TAO_INCLUDE_DIRS ${TAO_INCLUDE_DIR})
endif()

mark_as_advanced(TAO_INCLUDE_DIR TAO_LIBRARY)
