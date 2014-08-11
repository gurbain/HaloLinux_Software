cmake_minimum_required (VERSION 2.6)

set (LIBELAS_SRC_DIR libelas-src)

# include directory
include_directories("${LIBELAS_SRC_DIR}")
include_directories("/usr/include/eigen3")

# use sse3 instruction set
SET(CMAKE_CXX_FLAGS "-msse3")

# sources
FILE(GLOB LIBELAS_SRC_FILES "libelas-src/*.cpp")

include_directories("GSsupportingFiles")
file(GLOB SUPPORTING_FILES "GSsupportingFiles/*.cpp")

add_executable(TP_2301_HaloOpticsMountTest testproject.cpp ${LIBELAS_SRC_FILES} ${SUPPORTING_FILES})


TARGET_LINK_LIBRARIES(TP_2301_HaloOpticsMountTest ${OpenCV_LIBS} "/usr/lib/libueye_api.so")



