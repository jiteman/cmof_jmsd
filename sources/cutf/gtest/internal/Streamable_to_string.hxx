#pragma once


namespace jmsd {
namespace cutf {
namespace internal {


// Converts a streamable value to an std::string.
// A NULL pointer is converted to "(null)".
// When the input value is a ::string, ::std::string, ::wstring, or ::std::wstring object, each NUL
// character in it is replaced with "\\0".
template < typename A_type >
::std::string StreamableToString( T const &streamable );


} // namespace internal
} // namespace cutf
} // namespace jmsd
