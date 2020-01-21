#include "Unit_test_options.h"


#include "gtest/Unit_test.h"
#include "gtest/gtest-constants.h"
#include "gtest/gtest-flags.h"

#include "gtest-filepath.h"


namespace jmsd {
namespace cutf {
namespace internal {


// Returns the output format, or "" for normal printed output.
std::string UnitTestOptions::GetOutputFormat() {
	char const *const gtest_output_flag = ::testing:: GTEST_FLAG( output ).c_str();
	char const *const colon = ::strchr( gtest_output_flag, ':' );

	return
		( colon == nullptr ) ?
			::std::string( gtest_output_flag ) :
			::std::string( gtest_output_flag, static_cast< size_t >( colon - gtest_output_flag ) );
}

// Returns the name of the requested output file, or the default if none was explicitly specified.
::std::string UnitTestOptions::GetAbsolutePathToOutputFile() {
	char const *const gtest_output_flag = ::testing:: GTEST_FLAG(output).c_str();

	::std::string format = GetOutputFormat();

	if ( format.empty() ) {
		format = ::std::string( constants::kDefaultOutputFormat );
	}

	char const *const colon = ::strchr( gtest_output_flag, ':' );

	if ( colon == nullptr ) {
		return
			::testing::internal::FilePath::MakeFileName(
				::testing::internal::FilePath(
					UnitTest::GetInstance()->original_working_dir() ),
				::testing::internal::FilePath( ::jmsd::cutf::constants::kDefaultOutputFile ),
				0,
				format.c_str() ).string();
	}

	::testing::internal::FilePath output_name( colon + 1 );

	if ( !output_name.IsAbsolutePath() ) {
		output_name =
			::testing::internal::FilePath::ConcatPaths(
				::testing::internal::FilePath( ::jmsd::cutf::UnitTest::GetInstance()->original_working_dir() ),
				::testing::internal::FilePath( colon + 1 ) );
	}

	if ( !output_name.IsDirectory() ) {
		return output_name.string();
	}

	::testing::internal::FilePath result(
		::testing::internal::FilePath::GenerateUniqueFileName(
			output_name, ::testing::internal::GetCurrentExecutableName(), GetOutputFormat().c_str() ) );

	return result.string();
}

// Returns true if and only if the wildcard pattern matches the string.
// The first ':' or '\0' character in pattern marks the end of it.
//
// This recursive algorithm isn't very efficient, but is clear and
// works well enough for matching test names, which are short.
bool UnitTestOptions::PatternMatchesString( char const *const pattern, char const *const str ) {
	switch (*pattern) {
		case '\0':
		case ':':
			return *str == '\0'; // Either ':' or '\0' marks the end of the pattern.
		case '?':
			return *str != '\0' && PatternMatchesString( pattern + 1, str + 1 ); // Matches any single character.
		case '*':
			return ( *str != '\0' && PatternMatchesString( pattern, str + 1 ) ) || PatternMatchesString( pattern + 1, str ); // Matches any string (possibly empty) of characters.
		default:
			return *pattern == *str && PatternMatchesString( pattern + 1, str + 1 ); // Non-special character. Matches itself.
	}
}

// Returns true if and only if the user-specified filter matches the test suite name and the test name.
bool UnitTestOptions::FilterMatchesTest( std::string const &test_suite_name, ::std::string const &test_name ) {
	::std::string const &full_name = test_suite_name + "." + test_name.c_str();

  // Split --gtest_filter at '-', if there is one, to separate into
  // positive filter and negative filter portions
  const char* const p = ::testing:: GTEST_FLAG(filter).c_str();
  const char* const dash = strchr(p, '-');
  std::string positive;
  std::string negative;
  if (dash == nullptr) {
	positive = ::testing:: GTEST_FLAG(filter).c_str();  // Whole string is a positive filter
	negative = "";
  } else {
	positive = std::string(p, dash);   // Everything up to the dash
	negative = std::string(dash + 1);  // Everything after the dash
	if (positive.empty()) {
	  // Treat '-test1' as the same as '*-test1'
	  positive = constants::kUniversalFilter;
	}
  }

  // A filter is a colon-separated list of patterns.  It matches a
  // test if any pattern in it matches the test.
  return (MatchesFilter(full_name, positive.c_str()) && !MatchesFilter(full_name, negative.c_str()));
}

bool UnitTestOptions::MatchesFilter( ::std::string const &name, char const *const filter) {
  const char *cur_pattern = filter;
  for (;;) {
	if (PatternMatchesString(cur_pattern, name.c_str())) {
	  return true;
	}

	// Finds the next pattern in the filter.
	cur_pattern = strchr(cur_pattern, ':');

	// Returns if no more pattern can be found.
	if (cur_pattern == nullptr) {
	  return false;
	}

	// Skips the pattern separater (the ':' character).
	cur_pattern++;
  }
}

#if GTEST_HAS_SEH
// Returns EXCEPTION_EXECUTE_HANDLER if Google Test should handle the
// given SEH exception, or EXCEPTION_CONTINUE_SEARCH otherwise.
// This function is useful as an __except condition.
int UnitTestOptions::GTestShouldProcessSEH(DWORD exception_code) {
  // Google Test should handle a SEH exception if:
  //   1. the user wants it to, AND
  //   2. this is not a breakpoint exception, AND
  //   3. this is not a C++ exception (VC++ implements them via SEH,
  //      apparently).
  //
  // SEH exception code for C++ exceptions.
  // (see http://support.microsoft.com/kb/185294 for more information).
  const DWORD kCxxExceptionCode = 0xe06d7363;

  bool should_handle = true;

  if (! ::testing::GTEST_FLAG(catch_exceptions))
	should_handle = false;
  else if (exception_code == EXCEPTION_BREAKPOINT)
	should_handle = false;
  else if (exception_code == kCxxExceptionCode)
	should_handle = false;

  return should_handle ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH;
}
#endif  // GTEST_HAS_SEH


} // namespace internal
} // namespace cutf
} // namespace jmsd
