#pragma once

// Utility functions and classes used by the Google C++ testing framework.//
// This file contains purely Google Test's internal implementation.  Please
// DO NOT #INCLUDE IT IN A USER PROGRAM.


#include "Test_property.h"
#include "Test_suite.h"

#include "gtest-test-part.h"
#include "gtest-death-test.h"
#include "gtest-flags.h"

#include "gtest/gtest.h"
#include "gtest/gtest-spi.h"

#include "internal/gtest-port.h"

#ifndef _WIN32_WCE
# include <errno.h>
#endif  // !_WIN32_WCE
#include <stddef.h>
#include <stdlib.h>  // For strtoll/_strtoul64/malloc/free.
#include <string.h>  // For memmove.

#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>


#if GTEST_CAN_STREAM_RESULTS_
# include <arpa/inet.h>  // NOLINT
# include <netdb.h>  // NOLINT
#endif

#if GTEST_OS_WINDOWS
# include "internal/custom/temporary_windows_include.h"
#endif  // GTEST_OS_WINDOWS

GTEST_DISABLE_MSC_WARNINGS_PUSH_(4251 \
/* class A needs to have dll-interface to be used by clients of class B */)

namespace testing {

// Declares the flags.
//
// We don't want the users to modify this flag in the code, but want
// Google Test's own unit tests to be able to access it. Therefore we
// declare it here as opposed to in gtest.h.
GTEST_DECLARE_FLAG_bool_(death_test_use_fork);

namespace internal {

// The value of GetTestTypeId() as seen from within the Google Test
// library.  This is solely for testing GetTestTypeId().
JMSD_DEPRECATED_GTEST_API_ extern const TypeId kTestTypeIdInGoogleTest;

// Names of the flags (needed for parsing Google Test flags).
const char kAlsoRunDisabledTestsFlag[] = "also_run_disabled_tests";
const char kBreakOnFailureFlag[] = "break_on_failure";
const char kCatchExceptionsFlag[] = "catch_exceptions";
const char kColorFlag[] = "color";
const char kFilterFlag[] = "filter";
const char kListTestsFlag[] = "list_tests";
const char kOutputFlag[] = "output";
const char kPrintTimeFlag[] = "print_time";
const char kPrintUTF8Flag[] = "print_utf8";
const char kRandomSeedFlag[] = "random_seed";
const char kRepeatFlag[] = "repeat";
const char kShuffleFlag[] = "shuffle";
const char kStackTraceDepthFlag[] = "stack_trace_depth";
const char kStreamResultToFlag[] = "stream_result_to";
const char kThrowOnFailureFlag[] = "throw_on_failure";
const char kFlagfileFlag[] = "flagfile";

// A valid random seed must be in [1, kMaxRandomSeed].
const int kMaxRandomSeed = 99999;

// Returns the current time in milliseconds.
JMSD_DEPRECATED_GTEST_API_ TimeInMillis GetTimeInMillis();

// Parses a string for an Int32 flag, in the form of "--flag=value".
//
// On success, stores the value of the flag in *value, and returns
// true.  On failure, returns false without changing *value.
JMSD_DEPRECATED_GTEST_API_ bool ParseInt32Flag(
	const char* str, const char* flag, int32_t* value);

// Returns a random seed in range [1, kMaxRandomSeed] based on the
// given --gtest_random_seed flag value.
inline int GetRandomSeedFromFlag(int32_t random_seed_flag) {
  const unsigned int raw_seed = (random_seed_flag == 0) ?
	  static_cast<unsigned int>(GetTimeInMillis()) :
	  static_cast<unsigned int>(random_seed_flag);

  // Normalizes the actual seed to range [1, kMaxRandomSeed] such that
  // it's easy to type.
  const int normalized_seed =
	  static_cast<int>((raw_seed - 1U) %
					   static_cast<unsigned int>(kMaxRandomSeed)) + 1;
  return normalized_seed;
}

// Returns the first valid random seed after 'seed'.  The behavior is
// undefined if 'seed' is invalid.  The seed after kMaxRandomSeed is
// considered to be 1.
inline int GetNextRandomSeed(int seed) {
  GTEST_CHECK_(1 <= seed && seed <= kMaxRandomSeed)
	  << "Invalid random seed " << seed << " - must be in [1, "
	  << kMaxRandomSeed << "].";
  const int next_seed = seed + 1;
  return (next_seed > kMaxRandomSeed) ? 1 : next_seed;
}

// This class saves the values of all Google Test flags in its c'tor, and
// restores them in its d'tor.
class GTestFlagSaver {
 public:
  // The c'tor.
  GTestFlagSaver() {
	also_run_disabled_tests_ = GTEST_FLAG(also_run_disabled_tests);
	break_on_failure_ = GTEST_FLAG(break_on_failure);
	catch_exceptions_ = GTEST_FLAG(catch_exceptions);
	color_ = GTEST_FLAG(color);
	death_test_style_ = GTEST_FLAG(death_test_style);
	death_test_use_fork_ = GTEST_FLAG(death_test_use_fork);
	filter_ = GTEST_FLAG(filter);
	internal_run_death_test_ = GTEST_FLAG(internal_run_death_test);
	list_tests_ = GTEST_FLAG(list_tests);
	output_ = GTEST_FLAG(output);
	print_time_ = GTEST_FLAG(print_time);
	print_utf8_ = GTEST_FLAG(print_utf8);
	random_seed_ = GTEST_FLAG(random_seed);
	repeat_ = GTEST_FLAG(repeat);
	shuffle_ = GTEST_FLAG(shuffle);
	stack_trace_depth_ = GTEST_FLAG(stack_trace_depth);
	stream_result_to_ = GTEST_FLAG(stream_result_to);
	throw_on_failure_ = GTEST_FLAG(throw_on_failure);
  }

