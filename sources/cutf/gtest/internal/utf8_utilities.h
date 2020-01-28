#pragma once

#include "utf8_utilities.hxx"


#include "cutf.h"

#include "gtest-port.h"


namespace jmsd {
namespace cutf {
namespace internal {


class JMSD_CUTF_SHARED_INTERFACE utf8_utilities {

public:
	// Chops off the n lowest bits from a bit pattern.  Returns the n
	// lowest bits.  As a side effect, the original bit pattern will be
	// shifted to the right by n bits.
	static uint32_t ChopLowBits(uint32_t* bits, int n);

	// Converts a Unicode code point to a narrow string in UTF-8 encoding.
	// code_point parameter is of type uint32_t because wchar_t may not be
	// wide enough to contain a code point.
	// If the code_point is not a valid Unicode code point
	// (i.e. outside of Unicode range U+0 to U+10FFFF) it will be converted
	// to "(Invalid Unicode 0xXXXXXXXX)".
	static ::std::string CodePointToUtf8(uint32_t code_point);

	// The following two functions only make sense if the system
	// uses UTF-16 for wide string encoding. All supported systems
	// with 16 bit wchar_t (Windows, Cygwin) do use UTF-16.

	// Determines if the arguments constitute UTF-16 surrogate pair
	// and thus should be combined into a single Unicode code point
	// using CreateCodePointFromUtf16SurrogatePair.
	static bool IsUtf16SurrogatePair(wchar_t first, wchar_t second);

	// Creates a Unicode code point from UTF16 surrogate pair.
	static uint32_t CreateCodePointFromUtf16SurrogatePair(wchar_t first, wchar_t second);

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
	static ::std::string WideStringToUtf8(const wchar_t* str, int num_chars);

#if GTEST_HAS_STD_WSTRING

	// Converts an array of wide chars to a narrow string using the UTF-8 encoding, and streams the result to the given Message object.
	static void StreamWideCharsToMessage( wchar_t const *wstr, size_t length, Message *msg );

#endif  // GTEST_HAS_STD_WSTRING


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	static const uint32_t kMaxCodePoint1;

	// The maximum code-point a two-byte UTF-8 sequence can represent.
	static const uint32_t kMaxCodePoint2;

	// The maximum code-point a three-byte UTF-8 sequence can represent.
	static const uint32_t kMaxCodePoint3;

	// The maximum code-point a four-byte UTF-8 sequence can represent.
	static const uint32_t kMaxCodePoint4;

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
private:
	virtual ~utf8_utilities() noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
protected:
	utf8_utilities() noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	utf8_utilities( utf8_utilities const &another ) noexcept = delete;
	utf8_utilities &operator =( utf8_utilities const &another ) noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	utf8_utilities( utf8_utilities &&another ) noexcept = delete;
	utf8_utilities &operator =( utf8_utilities &&another ) noexcept = delete;

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
