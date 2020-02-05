#include "Exception_handling.h"


#include "gtest/gtest-test-part.h"

#include "gtest/Test_part_result_to_string.h"

#include "function_Print_test_part_result_to_string.h"

#include "gtest/Message.hin"

#include <iomanip>


namespace jmsd {
namespace cutf {


#if GTEST_HAS_SEH

// Adds an "exception thrown" fatal failure to the current test.  This
// function returns its result via an output parameter pointer because VC++
// prohibits creation of objects with destructors on stack in functions
// using __try (see error C2712).
::std::string *FormatSehExceptionMessage( DWORD exception_code, const char *location ) {
	Message message;
	message << "SEH exception with code 0x" << ::std::setbase(16) <<
	exception_code << ::std::setbase(10) << " thrown in " << location << ".";

	return new ::std::string(message.GetString());
}

#endif  // GTEST_HAS_SEH


namespace internal {


#if GTEST_HAS_EXCEPTIONS

// Adds an "exception thrown" fatal failure to the current test.
::std::string FormatCxxExceptionMessage( char const *description, char const *location ) {
	Message message;

	if ( description != nullptr ) {
		message << "C++ exception with description \"" << description << "\"";
	} else {
		message << "Unknown C++ exception";
	}

	message << " thrown in " << location << ".";
	return message.GetString();
}

GoogleTestFailureException::GoogleTestFailureException( ::testing::TestPartResult const &failure )
	:
		::std::runtime_error( function_Print_test_part_result_to_string::PrintTestPartResultToString( failure ).c_str() )
{}

#endif  // GTEST_HAS_EXCEPTIONS


} // namespace internal
} // namespace cutf
} // namespace jmsd
