PROJECT(halo)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release ) # choose between Debug or Release
ENDIF(NOT CMAKE_BUILD_TYPE)

IF(CMAKE_BUILD_TYPE STREQUAL Release)
    SET(CMAKE_CXX_FLAGS "-O3 -mfpmath=sse -mmmx -msse -msse2 -msse3")        ## Optimize
    #SET(CMAKE_CXX_FLAGS "-O3 -march=native -mtune=native -mfpmath=sse -mmmx -msse -msse2 -msse3")        ## Optimize
    #SET(CMAKE_EXE_LINKER_FLAGS "-s")  ## Strip binary
ENDIF(CMAKE_BUILD_TYPE STREQUAL Release)

IF(CMAKE_BUILD_TYPE STREQUAL Debug)
    SET(CMAKE_CXX_FLAGS "-mfpmath=sse -mmmx -msse -msse2 -msse3")        ## Optimize
ENDIF(CMAKE_BUILD_TYPE STREQUAL Debug)


INCLUDE_DIRECTORIES( 
					 "./"
					 "TestProjects"
					 "HaloCore"
					 "HaloCore/GSP"
					 "OpticsMountCore/ScienceLibraries"
					 "OpticsMountCore/ScienceLibraries2"
					 "OpticsMountCore/ScienceLibraries2/libelas-src"
					 "OpticsMountCore/ScienceLibraries2/dynamic-isam"
					 "OpticsMountCore/ExternalLibraries/isam/include"
					 #"DockingPortCore/ScienceLibraries"
					 #"DockingPortCore/ScienceLibraries2"
					 #"DockingPortCore/ScienceLibraries2/libelas-src"
					 #"DockingPortCore/ScienceLibraries2/dynamic-isam"
					 #"DockingPortCore/ExternalLibraries/isam/include"
					 "Utilities"
				     "/usr/include/eigen3"
                   )

SET( EXECUTABLE_OUTPUT_PATH ../../bin )

find_package(OpenCV REQUIRED )
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

ADD_SUBDIRECTORY(HaloCore)
ADD_SUBDIRECTORY(OpticsMountCore)
ADD_SUBDIRECTORY(DockingPortCore)
ADD_SUBDIRECTORY(OrfCore)
ADD_SUBDIRECTORY(ThermoCamCore)

LINK_LIBRARIES(HaloGSP_API)
LINK_LIBRARIES(OpticsMountGSP_API)
LINK_LIBRARIES(OrfGSP_API)
LINK_LIBRARIES(ThermocamGSP_API)
#LINK_LIBRARIES(OpticsMountScienceLibraries_API)
#LINK_LIBRARIES(OpticsMountSciLib2)
LINK_LIBRARIES(DockingPortGSP_API)
#LINK_LIBRARIES(DockingPortScienceLibraries_API)
#LINK_LIBRARIES(DockingPortSciLib2)
#LINK_LIBRARIES(dynamicISAM)
LINK_LIBRARIES(pthread)
#LINK_LIBRARIES(OpenGL)
#LINK_LIBRARIES(GLUT)

ADD_SUBDIRECTORY(TestProjects)
ADD_SUBDIRECTORY(Utilities)

