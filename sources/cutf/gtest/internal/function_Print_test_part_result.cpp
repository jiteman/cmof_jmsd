#include "function_Print_test_part_result.h"


#include "function_Print_test_part_result_to_string.h"

#include <string>

#if GTEST_OS_WINDOWS && !GTEST_OS_WINDOWS_MOBILE
#include "custom/temporary_windows_include.h"
#endif // #if GTEST_OS_WINDOWS && !GTEST_OS_WINDOWS_MOBILE


namespace jmsd {
namespace cutf {
namespace internal {


// Prints a TestPartResult.
// static
void function_Print_test_part_result::PrintTestPartResult( ::testing::TestPartResult const &test_part_result ) {
	::std::string const &result = function_Print_test_part_result_to_string::PrintTestPartResultToString(test_part_result);
	::printf("%s\n", result.c_str());
	::fflush(stdout);

#if GTEST_OS_WINDOWS && !GTEST_OS_WINDOWS_MOBILE
	// If the test program runs in Visual Studio or a debugger, the
	// following statements add the test part result message to the Output
	// window such that the user can double-click on it to jump to the
	// corresponding source code location; otherwise they do nothing.

	// We don't call OutputDebugString*() on Windows Mobile, as printing
	// to stdout is done by OutputDebugString() there already - we don't
	// want the same message printed twice.
	::OutputDebugStringA( result.c_str() );
	::OutputDebugStringA( "\n" );
#endif
}


} // namespace internal
} // namespace cutf
} // namespace jmsd
