#include "function_String_stream_to_string.h"


namespace jmsd {
namespace cutf {
namespace internal {


// Converts the buffer in a stringstream to an std::string, converting NUL bytes to "\\0" along the way.
::std::string function_String_stream_to_string::StringStreamToString( ::std::stringstream const &ss ) {
	::std::string const &str = ss.str();
	char const *const start = str.c_str();
	char const *const end = start + str.length();

	::std::string result;
	result.reserve( static_cast< size_t >( 2 * ( end - start ) ) );

	for ( char const *ch = start; ch != end; ++ch) {
		if ( *ch == '\0' ) {
			result += "\\0";  // Replaces NUL with "\\0";
		} else {
			result += *ch;
		}
	}

	return result;
}


} // namespace internal
} // namespace cutf
} // namespace jmsd
