# FindACE.cmake - Find ACE library
# Sets:
#   ACE_FOUND
#   ACE_INCLUDE_DIRS
#   ACE_LIBRARIES

find_path(ACE_INCLUDE_DIR
    NAMES ace/ACE.h
    PATHS /usr/include /usr/local/include
)

find_library(ACE_LIBRARY
    NAMES ACE
    PATHS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ACE
    REQUIRED_VARS ACE_LIBRARY ACE_INCLUDE_DIR
)

if(ACE_FOUND)
    set(ACE_LIBRARIES ${ACE_LIBRARY})
    set(ACE_INCLUDE_DIRS ${ACE_INCLUDE_DIR})
endif()

mark_as_advanced(ACE_INCLUDE_DIR ACE_LIBRARY)
