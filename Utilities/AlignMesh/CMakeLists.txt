cmake_minimum_required (VERSION 2.6)

	find_package(PCL 1.6 REQUIRED)
	
	include_directories(${PCL_INCLUDE_DIRS})
	link_directories(${PCL_LIBRARY_DIRS})
	add_definitions(${PCL_DEFINITIONS})



add_executable(alignMesh main.cpp)

target_link_libraries(alignMesh ${PCL_LIBRARIES})




