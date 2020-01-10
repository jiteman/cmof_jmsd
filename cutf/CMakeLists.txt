cmake_minimum_required( VERSION 3.7.1 )

project( tpc_google_test C CXX )


JMSD_SHOW_PROJECT_HEADER()


include( ${JMSD_COMPONENT_CMAKE_SETTINGS_PATH}/tpc-google-test-set-compiler-settings.cmake )
set( JMSD_THIS_PROJECT_HEADER_ROOT ${JMSD_COMPONENT_SOURCE_ROOT_PATH}/googletest/include )
set( JMSD_THIS_PROJECT_SOURCE_ROOT ${JMSD_COMPONENT_SOURCE_ROOT_PATH}/googletest/src )

## include dependencies
set( ${PROJECT_NAME}_DEPENDENCY_DIRS_VAR
	${JMSD_PLATFORM_SOURCES}
	${JMSD_THIS_PROJECT_HEADER_ROOT}
	${JMSD_COMPONENT_SOURCE_ROOT_PATH}/googletest )
list( REMOVE_DUPLICATES ${PROJECT_NAME}_DEPENDENCY_DIRS_VAR )
include_directories( ${${PROJECT_NAME}_DEPENDENCY_DIRS_VAR} ${JMSD_THIS_PROJECT_SOURCE_ROOT} )


## this project headers and sources enumeration section
file( GLOB_RECURSE header_and_source_files
	${JMSD_THIS_PROJECT_HEADER_ROOT}/*.h*
	${JMSD_THIS_PROJECT_HEADER_ROOT}/*.c*
	${JMSD_THIS_PROJECT_SOURCE_ROOT}/*.h*
	${JMSD_THIS_PROJECT_SOURCE_ROOT}/*.c* )

set( JMSD_THIS_PROJECT_FILES_TO_REMOVE
#	"gtest-param-test.h.pump"
#	"gtest-param-util-generated.h.pump"
	"gtest-type-util.h.pump"
	"gtest-all.cc"
	"gtest_main.cc" )
JMSD_REMOVE_FILES_FROM_THE_LIST( header_and_source_files JMSD_THIS_PROJECT_FILES_TO_REMOVE )

add_library( ${PROJECT_NAME} SHARED ${header_and_source_files} )


## definition section
set( ${PROJECT_NAME}_BUILD_DEFINITIONS_VAR
	-DGTEST_CREATE_SHARED_LIBRARY )
JMSD_LIST_TO_STRING( ${PROJECT_NAME}_BUILD_DEFINITIONS_VAR )
JMSD_STRING_REMOVE_DUPLICATES( ${PROJECT_NAME}_BUILD_DEFINITIONS_VAR )
set( ${PROJECT_NAME}_LINK_DEFINITIONS_VAR
	-DGTEST_LINKED_AS_SHARED_LIBRARY )
JMSD_LIST_TO_STRING( ${PROJECT_NAME}_LINK_DEFINITIONS_VAR )
JMSD_STRING_REMOVE_DUPLICATES( ${PROJECT_NAME}_LINK_DEFINITIONS_VAR )
add_definitions( "${${PROJECT_NAME}_BUILD_DEFINITIONS_VAR}" )


## required to be able to be linked into shared libraries.
set_target_properties( ${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON )
set_target_properties( ${PROJECT_NAME} PROPERTIES VERSION 0.1.0 )
set_target_properties( ${PROJECT_NAME} PROPERTIES SOVERSION 0.1 )


## project target section
if ( UNIX )
	set( ${PROJECT_NAME}_DEPENDENCY_LIBS_VAR
		"pthread" )
else()
	set( ${PROJECT_NAME}_DEPENDENCY_LIBS_VAR
		"" )
endif()

list( REMOVE_DUPLICATES ${PROJECT_NAME}_DEPENDENCY_LIBS_VAR )
target_link_libraries( ${PROJECT_NAME} ${${PROJECT_NAME}_DEPENDENCY_LIBS_VAR} )


## Expose public includes to other subprojects through cache variable.
include( ${JMSD_CMAKE_SETTINGS_PATH}/set-expose-dependencies.cmake )
JMSD_SHOW_PROJECT_FOOTER()
