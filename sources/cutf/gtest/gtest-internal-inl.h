#pragma once

// Utility functions and classes used by the Google C++ testing framework.//
// This file contains purely Google Test's internal implementation.  Please
// DO NOT #INCLUDE IT IN A USER PROGRAM.


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


#include "Test_property.h"


#include "gtest-test-part.h"
#include "gtest-death-test.h"
#include "gtest-flags.h"

#include "internal/gtest-port.h"

#if GTEST_CAN_STREAM_RESULTS_
# include <arpa/inet.h>  // NOLINT
# include <netdb.h>  // NOLINT
#endif

#if GTEST_OS_WINDOWS
# include <windows.h>  // NOLINT
#endif  // GTEST_OS_WINDOWS

#include "gtest/gtest.h"
#include "gtest/gtest-spi.h"

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
GTEST_API_ extern const TypeId kTestTypeIdInGoogleTest;

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

// g_help_flag is true if and only if the --help flag or an equivalent form
// is specified on the command line.
GTEST_API_ extern bool g_help_flag;

// Returns the current time in milliseconds.
GTEST_API_ TimeInMillis GetTimeInMillis();

// Returns true if and only if Google Test should use colors in the output.
GTEST_API_ bool ShouldUseColor(bool stdout_is_tty);

// Formats the given time in milliseconds as seconds.
GTEST_API_ std::string FormatTimeInMillisAsSeconds(TimeInMillis ms);

// Converts the given time in milliseconds to a date string in the ISO 8601
// format, without the timezone information.  N.B.: due to the use the
// non-reentrant localtime() function, this function is not thread safe.  Do
// not use it in any code that can be called from multiple threads.
GTEST_API_ std::string FormatEpochTimeInMillisAsIso8601(TimeInMillis ms);

