cmake_minimum_required( VERSION 3.7.1 )

project( ctf_test_lib C CXX )


JMSD_SHOW_PROJECT_HEADER()


include( ${JMSD_COMPONENT_CMAKE_SETTINGS_PATH}/jmsd-testing-ctf-test-lib-set-compiler-settings.cmake )
set( JMSD_THIS_PROJECT_SOURCE_ROOT ${JMSD_COMPONENT_SOURCE_ROOT_PATH}/ctf_test_lib )


## include dependencies
set( ${PROJECT_NAME}_DEPENDENCY_DIRS_VAR
	${JMSD_PLATFORM_SOURCES}
	${ctf_lib_DEPENDENCY_DIRS}
	${JMSD_THIS_PROJECT_SOURCE_ROOT} )
list( REMOVE_DUPLICATES ${PROJECT_NAME}_DEPENDENCY_DIRS_VAR )
include_directories( ${${PROJECT_NAME}_DEPENDENCY_DIRS_VAR} )


file( GLOB_RECURSE header_and_source_files
	${JMSD_THIS_PROJECT_SOURCE_ROOT}/*.h*
	${JMSD_THIS_PROJECT_SOURCE_ROOT}/*.c* )

set( JMSD_THIS_PROJECT_FILES_TO_REMOVE
	"gtest_all_test.cc"
	"gtest-unittest-api_test.cc"
	"gtest_assert_by_exception_test.cc"
	"gtest_environment_test.cc"
	"gtest_help_test_.cc"
	"gtest_list_output_unittest_.cc"
	"gtest_no_test_unittest.cc"
	"gtest_premature_exit_test.cc"
	"gtest_repeat_test.cc"
	"gtest_skip_in_environment_setup_test.cc"
	"gtest_stress_test.cc"
	"gtest_testbridge_test_.cc"
	"gtest_test_macro_stack_footprint_test.cc"
	"gtest_throw_on_failure_ex_test.cc"
	"gtest_xml_output_unittest_.cc"
	"googletest-break-on-failure-unittest_.cc"
	"googletest-catch-exceptions-test_.cc"
	"googletest-color-test_.cc"
	"googletest-death-test-test.cc"
	"googletest-env-var-test_.cc"
	"googletest-filter-unittest_.cc"
	"googletest-list-tests-unittest_.cc"
	"googletest-listener-test.cc"
	"googletest-output-test_.cc"
	"googletest-param-test-invalid-name1-test_.cc"
	"googletest-param-test-invalid-name2-test_.cc"
	"googletest-param-test-test.h"
	"googletest-param-test-test.cc"
	"googletest-param-test2-test.cc"
	"googletest-test2_test.cc"
	"googletest-shuffle-test_.cc"
	"googletest-throw-on-failure-test_.cc"
	"googletest-uninitialized-test_.cc"
	"googletest-death-test_ex_test.cc"
	"gmock_all_test.cc"
	"gmock_leak_test_.cc"
	"gmock_link_test.h"
	"gmock_link_test.cc"
	"gmock_link2_test.cc"
	"gmock-spec-builders_test.cc"
	"gmock_output_test_.cc"
	"gmock_stress_test.cc" )
JMSD_REMOVE_FILES_FROM_THE_LIST( header_and_source_files JMSD_THIS_PROJECT_FILES_TO_REMOVE )

add_library( ${PROJECT_NAME} SHARED ${header_and_source_files} )


## definition section
set( ${PROJECT_NAME}_BUILD_DEFINITIONS_VAR
	${ctf_lib_LINK_DEFINITIONS}
	-DJMSD_TPA_GTM_TEST_SHARED_INTERFACE_INTERNAL )
JMSD_LIST_TO_STRING( ${PROJECT_NAME}_BUILD_DEFINITIONS_VAR )
JMSD_STRING_REMOVE_DUPLICATES( ${PROJECT_NAME}_BUILD_DEFINITIONS_VAR )
set( ${PROJECT_NAME}_LINK_DEFINITIONS_VAR
	${ctf_lib_LINK_DEFINITIONS} )
JMSD_LIST_TO_STRING( ${PROJECT_NAME}_LINK_DEFINITIONS_VAR )
JMSD_STRING_REMOVE_DUPLICATES( ${PROJECT_NAME}_LINK_DEFINITIONS_VAR )
add_definitions( "${${PROJECT_NAME}_BUILD_DEFINITIONS_VAR}" )


## required to be able to be linked into shared libraries.
set_target_properties( ${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON )
set_target_properties( ${PROJECT_NAME} PROPERTIES VERSION 0.1.0 )
set_target_properties( ${PROJECT_NAME} PROPERTIES SOVERSION 0.1 )


## project target section
set( ${PROJECT_NAME}_DEPENDENCY_LIBS_VAR
	${ctf_lib_DEPENDENCY_LIBS}
	ctf_lib )
list( REMOVE_DUPLICATES ${PROJECT_NAME}_DEPENDENCY_LIBS_VAR )
target_link_libraries( ${PROJECT_NAME} ${${PROJECT_NAME}_DEPENDENCY_LIBS_VAR} )


## Expose public includes to other subprojects through cache variable.
include( ${JMSD_CMAKE_SETTINGS_PATH}/set-expose-dependencies.cmake )
JMSD_SHOW_PROJECT_FOOTER()
