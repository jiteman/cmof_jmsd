#pragma once

#include "Message.hxx"


#include <sstream>
#include <iostream>

#include "cutf.h"

#include "internal/gtest-port.h"


// Ensures that there is at least one operator<< in the global namespace.
// See Message& operator<<(...) below for why.
void operator <<( ::testing::internal::Secret const &, int );


namespace jmsd {
namespace cutf {


// The Message class works like an ostream repeater.
//
// Typical usage:
//
//   1. You stream a bunch of values to a Message object.
//      It will remember the text in a stringstream.
//   2. Then you stream the Message object to an ostream.
//      This causes the text in the Message to be streamed
//      to the ostream.
//
// For example;
//
//   testing::Message foo;
//   foo << 1 << " != " << 2;
//   std::cout << foo;
//
// will print "1 != 2".
//
// Message is not intended to be inherited from.  In particular, its
// destructor is not virtual.
//
// Note that stringstream behaves differently in gcc and in MSVC.  You
// can stream a NULL char pointer to it in the former, but not in the
// latter (it causes an access violation if you do).  The Message
// class hides this difference by treating a NULL char pointer as
// "(null)".
class JMSD_CUTF_SHARED_INTERFACE Message {
private:
	// The type of basic IO manipulators (endl, ends, and flush) for narrow streams.
	typedef ::std::ostream &( *BasicNarrowIoManip )( ::std::ostream & );

	public:
	// Constructs an empty Message.
	Message();

	// Copy constructor.
	Message(const Message& msg);

	// Constructs a Message from a C-string.
	explicit Message(const char* str);

	// Streams a non-pointer value to this object.
	template< typename A_type >
	Message& operator <<( A_type const &val );

	// Streams a pointer value to this object.
	//
	// This function is an overload of the previous one.  When you
	// stream a pointer to a Message, this definition will be used as it
	// is more specialized.  (The C++ Standard, section
	// [temp.func.order].)  If you stream a non-pointer, then the
	// previous definition will be used.
	//
	// The reason for this overload is that streaming a NULL pointer to
	// ostream is undefined behavior.  Depending on the compiler, you
	// may get "0", "(nil)", "(null)", or an access violation.  To
	// ensure consistent result across compilers, we always treat NULL
	// as "(null)".
	template< typename A_type >
	Message &operator <<( A_type *const &pointer );

	// Since the basic IO manipulators are overloaded for both narrow
	// and wide streams, we have to provide this specialized definition
	// of operator <<, even though its body is the same as the
	// templatized version above.  Without this definition, streaming
	// endl or other basic IO manipulators to Message will confuse the
	// compiler.
	Message& operator <<(BasicNarrowIoManip val);

	// Instead of 1/0, we want to see true/false for bool values.
	Message& operator <<(bool b);

	// These two overloads allow streaming a wide C string to a Message using the UTF-8 encoding.
	Message& operator <<(const wchar_t* wide_c_str);
	Message& operator <<(wchar_t* wide_c_str);

	#if GTEST_HAS_STD_WSTRING
	// Converts the given wide string to a narrow string using the UTF-8
	// encoding, and streams the result to this Message object.
	Message& operator <<(const ::std::wstring& wstr);
	#endif  // GTEST_HAS_STD_WSTRING

	// Gets the text streamed to this object so far as an std::string.
	// Each '\0' character in the buffer is replaced with "\\0".
	//
	// INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
	std::string GetString() const;

	private:
	// We'll hold the text streamed to this object here.
	const std::unique_ptr< ::std::stringstream> ss_;

	// We declare (but don't implement) this to prevent the compiler
	// from implementing the assignment operator.
	void operator=(const Message&);

};

// Streams a Message to an ostream.
JMSD_CUTF_SHARED_INTERFACE ::std::ostream &operator <<( ::std::ostream &os, Message const &sb );


} // namespace cutf
} // namespace jmsd