// Parses a string for an Int32 flag, in the form of "--flag=value".
//
// On success, stores the value of the flag in *value, and returns
// true.  On failure, returns false without changing *value.
GTEST_API_ bool ParseInt32Flag(
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

// Converts a Unicode code point to a narrow string in UTF-8 encoding.
// code_point parameter is of type UInt32 because wchar_t may not be
// wide enough to contain a code point.
// If the code_point is not a valid Unicode code point
// (i.e. outside of Unicode range U+0 to U+10FFFF) it will be converted
// to "(Invalid Unicode 0xXXXXXXXX)".
GTEST_API_ std::string CodePointToUtf8(uint32_t code_point);

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
GTEST_API_ std::string WideStringToUtf8(const wchar_t* str, int num_chars);

// Reads the GTEST_SHARD_STATUS_FILE environment variable, and creates the file
// if the variable is present. If a file already exists at this location, this
// function will write over it. If the variable is present, but the file cannot
// be created, prints an error and exits.
void WriteToShardStatusFileIfNeeded();

// Checks whether sharding is enabled by examining the relevant
// environment variable values. If the variables are present,
// but inconsistent (e.g., shard_index >= total_shards), prints
// an error and exits. If in_subprocess_for_death_test, sharding is
// disabled because it must only be applied to the original test
// process. Otherwise, we could filter out death tests we intended to execute.
GTEST_API_ bool ShouldShard(const char* total_shards_str,
							const char* shard_index_str,
							bool in_subprocess_for_death_test);

// Parses the environment variable var as a 32-bit integer. If it is unset,
// returns default_val. If it is not a 32-bit integer, prints an error and
// and aborts.
GTEST_API_ int32_t Int32FromEnvOrDie(const char* env_var, int32_t default_val);

// Given the total number of shards, the shard index, and the test id,
// returns true if and only if the test should be run on this shard. The test id
// is some arbitrary but unique non-negative integer assigned to each test
// method. Assumes that 0 <= shard_index < total_shards.
GTEST_API_ bool ShouldRunTestOnShard(
	int total_shards, int shard_index, int test_id);

// STL container utilities.

// Returns the number of elements in the given container that satisfy
// the given predicate.
template <class Container, typename Predicate>
inline int CountIf(const Container& c, Predicate predicate) {
  // Implemented as an explicit loop since std::count_if() in libCstd on
  // Solaris has a non-standard signature.
  int count = 0;
  for (typename Container::const_iterator it = c.begin(); it != c.end(); ++it) {
	if (predicate(*it))
	  ++count;
  }
  return count;
}

// Applies a function/functor to each element in the container.
template <class Container, typename Functor>
void ForEach(const Container& c, Functor functor) {
  std::for_each(c.begin(), c.end(), functor);
}

// Returns the i-th element of the vector, or default_value if i is not
// in range [0, v.size()).
template <typename E>
inline E GetElementOr(const std::vector<E>& v, int i, E default_value) {
  return (i < 0 || i >= static_cast<int>(v.size())) ? default_value
													: v[static_cast<size_t>(i)];
}

// Performs an in-place shuffle of a range of the vector's elements.
// 'begin' and 'end' are element indices as an STL-style range;
// i.e. [begin, end) are shuffled, where 'end' == size() means to
// shuffle to the end of the vector.
template <typename E>
void ShuffleRange(internal::Random* random, int begin, int end,
				  std::vector<E>* v) {
  const int size = static_cast<int>(v->size());
  GTEST_CHECK_(0 <= begin && begin <= size)
	  << "Invalid shuffle range start " << begin << ": must be in range [0, "
	  << size << "].";
  GTEST_CHECK_(begin <= end && end <= size)
	  << "Invalid shuffle range finish " << end << ": must be in range ["
	  << begin << ", " << size << "].";

  // Fisher-Yates shuffle, from
  // http://en.wikipedia.org/wiki/Fisher-Yates_shuffle
  for (int range_width = end - begin; range_width >= 2; range_width--) {
	const int last_in_range = begin + range_width - 1;
	const int selected =
		begin +
		static_cast<int>(random->Generate(static_cast<uint32_t>(range_width)));
	std::swap((*v)[static_cast<size_t>(selected)],
			  (*v)[static_cast<size_t>(last_in_range)]);
  }
}

// Performs an in-place shuffle of the vector's elements.
template <typename E>
inline void Shuffle(internal::Random* random, std::vector<E>* v) {
  ShuffleRange(random, 0, static_cast<int>(v->size()), v);
}

// A function for deleting an object.  Handy for being used as a
// functor.
template <typename T>
static void Delete(T* x) {
  delete x;
}

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

// Class UnitTestOptions.
//
// This class contains functions for processing options the user
// specifies when running the tests.  It has only static members.
//
// In most cases, the user can specify an option using either an
// environment variable or a command line flag.  E.g. you can set the
// test filter using either GTEST_FILTER or --gtest_filter.  If both
// the variable and the flag are present, the latter overrides the
// former.
class GTEST_API_ UnitTestOptions {
 public:
  // Functions for processing the gtest_output flag.

  // Returns the output format, or "" for normal printed output.
  static std::string GetOutputFormat();

  // Returns the absolute path of the requested output file, or the
  // default (test_detail.xml in the original working directory) if
  // none was explicitly specified.
  static std::string GetAbsolutePathToOutputFile();

  // Functions for processing the gtest_filter flag.

  // Returns true if and only if the wildcard pattern matches the string.
  // The first ':' or '\0' character in pattern marks the end of it.
  //
  // This recursive algorithm isn't very efficient, but is clear and
  // works well enough for matching test names, which are short.
  static bool PatternMatchesString(const char *pattern, const char *str);

  // Returns true if and only if the user-specified filter matches the test
  // suite name and the test name.
  static bool FilterMatchesTest(const std::string& test_suite_name,
								const std::string& test_name);

#if GTEST_OS_WINDOWS
  // Function for supporting the gtest_catch_exception flag.

  // Returns EXCEPTION_EXECUTE_HANDLER if Google Test should handle the
  // given SEH exception, or EXCEPTION_CONTINUE_SEARCH otherwise.
  // This function is useful as an __except condition.
  static int GTestShouldProcessSEH(DWORD exception_code);
#endif  // GTEST_OS_WINDOWS

  // Returns true if "name" matches the ':' separated list of glob-style
  // filters in "filter".
  static bool MatchesFilter(const std::string& name, const char* filter);
};

// Returns the current application's name, removing directory path if that
// is present.  Used by UnitTestOptions::GetOutputFile.
GTEST_API_ FilePath GetCurrentExecutableName();

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

//// This is the default global test part result reporter used in UnitTestImpl.
//// This class should only be used by UnitTestImpl.
//class DefaultGlobalTestPartResultReporter
//  : public TestPartResultReporterInterface {
// public:
//  explicit DefaultGlobalTestPartResultReporter(UnitTestImpl* unit_test);
//  // Implements the TestPartResultReporterInterface. Reports the test part
//  // result in the current test.
//  void ReportTestPartResult(const TestPartResult& result) override;

// private:
//  UnitTestImpl* const unit_test_;

//  GTEST_DISALLOW_COPY_AND_ASSIGN_(DefaultGlobalTestPartResultReporter);
//};

// This is the default per thread test part result reporter used in
// UnitTestImpl. This class should only be used by UnitTestImpl.
class DefaultPerThreadTestPartResultReporter
	: public TestPartResultReporterInterface {
 public:
  explicit DefaultPerThreadTestPartResultReporter(UnitTestImpl* unit_test);
  // Implements the TestPartResultReporterInterface. The implementation just
  // delegates to the current global test part result reporter of *unit_test_.
  void ReportTestPartResult(const TestPartResult& result) override;

 private:
  UnitTestImpl* const unit_test_;

  GTEST_DISALLOW_COPY_AND_ASSIGN_(DefaultPerThreadTestPartResultReporter);
};

#if GTEST_USES_SIMPLE_RE

// Internal helper functions for implementing the simple regular
// expression matcher.
GTEST_API_ bool IsInSet(char ch, const char* str);
GTEST_API_ bool IsAsciiDigit(char ch);
GTEST_API_ bool IsAsciiPunct(char ch);
GTEST_API_ bool IsRepeat(char ch);
GTEST_API_ bool IsAsciiWhiteSpace(char ch);
GTEST_API_ bool IsAsciiWordChar(char ch);
GTEST_API_ bool IsValidEscape(char ch);
GTEST_API_ bool AtomMatchesChar(bool escaped, char pattern, char ch);
GTEST_API_ bool ValidateRegex(const char* regex);
GTEST_API_ bool MatchRegexAtHead(const char* regex, const char* str);
GTEST_API_ bool MatchRepetitionAndRegexAtHead(
	bool escaped, char ch, char repeat, const char* regex, const char* str);
GTEST_API_ bool MatchRegexAnywhere(const char* regex, const char* str);

#endif  // GTEST_USES_SIMPLE_RE

// Parses the command line for Google Test flags, without initializing
// other parts of Google Test.
GTEST_API_ void ParseGoogleTestFlagsOnly(int* argc, char** argv);
GTEST_API_ void ParseGoogleTestFlagsOnly(int* argc, wchar_t** argv);

#if GTEST_HAS_DEATH_TEST

// Returns the message describing the last system error, regardless of the
// platform.
GTEST_API_ std::string GetLastErrnoDescription();

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

//// TestResult contains some private methods that should be hidden from
//// Google Test user but are required for testing. This class allow our tests
//// to access them.
////
//// This class is supplied only for the purpose of testing Google Test's own
//// constructs. Do not use it in user tests, either directly or indirectly.
//class TestResultAccessor {
// public:
//  static void RecordProperty(TestResult* test_result,
//							 const std::string& xml_element,
//							 const ::jmsd::cutf::TestProperty& property) {
//	test_result->RecordProperty(xml_element, property);
//  }

//  static void ClearTestPartResults(TestResult* test_result) {
//	test_result->ClearTestPartResults();
//  }

//  static const std::vector<testing::TestPartResult>& test_part_results(
//	  const TestResult& test_result) {
//	return test_result.test_part_results();
//  }
//};

#if GTEST_CAN_STREAM_RESULTS_

// Streams test results to the given port on the given host machine.
class StreamingListener : public EmptyTestEventListener {
 public:
  // Abstract base class for writing strings to a socket.
  class AbstractSocketWriter {
   public:
	virtual ~AbstractSocketWriter() {}

	// Sends a string to the socket.
	virtual void Send(const std::string& message) = 0;

	// Closes the socket.
	virtual void CloseConnection() {}

	// Sends a string and a newline to the socket.
	void SendLn(const std::string& message) { Send(message + "\n"); }
  };

  // Concrete class for actually writing strings to a socket.
  class SocketWriter : public AbstractSocketWriter {
   public:
	SocketWriter(const std::string& host, const std::string& port)
		: sockfd_(-1), host_name_(host), port_num_(port) {
	  MakeConnection();
	}

	~SocketWriter() override {
	  if (sockfd_ != -1)
		CloseConnection();
	}

	// Sends a string to the socket.
	void Send(const std::string& message) override {
	  GTEST_CHECK_(sockfd_ != -1)
		  << "Send() can be called only when there is a connection.";

	  const auto len = static_cast<size_t>(message.length());
	  if (write(sockfd_, message.c_str(), len) != static_cast<ssize_t>(len)) {
		GTEST_LOG_(WARNING)
			<< "stream_result_to: failed to stream to "
			<< host_name_ << ":" << port_num_;
	  }
	}

   private:
	// Creates a client socket and connects to the server.
	void MakeConnection();

	// Closes the socket.
	void CloseConnection() override {
	  GTEST_CHECK_(sockfd_ != -1)
		  << "CloseConnection() can be called only when there is a connection.";

	  close(sockfd_);
	  sockfd_ = -1;
	}

	int sockfd_;  // socket file descriptor
	const std::string host_name_;
	const std::string port_num_;

	GTEST_DISALLOW_COPY_AND_ASSIGN_(SocketWriter);
  };  // class SocketWriter

  // Escapes '=', '&', '%', and '\n' characters in str as "%xx".
  static std::string UrlEncode(const char* str);

  StreamingListener(const std::string& host, const std::string& port)
	  : socket_writer_(new SocketWriter(host, port)) {
	Start();
  }

  explicit StreamingListener(AbstractSocketWriter* socket_writer)
	  : socket_writer_(socket_writer) { Start(); }

  void OnTestProgramStart(const UnitTest& /* unit_test */) override {
	SendLn("event=TestProgramStart");
  }

  void OnTestProgramEnd(const UnitTest& unit_test) override {
	// Note that Google Test current only report elapsed time for each
	// test iteration, not for the entire test program.
	SendLn("event=TestProgramEnd&passed=" + FormatBool(unit_test.Passed()));

	// Notify the streaming server to stop.
	socket_writer_->CloseConnection();
  }

  void OnTestIterationStart(const UnitTest& /* unit_test */,
							int iteration) override {
	SendLn("event=TestIterationStart&iteration=" +
		   StreamableToString(iteration));
  }

  void OnTestIterationEnd(const UnitTest& unit_test,
						  int /* iteration */) override {
	SendLn("event=TestIterationEnd&passed=" +
		   FormatBool(unit_test.Passed()) + "&elapsed_time=" +
		   StreamableToString(unit_test.elapsed_time()) + "ms");
  }

  void OnTestSuiteStart(const TestSuite& test_case) override {
	SendLn(std::string("event=TestSuiteStart&name=") + test_case.name());
  }

  // Note that "event=TestCaseEnd" is a wire format and has to remain
  // "case" for compatibilty
  void OnTestSuiteEnd(const TestSuite& test_case) override {
	SendLn("event=TestSuiteEnd&passed=" + FormatBool(test_case.Passed()) +
		   "&elapsed_time=" + StreamableToString(test_case.elapsed_time()) +
		   "ms");
  }


 #ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_
  // Note that "event=TestCaseStart" is a wire format and has to remain
  // "case" for compatibilty
  void OnTestCaseStart(const TestCase& test_case) override {
	SendLn(std::string("event=TestCaseStart&name=") + test_case.name());
  }

  // Note that "event=TestCaseEnd" is a wire format and has to remain
  // "case" for compatibilty
  void OnTestCaseEnd(const TestCase& test_case) override {
	SendLn("event=TestCaseEnd&passed=" + FormatBool(test_case.Passed()) +
		   "&elapsed_time=" + StreamableToString(test_case.elapsed_time()) +
		   "ms");
  }
 #endif // #ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_

  void OnTestStart(const TestInfo& test_info) override {
	SendLn(std::string("event=TestStart&name=") + test_info.name());
  }

  void OnTestEnd(const TestInfo& test_info) override {
	SendLn("event=TestEnd&passed=" +
		   FormatBool((test_info.result())->Passed()) +
		   "&elapsed_time=" +
		   StreamableToString((test_info.result())->elapsed_time()) + "ms");
  }

  void OnTestPartResult(const TestPartResult& test_part_result) override {
	const char* file_name = test_part_result.file_name();
	if (file_name == nullptr) file_name = "";
	SendLn("event=TestPartResult&file=" + UrlEncode(file_name) +
		   "&line=" + StreamableToString(test_part_result.line_number()) +
		   "&message=" + UrlEncode(test_part_result.message()));
  }

 private:
  // Sends the given message and a newline to the socket.
  void SendLn(const std::string& message) { socket_writer_->SendLn(message); }

  // Called at the start of streaming to notify the receiver what
  // protocol we are using.
  void Start() { SendLn("gtest_streaming_protocol_version=1.0"); }

  std::string FormatBool(bool value) { return value ? "1" : "0"; }

  const std::unique_ptr<AbstractSocketWriter> socket_writer_;

  GTEST_DISALLOW_COPY_AND_ASSIGN_(StreamingListener);
};  // class StreamingListener

#endif  // GTEST_CAN_STREAM_RESULTS_

}  // namespace internal
}  // namespace testing

GTEST_DISABLE_MSC_WARNINGS_POP_()  //  4251
