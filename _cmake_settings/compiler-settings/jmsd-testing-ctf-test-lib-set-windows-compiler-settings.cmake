JMSD_CMAKE_CURRENT_FILE_IN( "${JMSD_FOREIGN_COMPONENT_FULL_NAME}-windows-compiler-settings.cmake" )

if ( WIN32 )
	set( ${PROJECT_NAME}_CXX_FLAGS ${CMAKE_CXX_FLAGS} )

	## list( APPEND ${PROJECT_NAME}_CXX_FLAGS "/wd" ) #
	## list( APPEND ${PROJECT_NAME}_CXX_FLAGS "/wd
	## " ) #
	list( APPEND ${PROJECT_NAME}_CXX_FLAGS "/wd4242" ) # '': conversion from '' to '', possible loss of data
	list( APPEND ${PROJECT_NAME}_CXX_FLAGS "/wd4365" ) # '': conversion from '' to '', signed/unsigned mismatch
	list( APPEND ${PROJECT_NAME}_CXX_FLAGS "/wd4668" ) # '' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	list( APPEND ${PROJECT_NAME}_CXX_FLAGS "/wd4702" ) # unreachable code
	list( APPEND ${PROJECT_NAME}_CXX_FLAGS "/wd4800" ) # Implicit conversion from '' to bool. Possible information loss
	list( APPEND ${PROJECT_NAME}_CXX_FLAGS "/wd4996" ) # '': warning STL4015: The std::iterator class template (used as a base class to provide typedefs) is deprecated in C++17.
	list( APPEND ${PROJECT_NAME}_CXX_FLAGS "/wd5032" ) # detected #pragma warning(push) with no corresponding #pragma warning(pop)

	# MSVC 2019
	list( APPEND ${PROJECT_NAME}_CXX_FLAGS "/Wv:18" ) # warning C5056: operator '==': deprecated for array types; note: to simplify migration, consider the temporary use of /Wv:18 flag with the version of the compiler with which you used to build without warnings

	string( REPLACE ";" " " ${PROJECT_NAME}_CXX_FLAGS_STR "${${PROJECT_NAME}_CXX_FLAGS}" )

	## string( REPLACE "X" "" ${PROJECT_NAME}_CXX_FLAGS_STR "${${PROJECT_NAME}_CXX_FLAGS_STR}" ) #
	string( REPLACE "/Za" "" ${PROJECT_NAME}_CXX_FLAGS_STR "${${PROJECT_NAME}_CXX_FLAGS_STR}" ) # disable language extension (no)

	set( CMAKE_CXX_FLAGS ${${PROJECT_NAME}_CXX_FLAGS_STR} )
else()
	message( SEND_ERROR "[JMSD] ${JMSD_FOREIGN_COMPONENT_FULL_NAME} COMPILER SETTINGS: ${JMSD_FOREIGN_COMPONENT_FULL_NAME}-windows-compiler-settings.cmake is included while not on windows" )

endif()

JMSD_CMAKE_CURRENT_FILE_OUT( "${JMSD_FOREIGN_COMPONENT_FULL_NAME}-windows-compiler-settings.cmake" )
