cmake_minimum_required (VERSION 2.6)


# use sse3 instruction set
SET(CMAKE_CXX_FLAGS "-msse3")


add_executable(TP_2600_ORF main.cpp testproject.cpp)


TARGET_LINK_LIBRARIES(TP_2600_ORF ${OpenCV_LIBS} "/usr/lib/libueye_api.so")



