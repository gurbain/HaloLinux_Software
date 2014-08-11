cmake_minimum_required(VERSION 2.6)



# find all source and header files in build directory
file(GLOB SRCS_CPP "*.cpp")
file(GLOB SRCS_H "*.h")


# DockingPortGSP_API
set(CMAKE_CPP_CREATE_STATIC_LIBRARY on)
add_library(DockingPortGSP_API STATIC ${SRCS_H} ${SRCS_CPP})
SET_TARGET_PROPERTIES(DockingPortGSP_API PROPERTIES LINKER_LANGUAGE CXX)
TARGET_LINK_LIBRARIES(pthread)

#ADD_SUBDIRECTORY(ExternalLibraries_DP)
#ADD_SUBDIRECTORY(ScienceLibraries_DP)
#ADD_SUBDIRECTORY(ScienceLibraries2_DP)
