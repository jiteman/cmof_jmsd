#include "function_Get_default_filter.h"


#include "gtest-constants.h"

#include "internal/gtest-port.h"


namespace jmsd {
namespace cutf {


// Bazel passes in the argument to '--test_filter' via the TESTBRIDGE_TEST_ONLY environment variable.
// static
char const *function_Get_default_filter::GetDefaultFilter() {
	char const *const testbridge_test_only = ::testing::internal::posix::GetEnv( "TESTBRIDGE_TEST_ONLY" );

	if ( testbridge_test_only == nullptr ) {
		return constants::kUniversalFilter;
	}

	return testbridge_test_only;
}


} // namespace cutf
} // namespace jmsd
