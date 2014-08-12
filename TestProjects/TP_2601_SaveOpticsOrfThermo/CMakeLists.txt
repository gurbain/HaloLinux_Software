cmake_minimum_required (VERSION 2.6)


# use sse3 instruction set
SET(CMAKE_CXX_FLAGS "-msse3")

# Add Pleora
add_definitions(-D_UNIX_)
EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )
message( STATUS "Architecture: ${ARCHITECTURE}" )
if( ${ARCHITECTURE} STREQUAL "x86_64" )
	INCLUDE_DIRECTORIES(/opt/pleora/ebus_sdk/RHEL-6-x86_64/include  ${pleora_INCLUDE_DIRS})
	LINK_DIRECTORIES(/opt/pleora/ebus_sdk/RHEL-6-x86_64/lib)
	LINK_DIRECTORIES(/opt/pleora/ebus_sdk/RHEL-6-x86_64/lib/genicam/bin/Linux64_x64)
	set(	PLEORA_LIB
		PvBase
		PvDevice
		PvBuffer
		PvGUIUtils
		PvGUI
		PvPersistence
		PvGenICam
		PvStreamRaw
		PvStream
		PvTransmitterRaw
		PvVirtualDevice
		EbNetworkLib
		PtUtilsLib
		PtConvertersLib
		PvSerial
		EbTransportLayerLib
		GCBase_gcc40_v2_3
		GenApi_gcc40_v2_3
		log4cpp_gcc40_v2_3
		Log_gcc40_v2_3
		MathParser_gcc40_v2_3
	)
else()
	INCLUDE_DIRECTORIES(/opt/pleora/ebus_sdk/RHEL-6-i686/include  ${pleora_INCLUDE_DIRS})
	LINK_DIRECTORIES(/opt/pleora/ebus_sdk/RHEL-6-i686/lib)
	LINK_DIRECTORIES(/opt/pleora/ebus_sdk/RHEL-6-i686/lib/genicam/bin/Linux32_i86)
	set(	PLEORA_LIB
		PvBase
		PvDevice
		PvBuffer
		PvGUIUtils
		PvGUI
		PvPersistence
		PvGenICam
		PvStreamRaw
		PvStream
		PvTransmitterRaw
		PvVirtualDevice
		EbNetworkLib
		PtUtilsLib
		PtConvertersLib
		PvSerial
		EbTransportLayerLib
		GCBase_gcc40_v2_3
		GenApi_gcc40_v2_3
		log4cpp_gcc40_v2_3
		Log_gcc40_v2_3
		MathParser_gcc40_v2_3
	)

endif()

add_executable(TP_2601_SaveOpticsOrfThermo main.cpp testproject.cpp)
TARGET_LINK_LIBRARIES(TP_2601_SaveOpticsOrfThermo ${OpenCV_LIBS} ${PLEORA_LIB} "/usr/lib/libueye_api.so")

