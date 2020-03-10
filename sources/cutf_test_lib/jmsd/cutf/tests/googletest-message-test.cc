// Tests for the Message class.

#include "gtest/Substring_assertions.h"
#include "gtest/internal/function_String_stream_to_string.h"

#include "gtest/Assertion_result.hin"
#include "gtest/Message.hin"

#include "gtest/gtest.h"


namespace {

using ::jmsd::cutf::Message;

// Tests the testing::Message class

// Tests the default constructor.
TEST(MessageTest, DefaultConstructor) {
  const Message msg;
  EXPECT_EQ("", msg.GetString());
}

// Tests the copy constructor.
TEST(MessageTest, CopyConstructor) {
  const Message msg1("Hello");
  const Message msg2(msg1);
  EXPECT_EQ("Hello", msg2.GetString());
}

// Tests constructing a Message from a C-string.
TEST(MessageTest, ConstructsFromCString) {
  Message msg("Hello");
  EXPECT_EQ("Hello", msg.GetString());
}

// Tests streaming a float.
TEST(MessageTest, StreamsFloat) {
  const std::string s = (Message() << 1.23456F << " " << 2.34567F).GetString();
  // Both numbers should be printed with enough precision.
  EXPECT_PRED_FORMAT2(::jmsd::cutf::Substring_assertions::IsSubstring, "1.234560", s.c_str());
  EXPECT_PRED_FORMAT2(::jmsd::cutf::Substring_assertions::IsSubstring, " 2.345669", s.c_str());
}

// Tests streaming a double.
TEST(MessageTest, StreamsDouble) {
  const std::string s = (Message() << 1260570880.4555497 << " "
                                  << 1260572265.1954534).GetString();
  // Both numbers should be printed with enough precision.
  EXPECT_PRED_FORMAT2(::jmsd::cutf::Substring_assertions::IsSubstring, "1260570880.45", s.c_str());
  EXPECT_PRED_FORMAT2(::jmsd::cutf::Substring_assertions::IsSubstring, " 1260572265.19", s.c_str());
}

// Tests streaming a non-char pointer.
TEST(MessageTest, StreamsPointer) {
  int n = 0;
  int* p = &n;
  EXPECT_NE("(null)", (Message() << p).GetString());
}

// Tests streaming a NULL non-char pointer.
TEST(MessageTest, StreamsNullPointer) {
  int* p = nullptr;
  EXPECT_EQ("(null)", (Message() << p).GetString());
}

// Tests streaming a C string.
TEST(MessageTest, StreamsCString) {
  EXPECT_EQ("Foo", (Message() << "Foo").GetString());
}

// Tests streaming a NULL C string.
TEST(MessageTest, StreamsNullCString) {
  char* p = nullptr;
  EXPECT_EQ("(null)", (Message() << p).GetString());
}

// Tests streaming std::string.
TEST(MessageTest, StreamsString) {
  const ::std::string str("Hello");
  EXPECT_EQ("Hello", (Message() << str).GetString());
}

// Tests that we can output strings containing embedded NULs.
TEST(MessageTest, StreamsStringWithEmbeddedNUL) {
  const char char_array_with_nul[] =
      "Here's a NUL\0 and some more string";
  const ::std::string string_with_nul(char_array_with_nul,
                                      sizeof(char_array_with_nul) - 1);
  EXPECT_EQ("Here's a NUL\\0 and some more string",
            (Message() << string_with_nul).GetString());
}

// Tests streaming a NUL char.
TEST(MessageTest, StreamsNULChar) {
  EXPECT_EQ("\\0", (Message() << '\0').GetString());
}

// Tests streaming int.
TEST(MessageTest, StreamsInt) {
  EXPECT_EQ("123", (Message() << 123).GetString());
}

// Tests that basic IO manipulators (endl, ends, and flush) can be
// streamed to Message.
TEST(MessageTest, StreamsBasicIoManip) {
  EXPECT_EQ("Line 1.\nA NUL char \\0 in line 2.",
               (Message() << "Line 1." << std::endl
                         << "A NUL char " << std::ends << std::flush
                         << " in line 2.").GetString());
}

// Tests Message::GetString()
TEST(MessageTest, GetString) {
  Message msg;
  msg << 1 << " lamb";
  EXPECT_EQ("1 lamb", msg.GetString());
}

// Tests streaming a Message object to an ostream.
TEST(MessageTest, StreamsToOStream) {
  Message msg("Hello");
  ::std::stringstream ss;
  ss << msg;
  EXPECT_EQ("Hello", ::jmsd::cutf::internal::function_String_stream_to_string::StringStreamToString( ss ) );
}

// Tests that a Message object doesn't take up too much stack space.
TEST(MessageTest, DoesNotTakeUpMuchStackSpace) {
  EXPECT_LE(sizeof(Message), 16U);
}

}  // namespace
