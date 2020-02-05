#include "function_Make_and_register_test_info.h"


#include "gtest/Test_info.h"

#include "Unit_test_impl.h"


namespace jmsd {
namespace cutf {
namespace internal {


// Creates a new TestInfo object and registers it with Google Test;
// returns the created object.
//
// Arguments:
//
//   test_suite_name:   name of the test suite
//   name:             name of the test
//   type_param:       the name of the test's type parameter, or NULL if
//                     this is not a typed or a type-parameterized test.
//   value_param:      text representation of the test's value parameter,
//                     or NULL if this is not a value-parameterized test.
//   code_location:    code location where the test is defined
//   fixture_class_id: ID of the test fixture class
//   set_up_tc:        pointer to the function that sets up the test suite
//   tear_down_tc:     pointer to the function that tears down the test suite
//   factory:          pointer to the factory that creates a test object.
//                     The newly created TestInfo instance will assume
//                     ownership of the factory object.
TestInfo *function_Make_and_register_test_info::MakeAndRegisterTestInfo(
	char const *const test_suite_name,
	char const *const name,
	char const *const type_param,
	char const *const value_param,
	::testing::internal::CodeLocation code_location,
	::testing::internal::TypeId fixture_class_id,
	::testing::internal::SetUpTestSuiteFunc set_up_tc,
	::testing::internal::TearDownTestSuiteFunc tear_down_tc,
	::testing::internal::TestFactoryBase* factory )
{
	TestInfo *const test_info = new TestInfo( test_suite_name, name, type_param, value_param, code_location, fixture_class_id, factory );

	GetUnitTestImpl()->AddTestInfo( set_up_tc, tear_down_tc, test_info );
	return test_info;
}


} // namespace internal
} // namespace cutf
} // namespace jmsd
