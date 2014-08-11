# find all source and header files in build directory
set(CMAKE_CPP_CREATE_STATIC_LIBRARY on)

file(GLOB SRCS_CPP "*.cpp" "libelas-src_DP/*.cpp")
file(GLOB DISAM_SRCS_CPP "dynamic-isam_DP/*.cpp")

include_directories("libelas-src_DP")
include_directories("dynamic-isam_DP")


add_library(DockingPortSciLib2 STATIC ${SRCS_CPP})
SET_TARGET_PROPERTIES(DockingPortSciLib2 PROPERTIES LINKER_LANGUAGE CXX)

add_library(dynamicISAM STATIC ${DISAM_SRCS_CPP})
target_link_libraries(dynamicISAM "${CMAKE_SOURCE_DIR}/DockingPortCore/ExternalLibraries_DP/isam_DP/lib/libisam.a" cholmod)
SET_TARGET_PROPERTIES(dynamicISAM PROPERTIES LINKER_LANGUAGE CXX)


