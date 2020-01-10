JMSD_CMAKE_CURRENT_FILE_IN( "${JMSD_FOREING_COMPONENT_BASE_NAME}-common-set-windows-compiler-settings.cmake" )

if ( WIN32 )
	## set( ${PROJECT_NAME}_CXX_FLAGS ${CMAKE_CXX_FLAGS} )

	## list( APPEND ${PROJECT_NAME}_CXX_FLAGS "/wd" ) #

	## string( REPLACE ";" " " ${PROJECT_NAME}_CXX_FLAGS_STR "${${PROJECT_NAME}_CXX_FLAGS}" )

	## string( REPLACE "X" "" ${PROJECT_NAME}_CXX_FLAGS_STR "${${PROJECT_NAME}_CXX_FLAGS_STR}" ) #

	## set( CMAKE_CXX_FLAGS ${${PROJECT_NAME}_CXX_FLAGS_STR} )
else()
	message( SEND_ERROR "[JMSD] ${JMSD_FOREING_COMPONENT_BASE_NAME}-common COMPILER SETTINGS: ${JMSD_FOREING_COMPONENT_BASE_NAME}-common-set-windows-compiler-settings.cmake is included while not on windows" )

endif()

JMSD_CMAKE_CURRENT_FILE_OUT( "${JMSD_FOREING_COMPONENT_BASE_NAME}-common-set-windows-compiler-settings.cmake" )
