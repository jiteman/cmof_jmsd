#include "Message.h"


#include "internal/gtest-string.h"
#include "internal/function_String_stream_to_string.h"
#include "internal/utf8_utilities.h"

#include "Message.hin"

#include <iomanip>
#include <limits>

#include "internal/gtest-port.h"


namespace jmsd {
namespace cutf {


// Constructs an empty Message.
// We allocate the stringstream separately because otherwise each use of
// ASSERT/EXPECT in a procedure adds over 200 bytes to the procedure's
// stack frame leading to huge stack frames in some cases; gcc does not reuse
// the stack space.
Message::Message()
	:
		ss_( new ::std::stringstream )
{
	// By default, we want there to be enough precision when printing a double to a Message.
	*ss_ << ::std::setprecision( ::std::numeric_limits< double >::digits10 + 2 );
}

Message::Message(const Message& msg)
	:
		ss_(new ::std::stringstream)
{
	*ss_ << msg.GetString();
}

Message::Message(const char* str)
	:
		ss_(new ::std::stringstream)
{
	*ss_ << str;
}

// Since the basic IO manipulators are overloaded for both narrow
// and wide streams, we have to provide this specialized definition
// of operator <<, even though its body is the same as the
// templatized version above.  Without this definition, streaming
// endl or other basic IO manipulators to Message will confuse the
// compiler.
Message &Message::operator <<(BasicNarrowIoManip val) {
	*ss_ << val;
	return *this;
}

// Instead of 1/0, we want to see true/false for bool values.
Message &Message::operator <<(bool b) {
	return *this << (b ? "true" : "false");
}

// These two overloads allow streaming a wide C string to a Message using the UTF-8 encoding.
Message &Message::operator <<( wchar_t const *wide_c_str ) {
	return *this << ::testing::internal::String::ShowWideCString( wide_c_str );
}

Message &Message::operator <<( wchar_t *wide_c_str ) {
  return *this << ::testing::internal::String::ShowWideCString( wide_c_str );
}

#if GTEST_HAS_STD_WSTRING
// Converts the given wide string to a narrow string using the UTF-8 encoding, and streams the result to this Message object.
Message & Message::operator <<( ::std::wstring const &wstr ) {
	internal::utf8_utilities::StreamWideCharsToMessage( wstr.c_str(), wstr.length(), this );
	return *this;
}
#endif  // GTEST_HAS_STD_WSTRING

// Gets the text streamed to this object so far as an std::string.
// Each '\0' character in the buffer is replaced with "\\0".
::std::string Message::GetString() const {
  return ::jmsd::cutf::internal::function_String_stream_to_string::StringStreamToString( *ss_.get() );
}

// Streams a Message to an ostream.
::std::ostream &operator <<( ::std::ostream &os, Message const &sb ) {
	return os << sb.GetString();
}


} // namespace cutf
} // namespace jmsd
