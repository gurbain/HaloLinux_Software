cmake_minimum_required (VERSION 2.6)


# use sse3 instruction set
SET(CMAKE_CXX_FLAGS "-msse3")


add_executable(TP_2305_Halo2DockingPortTest main.cpp testproject.cpp GSsupportingFiles/haralick_exterior_iter_nonlin.cpp GSsupportingFiles/MEKF.cpp)


TARGET_LINK_LIBRARIES(TP_2305_Halo2DockingPortTest ${OpenCV_LIBS} "/usr/lib/libueye_api.so")



