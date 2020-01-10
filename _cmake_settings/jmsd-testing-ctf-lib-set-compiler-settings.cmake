set( JMSD_COMPONENT_BASE_NAME jmsd-testing )
set( JMSD_COMPONENT_LAST_NAME ctf-lib )


set( JMSD_COMPONENT_FULL_NAME "${JMSD_COMPONENT_BASE_NAME}-${JMSD_COMPONENT_LAST_NAME}" )


JMSD_CMAKE_CURRENT_FILE_IN( "${JMSD_COMPONENT_FULL_NAME}-set-compiler-settings.cmake" )


if ( UNIX )
	JMSD_SHOW_BUILD_MESSAGE( "${JMSD_COMPONENT_FULL_NAME} COMPILER SETTINGS: Linux" )
	include( ${JMSD_COMPONENT_CMAKE_SETTINGS_PATH}/compiler-settings/${JMSD_COMPONENT_BASE_NAME}-common-set-linux-compiler-settings.cmake )
	include( ${JMSD_COMPONENT_CMAKE_SETTINGS_PATH}/compiler-settings/${JMSD_COMPONENT_FULL_NAME}-set-linux-compiler-settings.cmake )
elseif( WIN32 )
	JMSD_SHOW_BUILD_MESSAGE( "${JMSD_COMPONENT_FULL_NAME} COMPILER SETTINGS: Windows" )
	include( ${JMSD_COMPONENT_CMAKE_SETTINGS_PATH}/compiler-settings/${JMSD_COMPONENT_BASE_NAME}-common-set-windows-compiler-settings.cmake )
	include( ${JMSD_COMPONENT_CMAKE_SETTINGS_PATH}/compiler-settings/${JMSD_COMPONENT_FULL_NAME}-set-windows-compiler-settings.cmake )
else()
	message( STATUS "[JMSD] ${JMSD_COMPONENT_FULL_NAME} COMPILER SETTINGS: Unsupported platform. Default settings are used." )
endif()


JMSD_CMAKE_CURRENT_FILE_OUT( "${JMSD_COMPONENT_FULL_NAME}-set-compiler-settings.cmake" )
