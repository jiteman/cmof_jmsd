#include "function_Split_escaped_string.h"


namespace jmsd {
namespace cutf {
namespace internal {


// The string representation of the values received in EqFailure() are already
// escaped. Split them on escaped '\n' boundaries. Leave all other escaped
// characters the same.
::std::vector< ::std::string > function_Split_escaped_string::SplitEscapedString( ::std::string const &str ) {
	::std::vector< ::std::string > lines;
	size_t start = 0;
	size_t end = str.size();

	if ( end > 2 && str[ 0 ] == '"' && str[ end - 1 ] == '"' ) {
		++start;
		--end;
	}

	bool escaped = false;

	for ( size_t i = start; i + 1 < end; ++i ) {
		if ( escaped ) {
			escaped = false;

			if ( str[ i ] == 'n' ) {
				lines.push_back( str.substr( start, i - start - 1 ) );
				start = i + 1;
			}
		} else {
			escaped = ( str[ i ] == '\\' );
		}
	}

	lines.push_back( str.substr( start, end - start ) );
	return lines;
}


} // namespace internal
} // namespace cutf
} // namespace jmsd
