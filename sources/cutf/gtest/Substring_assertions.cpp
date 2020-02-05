#include "Substring_assertions.h"


#include "Assertion_result.hin"

#include "Message.hin"

#include "internal/Is_substring.hin"


namespace jmsd {
namespace cutf {


// IsSubstring() and IsNotSubstring() check whether needle is a
// substring of haystack (NULL is considered a substring of itself
// only), and return an appropriate error message when they fail.

AssertionResult Substring_assertions::IsSubstring( const char* needle_expr, const char* haystack_expr, const char* needle, const char* haystack) {
	return internal::Is_substring::IsSubstringImpl(true, needle_expr, haystack_expr, needle, haystack);
}

AssertionResult Substring_assertions::IsSubstring( const char* needle_expr, const char* haystack_expr, const wchar_t* needle, const wchar_t* haystack) {
	return internal::Is_substring::IsSubstringImpl(true, needle_expr, haystack_expr, needle, haystack);
}

AssertionResult Substring_assertions::IsNotSubstring( const char* needle_expr, const char* haystack_expr, const char* needle, const char* haystack) {
	return internal::Is_substring::IsSubstringImpl(false, needle_expr, haystack_expr, needle, haystack);
}

AssertionResult Substring_assertions::IsNotSubstring( const char* needle_expr, const char* haystack_expr, const wchar_t* needle, const wchar_t* haystack) {
	return internal::Is_substring::IsSubstringImpl(false, needle_expr, haystack_expr, needle, haystack);
}

AssertionResult Substring_assertions::IsSubstring( const char* needle_expr, const char* haystack_expr, const ::std::string& needle, const ::std::string& haystack) {
	return internal::Is_substring::IsSubstringImpl(true, needle_expr, haystack_expr, needle, haystack);
}

AssertionResult Substring_assertions::IsNotSubstring( const char* needle_expr, const char* haystack_expr, const ::std::string& needle, const ::std::string& haystack) {
	return internal::Is_substring::IsSubstringImpl(false, needle_expr, haystack_expr, needle, haystack);
}

#if defined( GTEST_HAS_STD_WSTRING )
AssertionResult Substring_assertions::IsSubstring( const char* needle_expr, const char* haystack_expr, const ::std::wstring& needle, const ::std::wstring& haystack) {
	return internal::Is_substring::IsSubstringImpl(true, needle_expr, haystack_expr, needle, haystack);
}

AssertionResult Substring_assertions::IsNotSubstring( const char* needle_expr, const char* haystack_expr, const ::std::wstring& needle, const ::std::wstring& haystack) {
  return internal::Is_substring::IsSubstringImpl(false, needle_expr, haystack_expr, needle, haystack);
}
#endif // #if defined( GTEST_HAS_STD_WSTRING )


} // namespace cutf
} // namespace jmsd
