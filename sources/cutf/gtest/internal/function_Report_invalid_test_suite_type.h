#pragma once

#include "function_Report_invalid_test_suite_type.hxx"


#include "gtest-internal.hxx"


#include "cutf.h"


namespace jmsd {
namespace cutf {
namespace internal {


class JMSD_CUTF_SHARED_INTERFACE function_Report_invalid_test_suite_type {

public:
	// Outputs a message explaining invalid registration of different fixture class for the same test suite.
	// This may happen when TEST_P macro is used to define two tests with the same name but in different namespaces.
	static void ReportInvalidTestSuiteType( char const *test_suite_name, ::testing::internal::CodeLocation const &code_location );

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
public:

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
private:
	virtual ~function_Report_invalid_test_suite_type() noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	function_Report_invalid_test_suite_type() noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	function_Report_invalid_test_suite_type( function_Report_invalid_test_suite_type const &another ) noexcept = delete;
	function_Report_invalid_test_suite_type &operator =( function_Report_invalid_test_suite_type const &another ) noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	function_Report_invalid_test_suite_type( function_Report_invalid_test_suite_type &&another ) noexcept = delete;
	function_Report_invalid_test_suite_type &operator =( function_Report_invalid_test_suite_type &&another ) noexcept = delete;

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
