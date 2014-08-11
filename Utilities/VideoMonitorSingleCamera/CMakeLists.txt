cmake_minimum_required (VERSION 2.6)

add_executable(videoMonitorSingleCamera main.cpp networkServer.cpp videoDisplay.cpp)


TARGET_LINK_LIBRARIES(videoMonitorSingleCamera ${OpenCV_LIBS} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})



