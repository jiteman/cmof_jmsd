#pragma once

#include "function_Streamable_to_string.hxx"


#include <string>


namespace jmsd {
namespace cutf {
namespace internal {


class function_Streamable_to_string {

public:
	// Converts a streamable value to an std::string.
	// A NULL pointer is converted to "(null)".
	// When the input value is a ::string, ::std::string, ::wstring, or ::std::wstring object, each NUL
	// character in it is replaced with "\\0".
	template< typename A_type >
	static ::std::string StreamableToString( A_type const &streamable );

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
public:

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
private:
	virtual ~function_Streamable_to_string() noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	function_Streamable_to_string() noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	function_Streamable_to_string( function_Streamable_to_string const &another ) noexcept = delete;
	function_Streamable_to_string &operator =( function_Streamable_to_string const &another ) noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	function_Streamable_to_string( function_Streamable_to_string &&another ) noexcept = delete;
	function_Streamable_to_string &operator =( function_Streamable_to_string &&another ) noexcept = delete;

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