  // The d'tor is not virtual.  DO NOT INHERIT FROM THIS CLASS.
  ~GTestFlagSaver() {
	GTEST_FLAG(also_run_disabled_tests) = also_run_disabled_tests_;
	GTEST_FLAG(break_on_failure) = break_on_failure_;
	GTEST_FLAG(catch_exceptions) = catch_exceptions_;
	GTEST_FLAG(color) = color_;
	GTEST_FLAG(death_test_style) = death_test_style_;
	GTEST_FLAG(death_test_use_fork) = death_test_use_fork_;
	GTEST_FLAG(filter) = filter_;
	GTEST_FLAG(internal_run_death_test) = internal_run_death_test_;
	GTEST_FLAG(list_tests) = list_tests_;
	GTEST_FLAG(output) = output_;
	GTEST_FLAG(print_time) = print_time_;
	GTEST_FLAG(print_utf8) = print_utf8_;
	GTEST_FLAG(random_seed) = random_seed_;
	GTEST_FLAG(repeat) = repeat_;
	GTEST_FLAG(shuffle) = shuffle_;
	GTEST_FLAG(stack_trace_depth) = stack_trace_depth_;
	GTEST_FLAG(stream_result_to) = stream_result_to_;
	GTEST_FLAG(throw_on_failure) = throw_on_failure_;
  }

 private:
  // Fields for saving the original values of flags.
  bool also_run_disabled_tests_;
  bool break_on_failure_;
  bool catch_exceptions_;
  std::string color_;
  std::string death_test_style_;
  bool death_test_use_fork_;
  std::string filter_;
  std::string internal_run_death_test_;
  bool list_tests_;
  std::string output_;
  bool print_time_;
  bool print_utf8_;
  int32_t random_seed_;
  int32_t repeat_;
  bool shuffle_;
  int32_t stack_trace_depth_;
  std::string stream_result_to_;
  bool throw_on_failure_;
} GTEST_ATTRIBUTE_UNUSED_;

// A predicate that checks the key of a TestProperty against a known key.
//
// TestPropertyKeyIs is copyable.
class TestPropertyKeyIs {
 public:
  // Constructor.
  //
  // TestPropertyKeyIs has NO default constructor.
  explicit TestPropertyKeyIs(const std::string& key) : key_(key) {}

  // Returns true if and only if the test name of test property matches on key_.
  bool operator()(const ::jmsd::cutf::TestProperty& test_property) const {
	return test_property.key() == key_;
  }

 private:
  std::string key_;
};


// Returns the current application's name, removing directory path if that
// is present.  Used by UnitTestOptions::GetOutputFile.
JMSD_DEPRECATED_GTEST_API_ FilePath GetCurrentExecutableName();

// The role interface for getting the OS stack trace as a string.
class OsStackTraceGetterInterface {
 public:
  OsStackTraceGetterInterface() {}
  virtual ~OsStackTraceGetterInterface() {}

  // Returns the current OS stack trace as an std::string.  Parameters:
  //
  //   max_depth  - the maximum number of stack frames to be included
  //                in the trace.
  //   skip_count - the number of top frames to be skipped; doesn't count
  //                against max_depth.
  virtual std::string CurrentStackTrace(int max_depth, int skip_count) = 0;

