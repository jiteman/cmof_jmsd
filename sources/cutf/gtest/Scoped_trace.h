#pragma once

#include "Scoped_trace.hxx"


#include "Message.h"

#include "gtest/internal/gtest-port.h"


namespace jmsd {
namespace cutf {


// Causes a trace (including the given source file path and line number,
// and the given message) to be included in every test failure message generated
// by code in the scope of the lifetime of an instance of this class. The effect
// is undone with the destruction of the instance.
//
// The message argument can be anything streamable to std::ostream.
//
// Example:
//   testing::ScopedTrace trace("file.cc", 123, "message");
//
class JMSD_DEPRECATED_GTEST_API_ ScopedTrace {
 public:
  // The c'tor pushes the given source file location and message onto
  // a trace stack maintained by Google Test.

  // Template version. Uses Message() to convert the values into strings.
  // Slow, but flexible.
  template <typename T>
  ScopedTrace(const char* file, int line, const T& message) {
	PushTrace(file, line, (::jmsd::cutf::Message() << message).GetString());
  }

  // Optimize for some known types.
  ScopedTrace(const char* file, int line, const char* message) {
	PushTrace(file, line, message ? message : "(null)");
  }

  ScopedTrace(const char* file, int line, const std::string& message) {
	PushTrace(file, line, message);
  }

  // The d'tor pops the info pushed by the c'tor.
  //
  // Note that the d'tor is not virtual in order to be efficient.
  // Don't inherit from ScopedTrace!
  ~ScopedTrace();

 private:
  void PushTrace(const char* file, int line, std::string message);

  GTEST_DISALLOW_COPY_AND_ASSIGN_(ScopedTrace);
} GTEST_ATTRIBUTE_UNUSED_;  // A ScopedTrace object does its job in its
							// c'tor and d'tor.  Therefore it doesn't
							// need to be used otherwise.


} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
