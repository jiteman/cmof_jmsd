#pragma once

#include "Substring_assertions.hxx"


#include "Assertion_result.hxx"


#include "internal/gtest-port.h"


#include "cutf.h"


namespace jmsd {
namespace cutf {


class JMSD_CUTF_SHARED_INTERFACE Substring_assertions {

private:
	typedef Substring_assertions THIS_STATIC;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
public:
// IsSubstring() and IsNotSubstring() are intended to be used as the first argument to {EXPECT,ASSERT}_PRED_FORMAT2(), not by themselves.
// They check whether needle is a substring of haystack (NULL is considered a substring of itself only), and return an
// appropriate error message when they fail.
//
// The {needle,haystack}_expr arguments are the stringified expressions that generated the two real arguments.

	static AssertionResult IsSubstring( const char* needle_expr, const char* haystack_expr, const char* needle, const char* haystack );
	static AssertionResult IsSubstring( const char* needle_expr, const char* haystack_expr, const wchar_t* needle, const wchar_t* haystack );
	static AssertionResult IsNotSubstring( const char* needle_expr, const char* haystack_expr, const char* needle, const char* haystack );
	static AssertionResult IsNotSubstring( const char* needle_expr, const char* haystack_expr, const wchar_t* needle, const wchar_t* haystack );
	static AssertionResult IsSubstring( const char* needle_expr, const char* haystack_expr, const ::std::string& needle, const ::std::string& haystack );
	static AssertionResult IsNotSubstring( const char* needle_expr, const char* haystack_expr, const ::std::string& needle, const ::std::string& haystack );

#if defined( GTEST_HAS_STD_WSTRING )
	static AssertionResult IsSubstring( const char* needle_expr, const char* haystack_expr, const ::std::wstring& needle, const ::std::wstring& haystack );
	static AssertionResult IsNotSubstring( const char* needle_expr, const char* haystack_expr, const ::std::wstring& needle, const ::std::wstring& haystack );
#endif // #if defined( GTEST_HAS_STD_WSTRING )

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
private:
	virtual ~Substring_assertions() noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	Substring_assertions() noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	Substring_assertions( Substring_assertions const &another ) noexcept = delete;
	Substring_assertions &operator =( Substring_assertions const &another ) noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	Substring_assertions( Substring_assertions &&another ) noexcept = delete;
	Substring_assertions &operator =( Substring_assertions &&another ) noexcept = delete;

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
private:

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:

};


} // namespace cutf
} // namespace jmsd
