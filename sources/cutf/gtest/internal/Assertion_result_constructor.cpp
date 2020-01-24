#include "Assertion_result_constructor.h"


#include "gtest/Assertion_result.h"
#include "gtest/gtest-message.h"

#include "Split_escaped_string.h"
#include "Distance_editor.h"


namespace jmsd {
namespace cutf {
namespace internal {


// Constructs and returns the message for an equality assertion
// (e.g. ASSERT_EQ, EXPECT_STREQ, etc) failure.
//
// The first four parameters are the expressions used in the assertion
// and their values, as strings.  For example, for ASSERT_EQ(foo, bar)
// where foo is 5 and bar is 6, we have:
//
//   expected_expression: "foo"
//   actual_expression:   "bar"
//   expected_value:      "5"
//   actual_value:        "6"
//
// The ignoring_case parameter is true if and only if the assertion is a
// *_STRCASEEQ*.  When it's true, the string " (ignoring case)" will
// be inserted into the message.
AssertionResult Assertion_result_constructor::construct_expected_equality(
	char const *const lhs_expression,
	char const *const rhs_expression,
	::std::string const &lhs_value,
	::std::string const &rhs_value,
	bool const ignoring_case )
{
	::testing::Message msg;
	msg << "Expected equality of these values:" << "\n  " << lhs_expression;

	if ( lhs_value != lhs_expression ) {
		msg << "\n    Which is: " << lhs_value;
	}

	msg << "\n  " << rhs_expression;

	if ( rhs_value != rhs_expression ) {
		msg << "\n    Which is: " << rhs_value;
	}

	if ( ignoring_case ) {
		msg << "\nIgnoring case";
	}

	if ( !lhs_value.empty() && !rhs_value.empty() ) {
		::std::vector< ::std::string > const lhs_lines = Split_escaped_string::SplitEscapedString( lhs_value );
		::std::vector< ::std::string > const rhs_lines = Split_escaped_string::SplitEscapedString( rhs_value );

		if ( lhs_lines.size() > 1 || rhs_lines.size() > 1 ) {
			msg << "\nWith diff:\n" << Distance_editor::CreateUnifiedDiff( lhs_lines, rhs_lines );
		}
	}

	return AssertionResult::AssertionFailure() << msg;
}


} // namespace internal
} // namespace cutf
} // namespace jmsd
