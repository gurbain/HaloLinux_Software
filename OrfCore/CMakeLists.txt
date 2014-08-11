CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# find all source and header files in build directory
file(GLOB SRCS_CPP "*.cpp")
file(GLOB SRCS_H "*.h")

## Add MESA SR libaries
FIND_PATH( MESASR_INCLUDE_PATH libMesaSR.h
	/usr/include
	/usr/local/include
	/sw/include
	/opt/local/include
	DOC "The directory where libMesaSR.h resides")
FIND_LIBRARY( MESASR_LIBRARY
	NAMES mesasr
	PATHS
	/usr/lib64
	/usr/lib
	/usr/local/lib64
	/usr/local/lib
	/sw/lib
	/opt/local/lib
	DOC "The MESASR library")
INCLUDE_DIRECTORIES(${MESASR_INCLUDE_PATH})

# OrfGSP API
add_library(OrfGSP_API STATIC ${SRCS_H} ${SRCS_CPP})
TARGET_LINK_LIBRARIES(OrfGSP_API ${MESASR_LIBRARY})
SET_TARGET_PROPERTIES(OrfGSP_API PROPERTIES LINKER_LANGUAGE CXX)
LINK_LIBRARIES(pthread)