  // UponLeavingGTest() should be called immediately before Google Test calls
  // user code. It saves some information about the current stack that
  // CurrentStackTrace() will use to find and hide Google Test stack frames.
  virtual void UponLeavingGTest() = 0;

  // This string is inserted in place of stack frames that are part of
  // Google Test's implementation.
  static const char* const kElidedFramesMarker;

 private:
  GTEST_DISALLOW_COPY_AND_ASSIGN_(OsStackTraceGetterInterface);
};

// A working implementation of the OsStackTraceGetterInterface interface.
class OsStackTraceGetter : public OsStackTraceGetterInterface {
 public:
  OsStackTraceGetter() {}

  std::string CurrentStackTrace(int max_depth, int skip_count) override;
  void UponLeavingGTest() override;

 private:
#if GTEST_HAS_ABSL
  Mutex mutex_;  // Protects all internal state.

  // We save the stack frame below the frame that calls user code.
  // We do this because the address of the frame immediately below
  // the user code changes between the call to UponLeavingGTest()
  // and any calls to the stack trace code from within the user code.
  void* caller_frame_ = nullptr;
#endif  // GTEST_HAS_ABSL

  GTEST_DISALLOW_COPY_AND_ASSIGN_(OsStackTraceGetter);
};

// Information about a Google Test trace point.
struct TraceInfo {
  const char* file;
  int line;
  std::string message;
};

#if GTEST_USES_SIMPLE_RE

// Internal helper functions for implementing the simple regular
// expression matcher.
JMSD_DEPRECATED_GTEST_API_ bool IsInSet(char ch, const char* str);
JMSD_DEPRECATED_GTEST_API_ bool IsAsciiDigit(char ch);
JMSD_DEPRECATED_GTEST_API_ bool IsAsciiPunct(char ch);
JMSD_DEPRECATED_GTEST_API_ bool IsRepeat(char ch);
JMSD_DEPRECATED_GTEST_API_ bool IsAsciiWhiteSpace(char ch);
JMSD_DEPRECATED_GTEST_API_ bool IsAsciiWordChar(char ch);
JMSD_DEPRECATED_GTEST_API_ bool IsValidEscape(char ch);
JMSD_DEPRECATED_GTEST_API_ bool AtomMatchesChar(bool escaped, char pattern, char ch);
JMSD_DEPRECATED_GTEST_API_ bool ValidateRegex(const char* regex);
JMSD_DEPRECATED_GTEST_API_ bool MatchRegexAtHead(const char* regex, const char* str);
JMSD_DEPRECATED_GTEST_API_ bool MatchRepetitionAndRegexAtHead(
	bool escaped, char ch, char repeat, const char* regex, const char* str);
JMSD_DEPRECATED_GTEST_API_ bool MatchRegexAnywhere(const char* regex, const char* str);

#endif  // GTEST_USES_SIMPLE_RE

// Parses the command line for Google Test flags, without initializing
// other parts of Google Test.
JMSD_DEPRECATED_GTEST_API_ void ParseGoogleTestFlagsOnly(int* argc, char** argv);
JMSD_DEPRECATED_GTEST_API_ void ParseGoogleTestFlagsOnly(int* argc, wchar_t** argv);

#if GTEST_HAS_DEATH_TEST

// Attempts to parse a string into a positive integer pointed to by the
// number parameter.  Returns true if that is possible.
// GTEST_HAS_DEATH_TEST implies that we have ::std::string, so we can use
// it here.
template <typename Integer>
bool ParseNaturalNumber(const ::std::string& str, Integer* number) {
  // Fail fast if the given string does not begin with a digit;
  // this bypasses strtoXXX's "optional leading whitespace and plus
  // or minus sign" semantics, which are undesirable here.
  if (str.empty() || !IsDigit(str[0])) {
	return false;
  }
  errno = 0;

  char* end;
  // BiggestConvertible is the largest integer type that system-provided
  // string-to-number conversion routines can return.
  using BiggestConvertible = unsigned long long;  // NOLINT

  const BiggestConvertible parsed = strtoull(str.c_str(), &end, 10);  // NOLINT
  const bool parse_success = *end == '\0' && errno == 0;

  GTEST_CHECK_(sizeof(Integer) <= sizeof(parsed));

  const Integer result = static_cast<Integer>(parsed);
  if (parse_success && static_cast<BiggestConvertible>(result) == parsed) {
	*number = result;
	return true;
  }
  return false;
}
#endif  // GTEST_HAS_DEATH_TEST


}  // namespace internal
}  // namespace testing

GTEST_DISABLE_MSC_WARNINGS_POP_()  //  4251
