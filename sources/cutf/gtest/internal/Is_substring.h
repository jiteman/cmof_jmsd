#pragma once

#include "Is_substring.hxx"


#include "gtest/Assertion_result.hxx"



namespace jmsd {
namespace cutf {
namespace internal {


// Helper functions for implementing String_utilities (IsSubString() and IsNotSubstring()).
class Is_substring {

private:
	typedef Is_substring THIS_STATIC;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
public:
	// This group of overloaded functions return true if and only if needle is a substring of haystack.
	// NULL is considered a substring of itself only.

	static bool IsSubstringPred( char const *needle, char const *haystack );

	static bool IsSubstringPred( wchar_t const *needle, wchar_t const *haystack );

	// StringType here can be either ::std::string or ::std::wstring.
	template< typename StringType >
	static bool IsSubstringPred( StringType const &needle, StringType const &haystack );

	// This function implements either IsSubstring() or IsNotSubstring(),
	// depending on the value of the expected_to_be_substring parameter.
	// StringType here can be const char*, const wchar_t*, ::std::string,
	// or ::std::wstring.
	template< typename StringType >
	static AssertionResult IsSubstringImpl( bool expected_to_be_substring, char const *needle_expr, char const *haystack_expr, StringType const &needle, StringType const &haystack );

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
private:
	virtual ~Is_substring() noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	Is_substring() noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	Is_substring( Is_substring const &another ) noexcept = delete;
	Is_substring &operator =( Is_substring const &another ) noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	Is_substring( Is_substring &&another ) noexcept = delete;
	Is_substring &operator =( Is_substring &&another ) noexcept = delete;

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
private:

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:

};


} // namespace internal
} // namespace cutf
} // namespace jmsd
