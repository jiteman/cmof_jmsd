cmake_minimum_required( VERSION 3.7.1 )

project( cmof_cutf )


JMSD_SHOW_PROJECT_GROUP_HEADER()


set( JMSD_COMPONENT_SOURCE_ROOT_PATH ${PROJECT_SOURCE_DIR}/sources )
set( JMSD_COMPONENT_CMAKE_SETTINGS_PATH ${PROJECT_SOURCE_DIR}/_cmake_settings )


add_subdirectory( cutf ) # C++ unit-testing framework
add_subdirectory( cmof ) # C++ mocking framework
add_subdirectory( ctf_lib ) # C++ testing framework
add_subdirectory( ctf_test_lib ) # C++ testing framework tests
add_subdirectory( ctf_tests ) # C++ testing framework test launcher


JMSD_SHOW_PROJECT_GROUP_FOOTER()
