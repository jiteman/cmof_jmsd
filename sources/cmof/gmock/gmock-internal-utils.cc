// This file defines some utilities useful for implementing Google
// Mock.  They are subject to change without notice, so please DO NOT
// USE THEM IN USER CODE.

#include "gmock/internal/gmock-internal-utils.h"

#include <ctype.h>
#include <ostream>
#include <string>
#include "gmock/gmock.h"
#include "gmock/internal/gmock-port.h"
#include "gtest/gtest.h"

#include "gtest/Message.hin"

namespace testing {
namespace internal {

// Joins a vector of strings as if they are fields of a tuple; returns
// the joined string.
JMSD_DEPRECATED_GMOCK_API_ std::string JoinAsTuple(const Strings& fields) {
  switch (fields.size()) {
    case 0:
      return "";
    case 1:
      return fields[0];
    default:
      std::string result = "(" + fields[0];
      for (size_t i = 1; i < fields.size(); i++) {
        result += ", ";
        result += fields[i];
      }
      result += ")";
      return result;
  }
}

// Converts an identifier name to a space-separated list of lower-case
// words.  Each maximum substring of the form [A-Za-z][a-z]*|\d+ is
// treated as one word.  For example, both "FooBar123" and
// "foo_bar_123" are converted to "foo bar 123".
JMSD_DEPRECATED_GMOCK_API_ std::string ConvertIdentifierNameToWords(const char* id_name) {
  std::string result;
  char prev_char = '\0';
  for (const char* p = id_name; *p != '\0'; prev_char = *(p++)) {
    // We don't care about the current locale as the input is
    // guaranteed to be a valid C++ identifier name.
    const bool starts_new_word = IsUpper(*p) ||
        (!IsAlpha(prev_char) && IsLower(*p)) ||
        (!IsDigit(prev_char) && IsDigit(*p));

    if (IsAlNum(*p)) {
      if (starts_new_word && result != "")
        result += ' ';
      result += ToLower(*p);
    }
  }
  return result;
}

// This class reports Google Mock failures as Google Test failures.  A
// user can define another class in a similar fashion if they intend to
// use Google Mock with a testing framework other than Google Test.
class GoogleTestFailureReporter : public FailureReporterInterface {
 public:
  void ReportFailure(FailureType type, const char* file, int line,
                     const std::string& message) override {
    AssertHelper(type == kFatal ?
                 TestPartResult::kFatalFailure :
                 TestPartResult::kNonFatalFailure,
                 file,
                 line,
                 message.c_str()) = ::jmsd::cutf::Message();
    if (type == kFatal) {
      posix::Abort();
    }
  }
};

// Returns the global failure reporter.  Will create a
// GoogleTestFailureReporter and return it the first time called.
JMSD_DEPRECATED_GMOCK_API_ FailureReporterInterface* GetFailureReporter() {
  // Points to the global failure reporter used by Google Mock.  gcc
  // guarantees that the following use of failure_reporter is
  // thread-safe.  We may need to add additional synchronization to
  // protect failure_reporter if we port Google Mock to other
  // compilers.
  static FailureReporterInterface* const failure_reporter =
      new GoogleTestFailureReporter();
  return failure_reporter;
}

// Protects global resources (stdout in particular) used by Log().
static GTEST_DEFINE_STATIC_MUTEX_(g_log_mutex);

// Returns true if and only if a log with the given severity is visible
// according to the --gmock_verbose flag.
JMSD_DEPRECATED_GMOCK_API_ bool LogIsVisible(LogSeverity severity) {
  if (GMOCK_FLAG(verbose) == kInfoVerbosity) {
    // Always show the log if --gmock_verbose=info.
    return true;
  } else if (GMOCK_FLAG(verbose) == kErrorVerbosity) {
    // Always hide it if --gmock_verbose=error.
    return false;
  } else {
    // If --gmock_verbose is neither "info" nor "error", we treat it
    // as "warning" (its default value).
    return severity == kWarning;
  }
}

// Prints the given message to stdout if and only if 'severity' >= the level
// specified by the --gmock_verbose flag.  If stack_frames_to_skip >=
// 0, also prints the stack trace excluding the top
// stack_frames_to_skip frames.  In opt mode, any positive
// stack_frames_to_skip is treated as 0, since we don't know which
// function calls will be inlined by the compiler and need to be
// conservative.
JMSD_DEPRECATED_GMOCK_API_ void Log(LogSeverity severity, const std::string& message,
                    int stack_frames_to_skip) {
  if (!LogIsVisible(severity))
    return;

  // Ensures that logs from different threads don't interleave.
  MutexLock l(&g_log_mutex);

  if (severity == kWarning) {
    // Prints a GMOCK WARNING marker to make the warnings easily searchable.
    std::cout << "\nGMOCK WARNING:";
  }
  // Pre-pends a new-line to message if it doesn't start with one.
  if (message.empty() || message[0] != '\n') {
    std::cout << "\n";
  }
  std::cout << message;
  if (stack_frames_to_skip >= 0) {
#ifdef NDEBUG
    // In opt mode, we have to be conservative and skip no stack frame.
    const int actual_to_skip = 0;
#else
    // In dbg mode, we can do what the caller tell us to do (plus one
    // for skipping this function's stack frame).
    const int actual_to_skip = stack_frames_to_skip + 1;
#endif  // NDEBUG

    // Appends a new-line to message if it doesn't end with one.
    if (!message.empty() && *message.rbegin() != '\n') {
      std::cout << "\n";
    }
    std::cout << "Stack trace:\n"
         << ::testing::internal::GetCurrentOsStackTraceExceptTop(
             ::jmsd::cutf::UnitTest::GetInstance(), actual_to_skip);
  }
  std::cout << ::std::flush;
}

JMSD_DEPRECATED_GMOCK_API_ WithoutMatchers GetWithoutMatchers() { return WithoutMatchers(); }

JMSD_DEPRECATED_GMOCK_API_ void IllegalDoDefault(const char* file, int line) {
  internal::Assert(
      false, file, line,
      "You are using DoDefault() inside a composite action like "
      "DoAll() or WithArgs().  This is not supported for technical "
      "reasons.  Please instead spell out the default action, or "
      "assign the default action to an Action variable and use "
      "the variable in various places.");
}

}  // namespace internal
}  // namespace testing
