#include "function_Format_countable.h"


#include "Message.hin"
#include "internal/function_Streamable_to_string.hin"


namespace jmsd {
namespace cutf {


// Formats a countable noun.  Depending on its quantity, either the singular form or the plural form is used. e.g.
//
// FormatCountableNoun(1, "formula", "formuli") returns "1 formula".
// FormatCountableNoun(5, "book", "books") returns "5 books".
// static
::std::string function_Format_countable::FormatCountableNoun( int count, char const *singular_form, char const *plural_form) {
	return internal::function_Streamable_to_string::StreamableToString( count ) + " " + ( count == 1 ? singular_form : plural_form );
}

// Formats the count of tests.
// static
::std::string function_Format_countable::FormatTestCount( int test_count ) {
	return FormatCountableNoun( test_count, "test", "tests" );
}

// Formats the count of test suites.
// static
::std::string function_Format_countable::FormatTestSuiteCount( int test_suite_count ) {
	return FormatCountableNoun( test_suite_count, "test suite", "test suites" );
}


} // namespace cutf
} // namespace jmsd

