#include "Assertion_message_constructor.h"


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
::jmsd::cutf::AssertionResult EqFailure(const char* expected_expression,
									 const char* actual_expression,
									 const std::string& expected_value,
									 const std::string& actual_value,
									 bool ignoring_case);


} // namespace internal
} // namespace cutf
} // namespace jmsd
