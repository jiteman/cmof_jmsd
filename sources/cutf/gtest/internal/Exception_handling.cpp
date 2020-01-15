#include "Exception_handling.h"


#include "gtest/gtest-test-part.h"
#include "gtest/gtest-message.h"

#include "gtest/Test_part_result_to_string.h"

#include <iomanip>


namespace jmsd {
namespace cutf {


#if GTEST_HAS_SEH

// Adds an "exception thrown" fatal failure to the current test.  This
// function returns its result via an output parameter pointer because VC++
// prohibits creation of objects with destructors on stack in functions
// using __try (see error C2712).
::std::string *FormatSehExceptionMessage( DWORD exception_code, const char *location ) {
	::testing::Message message;
	message << "SEH exception with code 0x" << ::std::setbase(16) <<
	exception_code << ::std::setbase(10) << " thrown in " << location << ".";

	return new ::std::string(message.GetString());
}

#endif  // GTEST_HAS_SEH


namespace internal {


#if GTEST_HAS_EXCEPTIONS

// Adds an "exception thrown" fatal failure to the current test.
::std::string FormatCxxExceptionMessage( char const *description, char const *location ) {
	::testing::Message message;

	if ( description != nullptr ) {
		message << "C++ exception with description \"" << description << "\"";
	} else {
		message << "Unknown C++ exception";
	}

	message << " thrown in " << location << ".";
	return message.GetString();
}

// Prints a TestPartResult to an std::string.
::std::string PrintTestPartResultToString( ::testing::TestPartResult const &test_part_result ) {
  return
	( ::testing::Message() <<
		  ::testing::internal::FormatFileLocation( test_part_result.file_name(), test_part_result.line_number() ) <<
		  " " << ::jmsd::cutf::TestPartResultTypeToString( test_part_result.type() ) <<
		  test_part_result.message() ).GetString();
}

GoogleTestFailureException::GoogleTestFailureException( ::testing::TestPartResult const &failure )
	:
		::std::runtime_error( PrintTestPartResultToString( failure ).c_str() )
{}

#endif  // GTEST_HAS_EXCEPTIONS


} // namespace internal
} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
