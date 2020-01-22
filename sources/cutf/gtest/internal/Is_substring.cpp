#include "Is_substring.h"


#include <cstring>


namespace jmsd {
namespace cutf {
namespace internal {


// static
bool Is_substring::IsSubstringPred( char const *needle, char const *haystack ) {
	if ( needle == nullptr || haystack == nullptr ) {
		return needle == haystack;
	}

	return ::strstr( haystack, needle ) != nullptr;
}

// static
bool Is_substring::IsSubstringPred( wchar_t const *needle, wchar_t const *haystack ) {
	if ( needle == nullptr || haystack == nullptr ) {
		return needle == haystack;
	}

	return ::wcsstr(haystack, needle) != nullptr;
}


} // namespace internal
} // namespace cutf
} // namespace jmsd
