#pragma once

#include "function_Print_test_part_result_to_string.hxx"


#include "gtest/gtest-test-part.h"


#include <string>


namespace jmsd {
namespace cutf {
namespace internal {


class function_Print_test_part_result_to_string {

public:
	static ::std::string PrintTestPartResultToString( ::testing::TestPartResult const &test_part_result );

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
public:

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
private:
	virtual ~function_Print_test_part_result_to_string() noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	function_Print_test_part_result_to_string() noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	function_Print_test_part_result_to_string( function_Print_test_part_result_to_string const &another ) noexcept = delete;
	function_Print_test_part_result_to_string &operator =( function_Print_test_part_result_to_string const &another ) noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	function_Print_test_part_result_to_string( function_Print_test_part_result_to_string &&another ) noexcept = delete;
	function_Print_test_part_result_to_string &operator =( function_Print_test_part_result_to_string &&another ) noexcept = delete;

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
private:

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:

};


} // namespace internal
} // namespace cutf
} // namespace jmsd

