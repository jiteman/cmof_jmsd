#pragma once

#include "Unit_test_options.hxx"

#include "gtest-port.h"

#if GTEST_HAS_SEH
#include "custom/temporary_windows_include.h"
#endif // #if GTEST_HAS_SEH

namespace jmsd {
namespace cutf {
namespace internal {


// This class contains functions for processing options the user
// specifies when running the tests.  It has only static members.
//
// In most cases, the user can specify an option using either an
// environment variable or a command line flag.  E.g. you can set the
// test filter using either GTEST_FILTER or --gtest_filter.  If both
// the variable and the flag are present, the latter overrides the
// former.
class JMSD_DEPRECATED_GTEST_API_ UnitTestOptions {
public:
	// Functions for processing the gtest_output flag.

	// Returns the output format, or "" for normal printed output.
	static ::std::string GetOutputFormat();

	// Returns the absolute path of the requested output file, or the
	// default (test_detail.xml in the original working directory) if
	// none was explicitly specified.
	static ::std::string GetAbsolutePathToOutputFile();

	// Functions for processing the gtest_filter flag.

	// Returns true if and only if the wildcard pattern matches the string.
	// The first ':' or '\0' character in pattern marks the end of it.
	//
	// This recursive algorithm isn't very efficient, but is clear and
	// works well enough for matching test names, which are short.
	static bool PatternMatchesString(const char *pattern, const char *str);

	// Returns true if and only if the user-specified filter matches the test
	// suite name and the test name.
	static bool FilterMatchesTest(const ::std::string& test_suite_name, const ::std::string& test_name);

	// Returns true if "name" matches the ':' separated list of glob-style
	// filters in "filter".
	static bool MatchesFilter(const ::std::string& name, const char* filter);

#if GTEST_HAS_SEH
	// Function for supporting the gtest_catch_exception flag.

	// Returns EXCEPTION_EXECUTE_HANDLER if Google Test should handle the
	// given SEH exception, or EXCEPTION_CONTINUE_SEARCH otherwise.
	// This function is useful as an __except condition.
	static int GTestShouldProcessSEH(DWORD exception_code);
#endif  // GTEST_OS_WINDOWS
};


} // namespace internal
} // namespace cutf
} // namespace jmsd
