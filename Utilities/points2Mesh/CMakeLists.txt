cmake_minimum_required (VERSION 2.6)

#INCLUDE_DIRECTORIES( "/usr/include/pcl-1.6")
find_package(PCL 1.6 REQUIRED)
include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

add_executable(points2Mesh main.cpp ${PCL_FILES})

target_link_libraries(points2Mesh ${PCL_LIBRARIES})