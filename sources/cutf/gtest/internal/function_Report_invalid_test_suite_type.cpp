#include "function_Report_invalid_test_suite_type.h"


#include "gtest/Message.hin"

#include "gtest-internal.h"


namespace jmsd {
namespace cutf {
namespace internal {


// static
void function_Report_invalid_test_suite_type::ReportInvalidTestSuiteType( char const *const test_suite_name, ::testing::internal::CodeLocation const &code_location ) {
	Message errors;
	errors <<
		"Attempted redefinition of test suite " << test_suite_name << ".\n" <<
		"All tests in the same test suite must use the same test fixture\n" <<
		"class.  However, in test suite " << test_suite_name << ", you tried\n" <<
		"to define a test using a fixture class different from the one\n" <<
		"used earlier. This can happen if the two fixture classes are\n" <<
		"from different namespaces and have the same name. You should\n" <<
		"probably rename one of the classes to put the tests into different\n" <<
		"test suites.";

	GTEST_LOG_( ERROR ) << ::testing::internal::FormatFileLocation( code_location.file.c_str(), code_location.line ) << " " << errors.GetString();
}


} // namespace internal
} // namespace cutf
} // namespace jmsd
