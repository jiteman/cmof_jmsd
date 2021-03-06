#include "utf8_utilities.h"


#include "function_String_stream_to_string.h"

#include "gtest-string.h"

#include "gtest/Message.hin"

#include <sstream>


namespace jmsd {
namespace cutf {
namespace internal {


// Utility functions for encoding Unicode text (wide strings) in
// UTF-8.

// A Unicode code-point can have up to 21 bits, and is encoded in UTF-8
// like this:
//
// Code-point length   Encoding
//   0 -  7 bits       0xxxxxxx
//   8 - 11 bits       110xxxxx 10xxxxxx
//  12 - 16 bits       1110xxxx 10xxxxxx 10xxxxxx
//  17 - 21 bits       11110xxx 10xxxxxx 10xxxxxx 10xxxxxx

// The maximum code-point a one-byte UTF-8 sequence can represent.
// static
uint32_t const utf8_utilities::kMaxCodePoint1 = ( static_cast< uint32_t >( 1 ) <<  7 ) - 1;

// static
// The maximum code-point a two-byte UTF-8 sequence can represent.
uint32_t const utf8_utilities::kMaxCodePoint2 = ( static_cast< uint32_t >( 1 ) << ( 5 + 6 ) ) - 1;

// static
// The maximum code-point a three-byte UTF-8 sequence can represent.
uint32_t const utf8_utilities::kMaxCodePoint3 = ( static_cast< uint32_t >( 1 ) << ( 4 + 2 * 6 ) ) - 1;

// static
// The maximum code-point a four-byte UTF-8 sequence can represent.
uint32_t const utf8_utilities::kMaxCodePoint4 = ( static_cast< uint32_t >( 1 ) << ( 3 + 3 * 6 ) ) - 1;

// Chops off the n lowest bits from a bit pattern.  Returns the n
// lowest bits.  As a side effect, the original bit pattern will be
// shifted to the right by n bits.
uint32_t utf8_utilities::utf8_utilities::ChopLowBits(uint32_t* bits, int n) {
  const uint32_t low_bits = *bits & ((static_cast<uint32_t>(1) << n) - 1);
  *bits >>= n;
  return low_bits;
}

// Converts a Unicode code point to a narrow string in UTF-8 encoding.
// code_point parameter is of type uint32_t because wchar_t may not be
// wide enough to contain a code point.
// If the code_point is not a valid Unicode code point
// (i.e. outside of Unicode range U+0 to U+10FFFF) it will be converted
// to "(Invalid Unicode 0xXXXXXXXX)".
::std::string utf8_utilities::CodePointToUtf8(uint32_t code_point) {
  if (code_point > kMaxCodePoint4) {
	return "(Invalid Unicode 0x" + ::testing::internal::String::FormatHexUInt32(code_point) + ")";
  }

  char str[5];  // Big enough for the largest valid code point.
  if (code_point <= kMaxCodePoint1) {
	str[1] = '\0';
	str[0] = static_cast<char>(code_point);                          // 0xxxxxxx
  } else if (code_point <= kMaxCodePoint2) {
	str[2] = '\0';
	str[1] = static_cast<char>(0x80 | ChopLowBits(&code_point, 6));  // 10xxxxxx
	str[0] = static_cast<char>(0xC0 | code_point);                   // 110xxxxx
  } else if (code_point <= kMaxCodePoint3) {
	str[3] = '\0';
	str[2] = static_cast<char>(0x80 | ChopLowBits(&code_point, 6));  // 10xxxxxx
	str[1] = static_cast<char>(0x80 | ChopLowBits(&code_point, 6));  // 10xxxxxx
	str[0] = static_cast<char>(0xE0 | code_point);                   // 1110xxxx
  } else {  // code_point <= kMaxCodePoint4
	str[4] = '\0';
	str[3] = static_cast<char>(0x80 | ChopLowBits(&code_point, 6));  // 10xxxxxx
	str[2] = static_cast<char>(0x80 | ChopLowBits(&code_point, 6));  // 10xxxxxx
	str[1] = static_cast<char>(0x80 | ChopLowBits(&code_point, 6));  // 10xxxxxx
	str[0] = static_cast<char>(0xF0 | code_point);                   // 11110xxx
  }
  return str;
}

// The following two functions only make sense if the system
// uses UTF-16 for wide string encoding. All supported systems
// with 16 bit wchar_t (Windows, Cygwin) do use UTF-16.

// Determines if the arguments constitute UTF-16 surrogate pair
// and thus should be combined into a single Unicode code point
// using CreateCodePointFromUtf16SurrogatePair.
bool utf8_utilities::IsUtf16SurrogatePair(wchar_t first, wchar_t second) {
  return sizeof(wchar_t) == 2 &&
	  (first & 0xFC00) == 0xD800 && (second & 0xFC00) == 0xDC00;
}

// Creates a Unicode code point from UTF16 surrogate pair.
uint32_t utf8_utilities::CreateCodePointFromUtf16SurrogatePair(wchar_t first,
													  wchar_t second) {
  const auto first_u = static_cast<uint32_t>(first);
  const auto second_u = static_cast<uint32_t>(second);
  const uint32_t mask = (1 << 10) - 1;
  return (sizeof(wchar_t) == 2)
			 ? (((first_u & mask) << 10) | (second_u & mask)) + 0x10000
			 :
			 // This function should not be called when the condition is
			 // false, but we provide a sensible default in case it is.
			 first_u;
}

// Converts a wide string to a narrow string in UTF-8 encoding.
// The wide string is assumed to have the following encoding:
//   UTF-16 if sizeof(wchar_t) == 2 (on Windows, Cygwin)
//   UTF-32 if sizeof(wchar_t) == 4 (on Linux)
// Parameter str points to a null-terminated wide string.
// Parameter num_chars may additionally limit the number
// of wchar_t characters processed. -1 is used when the entire string
// should be processed.
// If the string contains code points that are not valid Unicode code points
// (i.e. outside of Unicode range U+0 to U+10FFFF) they will be output
// as '(Invalid Unicode 0xXXXXXXXX)'. If the string is in UTF16 encoding
// and contains invalid UTF-16 surrogate pairs, values in those pairs
// will be encoded as individual Unicode characters from Basic Normal Plane.
::std::string utf8_utilities::WideStringToUtf8(const wchar_t* str, int num_chars) {
  if (num_chars == -1)
	num_chars = static_cast<int>(wcslen(str));

  ::std::stringstream stream;
  for (int i = 0; i < num_chars; ++i) {
	uint32_t unicode_code_point;

	if (str[i] == L'\0') {
	  break;
	} else if (i + 1 < num_chars && IsUtf16SurrogatePair(str[i], str[i + 1])) {
	  unicode_code_point = CreateCodePointFromUtf16SurrogatePair(str[i],
																 str[i + 1]);
	  i++;
	} else {
	  unicode_code_point = static_cast<uint32_t>(str[i]);
	}

	stream << CodePointToUtf8(unicode_code_point);
  }

  return function_String_stream_to_string::StringStreamToString( stream );
}

#if GTEST_HAS_STD_WSTRING

// Converts an array of wide chars to a narrow string using the UTF-8 encoding, and streams the result to the given Message object.
// static
void utf8_utilities::StreamWideCharsToMessage( wchar_t const *wstr, size_t length, Message *msg ) {
	for ( size_t i = 0; i != length; ) {
		if ( wstr[i] != L'\0' ) {
			*msg << WideStringToUtf8( wstr + i, static_cast< int >( length - i ) );

			while ( i != length && wstr[ i ] != L'\0' ) {
				i++;
			}
		} else {
			*msg << '\0';
			i++;
		}
	}
}

#endif  // GTEST_HAS_STD_WSTRING


} // namespace internal
} // namespace cutf
} // namespace jmsd
