cmake_minimum_required (VERSION 2.6)


# use sse3 instruction set
SET(CMAKE_CXX_FLAGS "-msse3")


add_executable(TP_2304_HaloOpticsAndXSCamTest main.cpp testproject.cpp GSsupportingFiles/haralick_exterior_iter_nonlin.cpp GSsupportingFiles/MEKF.cpp)


TARGET_LINK_LIBRARIES(TP_2304_HaloOpticsAndXSCamTest ${OpenCV_LIBS} "/usr/lib/libueye_api.so")



