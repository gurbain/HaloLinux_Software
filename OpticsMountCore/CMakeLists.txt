cmake_minimum_required(VERSION 2.6)



# find all source and header files in build directory
file(GLOB SRCS_CPP "*.cpp")
file(GLOB SRCS_H "*.h")


# OpticsMountGSP_API
set(CMAKE_CPP_CREATE_STATIC_LIBRARY on)
add_library(OpticsMountGSP_API STATIC ${SRCS_H} ${SRCS_CPP})
SET_TARGET_PROPERTIES(OpticsMountGSP_API PROPERTIES LINKER_LANGUAGE CXX)
LINK_LIBRARIES(pthread)

ADD_SUBDIRECTORY(ExternalLibraries/isam)
ADD_SUBDIRECTORY(ScienceLibraries)
ADD_SUBDIRECTORY(ScienceLibraries2)