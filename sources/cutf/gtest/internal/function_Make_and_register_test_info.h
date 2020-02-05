#pragma once

#include "function_Make_and_register_test_info.hxx"


#include "gtest/Test_info.hxx"
#include "gtest-internal.hxx"


#include "cutf.h"


namespace jmsd {
namespace cutf {
namespace internal {


class JMSD_CUTF_SHARED_INTERFACE function_Make_and_register_test_info {

public:
	// Creates a new TestInfo object and registers it with Google Test;
	// returns the created object.
	//
	// Arguments:
	//
	//   test_suite_name:   name of the test suite
	//   name:             name of the test
	//   type_param        the name of the test's type parameter, or NULL if
	//                     this is not a typed or a type-parameterized test.
	//   value_param       text representation of the test's value parameter,
	//                     or NULL if this is not a type-parameterized test.
	//   code_location:    code location where the test is defined
	//   fixture_class_id: ID of the test fixture class
	//   set_up_tc:        pointer to the function that sets up the test suite
	//   tear_down_tc:     pointer to the function that tears down the test suite
	//   factory:          pointer to the factory that creates a test object.
	//                     The newly created TestInfo instance will assume
	//                     ownership of the factory object.
	static TestInfo *MakeAndRegisterTestInfo(
		const char* test_suite_name,
		const char* name,
		const char* type_param,
		const char* value_param,
		::testing::internal::CodeLocation code_location,
		::testing::internal::TypeId fixture_class_id,
		::testing::internal::SetUpTestSuiteFunc set_up_tc,
		::testing::internal::TearDownTestSuiteFunc tear_down_tc,
		::testing::internal::TestFactoryBase *factory );

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
public:

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
private:
	virtual ~function_Make_and_register_test_info() noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	function_Make_and_register_test_info() noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	function_Make_and_register_test_info( function_Make_and_register_test_info const &another ) noexcept = delete;
	function_Make_and_register_test_info &operator =( function_Make_and_register_test_info const &another ) noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	function_Make_and_register_test_info( function_Make_and_register_test_info &&another ) noexcept = delete;
	function_Make_and_register_test_info &operator =( function_Make_and_register_test_info &&another ) noexcept = delete;

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
