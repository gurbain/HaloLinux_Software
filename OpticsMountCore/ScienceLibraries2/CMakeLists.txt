# find all source and header files in build directory
set(CMAKE_CPP_CREATE_STATIC_LIBRARY on)

file(GLOB SRCS_CPP "*.cpp" "libelas-src/*.cpp")
file(GLOB DISAM_SRCS_CPP "dynamic-isam/*.cpp")

include_directories("libelas-src")
include_directories("dynamic-isam")


add_library(OpticsMountSciLib2 STATIC ${SRCS_CPP})
SET_TARGET_PROPERTIES(OpticsMountSciLib2 PROPERTIES LINKER_LANGUAGE CXX)

add_library(dynamicISAM STATIC ${DISAM_SRCS_CPP})
target_link_libraries(dynamicISAM "${CMAKE_SOURCE_DIR}/OpticsMountCore/ExternalLibraries/isam/lib/libisam.a" cholmod)
SET_TARGET_PROPERTIES(dynamicISAM PROPERTIES LINKER_LANGUAGE CXX)


