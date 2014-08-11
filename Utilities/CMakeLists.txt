cmake_minimum_required (VERSION 2.6)

INCLUDE_DIRECTORIES( 
					 "VideoMonitor"
					 "VideoMonitorSingleCamera"
					 "SerialTest"
					 "SerialTest2"
					 "SerialSendTest"
					 "WriteTest"
					 #"points2Mesh"
					 #"AlignMesh"
                   )


ADD_SUBDIRECTORY(VideoMonitor)
ADD_SUBDIRECTORY(VideoMonitorSingleCamera)
ADD_SUBDIRECTORY(SerialTest)
ADD_SUBDIRECTORY(SerialTest2)
ADD_SUBDIRECTORY(SerialSendTest)
ADD_SUBDIRECTORY(WriteTest)
#ADD_SUBDIRECTORY(points2Mesh)
#ADD_SUBDIRECTORY(AlignMesh)
