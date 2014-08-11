CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# find all source and header files in build directory
file(GLOB SRCS_CPP "*.cpp")
file(GLOB SRCS_H "*.h")

# Add Pleora
add_definitions(-D_UNIX_)
EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )
message( STATUS "Architecture: ${ARCHITECTURE}" )
if( ${ARCHITECTURE} STREQUAL "64Bit" )
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
	INCLUDE_DIRECTORIES(/opt/pleora/ebus_sdk/RHEL-i686/include  ${pleora_INCLUDE_DIRS})
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


# ThermocamGSP API
add_library(ThermocamGSP_API STATIC ${SRCS_H} ${SRCS_CPP})
SET_TARGET_PROPERTIES(ThermocamGSP_API PROPERTIES LINKER_LANGUAGE CXX)
LINK_LIBRARIES(pthread)
LINK_LIBRARIES(${PLEORA_LIB}})
