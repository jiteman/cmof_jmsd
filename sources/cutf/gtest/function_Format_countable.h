#pragma once

#include "function_Format_countable.hxx"


#include <string>


namespace jmsd {
namespace cutf {


class function_Format_countable {

public:
	// Formats a countable noun.  Depending on its quantity, either the singular form or the plural form is used. e.g.
	//
	// FormatCountableNoun(1, "formula", "formuli") returns "1 formula".
	// FormatCountableNoun(5, "book", "books") returns "5 books".
	static ::std::string FormatCountableNoun( int count, char const *singular_form, char const *plural_form);

	// Formats the count of tests.
	static ::std::string FormatTestCount( int test_count );

	// Formats the count of test suites.
	static ::std::string FormatTestSuiteCount( int test_suite_count );

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
public:

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
private:
	virtual ~function_Format_countable() noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	function_Format_countable() noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	function_Format_countable( function_Format_countable const &another ) noexcept = delete;
	function_Format_countable &operator =( function_Format_countable const &another ) noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	function_Format_countable( function_Format_countable &&another ) noexcept = delete;
	function_Format_countable &operator =( function_Format_countable &&another ) noexcept = delete;

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
private:

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:

};


} // namespace cutf
} // namespace jmsd
