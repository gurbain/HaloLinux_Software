cmake_minimum_required(VERSION 2.6)



# find all source and header files in build directory
file(GLOB SRCS_CPP "*.cpp")
file(GLOB SRCS_H "*.h")


# HaloGSP_API
set(CMAKE_CPP_CREATE_STATIC_LIBRARY on)
add_library(HaloGSP_API STATIC ${SRCS_H} ${SRCS_CPP})
SET_TARGET_PROPERTIES(HaloGSP_API PROPERTIES LINKER_LANGUAGE CXX)
LINK_LIBRARIES(pthread)
