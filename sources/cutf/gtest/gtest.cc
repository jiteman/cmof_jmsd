// The Google C++ Testing and Mocking Framework (Google Test)

#include "gtest/gtest.h"
#include "gtest/internal/custom/gtest.h"
#include "gtest/gtest-spi.h"

#include "gtest-constants.h"

#include "Empty_test_event_listener.h"
#include "Test_event_listener.h"
#include "Test_suite.h"

#include "internal/Unit_test_impl.h"
#include "internal/Print_color_encoded.h"

#include "Text_output_utilities.hxx"

#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>
#include <wctype.h>

#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <limits>
#include <list>
#include <map>
#include <ostream>
#include <sstream>
#include <vector>

#if GTEST_OS_LINUX

# define GTEST_HAS_GETTIMEOFDAY_ 1

# include <fcntl.h>  // NOLINT
# include <limits.h>  // NOLINT
# include <sched.h>  // NOLINT
// Declares vsnprintf().  This header is not available on Windows.
# include <strings.h>  // NOLINT
# include <sys/mman.h>  // NOLINT
# include <sys/time.h>  // NOLINT
# include <unistd.h>  // NOLINT
# include <string>

#elif GTEST_OS_ZOS
# define GTEST_HAS_GETTIMEOFDAY_ 1
# include <sys/time.h>  // NOLINT

// On z/OS we additionally need strings.h for strcasecmp.
# include <strings.h>  // NOLINT

#elif GTEST_OS_WINDOWS_MOBILE  // We are on Windows CE.

# include <windows.h>  // NOLINT
# undef min

#elif GTEST_OS_WINDOWS  // We are on Windows proper.

# include <windows.h>  // NOLINT
# undef min

#ifdef _MSC_VER
# include <crtdbg.h>  // NOLINT
# include <debugapi.h>  // NOLINT
#endif

# include <io.h>  // NOLINT
# include <sys/timeb.h>  // NOLINT
# include <sys/types.h>  // NOLINT
# include <sys/stat.h>  // NOLINT

# if GTEST_OS_WINDOWS_MINGW
// MinGW has gettimeofday() but not _ftime64().
#  define GTEST_HAS_GETTIMEOFDAY_ 1
#  include <sys/time.h>  // NOLINT
# endif  // GTEST_OS_WINDOWS_MINGW

#else

// Assume other platforms have gettimeofday().
# define GTEST_HAS_GETTIMEOFDAY_ 1

// cpplint thinks that the header is already included, so we want to
// silence it.
# include <sys/time.h>  // NOLINT
# include <unistd.h>  // NOLINT

#endif  // GTEST_OS_LINUX

#if GTEST_HAS_EXCEPTIONS
# include <stdexcept>
#endif

#if GTEST_CAN_STREAM_RESULTS_
# include <arpa/inet.h>  // NOLINT
# include <netdb.h>  // NOLINT
# include <sys/socket.h>  // NOLINT
# include <sys/types.h>  // NOLINT
#endif

#include "gtest/gtest-internal-inl.h"

#if GTEST_OS_WINDOWS
# define vsnprintf _vsnprintf
#endif  // GTEST_OS_WINDOWS

#if GTEST_OS_MAC
#ifndef GTEST_OS_IOS
#include <crt_externs.h>
#endif
#endif

#if GTEST_HAS_ABSL
#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/stacktrace.h"
#include "absl/debugging/symbolize.h"
#include "absl/strings/str_cat.h"
#endif  // GTEST_HAS_ABSL

namespace testing {
namespace internal {

// The text used in failure messages to indicate the start of the
// stack trace.
const char kStackTraceMarker[] = "\nStack trace:\n";

// g_help_flag is true if and only if the --help flag or an equivalent form
// is specified on the command line.
bool g_help_flag = false;


// GTestIsInitialized() returns true if and only if the user has initialized
// Google Test.  Useful for catching the user mistake of not initializing
// Google Test before calling RUN_ALL_TESTS().
static bool GTestIsInitialized() { return GetArgvs().size() > 0; }

// AssertHelper constructor.
AssertHelper::AssertHelper(TestPartResult::Type type,
						   const char* file,
						   int line,
						   const char* message)
	: data_(new AssertHelperData(type, file, line, message)) {
}

AssertHelper::~AssertHelper() {
  delete data_;
}

// Message assignment, for assertion streaming support.
void AssertHelper::operator=(const Message& message) const {
	::jmsd::cutf::UnitTest::GetInstance()->AddTestPartResult(
		data_->type,
		data_->file,
		data_->line,
		AppendUserMessage( data_->message, message ),
		::jmsd::cutf::UnitTest::GetInstance()->impl()->CurrentOsStackTraceExceptTop( 1 ) );// Skips the stack frame for this function itself.
}

namespace {

// When TEST_P is found without a matching INSTANTIATE_TEST_SUITE_P
// to creates test cases for it, a syntetic test case is
// inserted to report ether an error or a log message.
//
// This configuration bit will likely be removed at some point.
constexpr bool kErrorOnUninstantiatedParameterizedTest = false;

// A test that fails at a given file/line location with a given message.
class FailureTest : public ::jmsd::cutf::Test {
 public:
  explicit FailureTest(const CodeLocation& loc, std::string error_message,
					   bool as_error)
	  : loc_(loc),
		error_message_(std::move(error_message)),
		as_error_(as_error) {}

  void TestBody() override {
	if (as_error_) {
	  AssertHelper(TestPartResult::kNonFatalFailure, loc_.file.c_str(),
				   loc_.line, "") = Message() << error_message_;
	} else {
	  std::cout << error_message_ << std::endl;
	}
  }

 private:
  const CodeLocation loc_;
  const std::string error_message_;
  const bool as_error_;
};


}  // namespace

// If this parameterized test suite has no instantiations (and that
// has not been marked as okay), emit a test case reporting that.
void InsertSyntheticTestCase(const std::string &name, CodeLocation location) {
  std::string message =
	  "Paramaterized test suite " + name +
	  " is defined via TEST_P, but never instantiated. None of the test cases "
	  "will run. Either no INSTANTIATE_TEST_SUITE_P is provided or the only "
	  "ones provided expand to nothing."
	  "\n\n"
	  "Ideally, TEST_P definitions should only ever be included as part of "
	  "binaries that intend to use them. (As opposed to, for example, being "
	  "placed in a library that may be linked in to get other utilities.)";

  std::string full_name = "UninstantiatedParamaterizedTestSuite<" + name + ">";
  RegisterTest(  //
	  "GoogleTestVerification", full_name.c_str(),
	  nullptr,  // No type parameter.
	  nullptr,  // No value parameter.
	  location.file.c_str(), location.line, [message, location] {
		return new FailureTest(location, message,
							   kErrorOnUninstantiatedParameterizedTest);
	  });
}

// A copy of all command line arguments.  Set by InitGoogleTest().
static ::std::vector<std::string> g_argvs;

::std::vector<std::string> GetArgvs() {
#if defined(GTEST_CUSTOM_GET_ARGVS_)
  // GTEST_CUSTOM_GET_ARGVS_() may return a container of std::string or
  // ::string. This code converts it to the appropriate type.
  const auto& custom = GTEST_CUSTOM_GET_ARGVS_();
  return ::std::vector<std::string>(custom.begin(), custom.end());
#else   // defined(GTEST_CUSTOM_GET_ARGVS_)
  return g_argvs;
#endif  // defined(GTEST_CUSTOM_GET_ARGVS_)
}

// Returns the current application's name, removing directory path if that
// is present.
FilePath GetCurrentExecutableName() {
  FilePath result;

#if GTEST_OS_WINDOWS || GTEST_OS_OS2
  result.Set(FilePath(GetArgvs()[0]).RemoveExtension("exe"));
#else
  result.Set(FilePath(GetArgvs()[0]));
#endif  // GTEST_OS_WINDOWS

  return result.RemoveDirectoryName();
}

}  // namespace internal

// The c'tor sets this object as the test part result reporter used by
// Google Test.  The 'result' parameter specifies where to report the
// results. Intercepts only failures from the current thread.
ScopedFakeTestPartResultReporter::ScopedFakeTestPartResultReporter(
	TestPartResultArray* result)
	: intercept_mode_(INTERCEPT_ONLY_CURRENT_THREAD),
	  result_(result) {
  Init();
}

// The c'tor sets this object as the test part result reporter used by
// Google Test.  The 'result' parameter specifies where to report the
// results.
ScopedFakeTestPartResultReporter::ScopedFakeTestPartResultReporter(
	InterceptMode intercept_mode, TestPartResultArray* result)
	: intercept_mode_(intercept_mode),
	  result_(result) {
  Init();
}

void ScopedFakeTestPartResultReporter::Init() {
  ::jmsd::cutf::internal::UnitTestImpl* const impl = ::jmsd::cutf::internal::GetUnitTestImpl();
  if (intercept_mode_ == INTERCEPT_ALL_THREADS) {
	old_reporter_ = impl->GetGlobalTestPartResultReporter();
	impl->SetGlobalTestPartResultReporter(this);
  } else {
	old_reporter_ = impl->GetTestPartResultReporterForCurrentThread();
	impl->SetTestPartResultReporterForCurrentThread(this);
  }
}

// The d'tor restores the test part result reporter used by Google Test
// before.
ScopedFakeTestPartResultReporter::~ScopedFakeTestPartResultReporter() {
  ::jmsd::cutf::internal::UnitTestImpl* const impl = ::jmsd::cutf::internal::GetUnitTestImpl();
  if (intercept_mode_ == INTERCEPT_ALL_THREADS) {
	impl->SetGlobalTestPartResultReporter(old_reporter_);
  } else {
	impl->SetTestPartResultReporterForCurrentThread(old_reporter_);
  }
}

// Increments the test part result count and remembers the result.
// This method is from the TestPartResultReporterInterface interface.
void ScopedFakeTestPartResultReporter::ReportTestPartResult(
	const TestPartResult& result) {
  result_->Append(result);
}

namespace internal {

// Returns the type ID of ::testing::Test.  We should always call this
// instead of GetTypeId< ::testing::Test>() to get the type ID of
// testing::Test.  This is to work around a suspected linker bug when
// using Google Test as a framework on Mac OS X.  The bug causes
// GetTypeId< ::testing::Test>() to return different values depending
// on whether the call is from the Google Test framework itself or
// from user test code.  GetTestTypeId() is guaranteed to always
// return the same value, as it always calls GetTypeId<>() from the
// gtest.cc, which is within the Google Test framework.
TypeId GetTestTypeId() {
	return GetTypeId< ::jmsd::cutf::Test >();
}

// The value of GetTestTypeId() as seen from within the Google Test
// library.  This is solely for testing GetTestTypeId().
extern const TypeId kTestTypeIdInGoogleTest = GetTestTypeId();

// This predicate-formatter checks that 'results' contains a test part
// failure of the given type and that the failure message contains the
// given substring.
static ::jmsd::cutf::AssertionResult HasOneFailure(const char* /* results_expr */,
									 const char* /* type_expr */,
									 const char* /* substr_expr */,
									 const TestPartResultArray& results,
									 TestPartResult::Type type,
									 const std::string& substr) {
  const std::string expected(type == TestPartResult::kFatalFailure ?
						"1 fatal failure" :
						"1 non-fatal failure");
  Message msg;
  if (results.size() != 1) {
	msg << "Expected: " << expected << "\n"
		<< "  Actual: " << results.size() << " failures";
	for (int i = 0; i < results.size(); i++) {
	  msg << "\n" << results.GetTestPartResult(i);
	}
	return ::jmsd::cutf::AssertionResult::AssertionFailure() << msg;
  }

  const TestPartResult& r = results.GetTestPartResult(0);
  if (r.type() != type) {
	return ::jmsd::cutf::AssertionResult::AssertionFailure() << "Expected: " << expected << "\n"
							  << "  Actual:\n"
							  << r;
  }

  if (strstr(r.message(), substr.c_str()) == nullptr) {
	return ::jmsd::cutf::AssertionResult::AssertionFailure() << "Expected: " << expected << " containing \""
							  << substr << "\"\n"
							  << "  Actual:\n"
							  << r;
  }

  return ::jmsd::cutf::AssertionResult::AssertionSuccess();
}

// The constructor of SingleFailureChecker remembers where to look up
// test part results, what type of failure we expect, and what
// substring the failure message should contain.
SingleFailureChecker::SingleFailureChecker(const TestPartResultArray* results,
										   TestPartResult::Type type,
										   const std::string& substr)
	: results_(results), type_(type), substr_(substr) {}

// The destructor of SingleFailureChecker verifies that the given
// TestPartResultArray contains exactly one failure that has the given
// type and contains the given substring.  If that's not the case, a
// non-fatal failure will be generated.
SingleFailureChecker::~SingleFailureChecker() {
	EXPECT_PRED_FORMAT3(HasOneFailure, *results_, type_, substr_);
}

// Returns the current time in milliseconds.
TimeInMillis GetTimeInMillis() {
#if GTEST_OS_WINDOWS_MOBILE || defined(__BORLANDC__)
  // Difference between 1970-01-01 and 1601-01-01 in milliseconds.
  // http://analogous.blogspot.com/2005/04/epoch.html
  const TimeInMillis kJavaEpochToWinFileTimeDelta =
	static_cast<TimeInMillis>(116444736UL) * 100000UL;
  const DWORD kTenthMicrosInMilliSecond = 10000;

  SYSTEMTIME now_systime;
  FILETIME now_filetime;
  ULARGE_INTEGER now_int64;
  GetSystemTime(&now_systime);
  if (SystemTimeToFileTime(&now_systime, &now_filetime)) {
	now_int64.LowPart = now_filetime.dwLowDateTime;
	now_int64.HighPart = now_filetime.dwHighDateTime;
	now_int64.QuadPart = (now_int64.QuadPart / kTenthMicrosInMilliSecond) -
	  kJavaEpochToWinFileTimeDelta;
	return now_int64.QuadPart;
  }
  return 0;
#elif GTEST_OS_WINDOWS && !GTEST_HAS_GETTIMEOFDAY_
  __timeb64 now;

  // MSVC 8 deprecates _ftime64(), so we want to suppress warning 4996
  // (deprecated function) there.
  GTEST_DISABLE_MSC_DEPRECATED_PUSH_()
  _ftime64(&now);
  GTEST_DISABLE_MSC_DEPRECATED_POP_()

  return static_cast<TimeInMillis>(now.time) * 1000 + now.millitm;
#elif GTEST_HAS_GETTIMEOFDAY_
  struct timeval now;
  gettimeofday(&now, nullptr);
  return static_cast<TimeInMillis>(now.tv_sec) * 1000 + now.tv_usec / 1000;
#else
# error "Don't know how to get the current time on your system."
#endif
}

// Utilities

// class String.

#if GTEST_OS_WINDOWS_MOBILE
// Creates a UTF-16 wide string from the given ANSI string, allocating
// memory using new. The caller is responsible for deleting the return
// value using delete[]. Returns the wide string, or NULL if the
// input is NULL.
LPCWSTR String::AnsiToUtf16(const char* ansi) {
  if (!ansi) return nullptr;
  const int length = strlen(ansi);
  const int unicode_length =
	  MultiByteToWideChar(CP_ACP, 0, ansi, length, nullptr, 0);
  WCHAR* unicode = new WCHAR[unicode_length + 1];
  MultiByteToWideChar(CP_ACP, 0, ansi, length,
					  unicode, unicode_length);
  unicode[unicode_length] = 0;
  return unicode;
}

// Creates an ANSI string from the given wide string, allocating
// memory using new. The caller is responsible for deleting the return
// value using delete[]. Returns the ANSI string, or NULL if the
// input is NULL.
const char* String::Utf16ToAnsi(LPCWSTR utf16_str)  {
  if (!utf16_str) return nullptr;
  const int ansi_length = WideCharToMultiByte(CP_ACP, 0, utf16_str, -1, nullptr,
											  0, nullptr, nullptr);
  char* ansi = new char[ansi_length + 1];
  WideCharToMultiByte(CP_ACP, 0, utf16_str, -1, ansi, ansi_length, nullptr,
					  nullptr);
  ansi[ansi_length] = 0;
  return ansi;
}

#endif  // GTEST_OS_WINDOWS_MOBILE

// Compares two C strings.  Returns true if and only if they have the same
// content.
//
// Unlike strcmp(), this function can handle NULL argument(s).  A NULL
// C string is considered different to any non-NULL C string,
// including the empty string.
bool String::CStringEquals(const char * lhs, const char * rhs) {
  if (lhs == nullptr) return rhs == nullptr;

  if (rhs == nullptr) return false;

  return strcmp(lhs, rhs) == 0;
}

#if GTEST_HAS_STD_WSTRING

// Converts an array of wide chars to a narrow string using the UTF-8
// encoding, and streams the result to the given Message object.
static void StreamWideCharsToMessage(const wchar_t* wstr, size_t length,
									 Message* msg) {
  for (size_t i = 0; i != length; ) {  // NOLINT
	if (wstr[i] != L'\0') {
	  *msg << WideStringToUtf8(wstr + i, static_cast<int>(length - i));
	  while (i != length && wstr[i] != L'\0')
		i++;
	} else {
	  *msg << '\0';
	  i++;
	}
  }
}

#endif  // GTEST_HAS_STD_WSTRING

void SplitString(const ::std::string& str, char delimiter,
				 ::std::vector< ::std::string>* dest) {
  ::std::vector< ::std::string> parsed;
  ::std::string::size_type pos = 0;
  while (::testing::internal::AlwaysTrue()) {
	const ::std::string::size_type colon = str.find(delimiter, pos);
	if (colon == ::std::string::npos) {
	  parsed.push_back(str.substr(pos));
	  break;
	} else {
	  parsed.push_back(str.substr(pos, colon - pos));
	  pos = colon + 1;
	}
  }
  dest->swap(parsed);
}

}  // namespace internal

// Constructs an empty Message.
// We allocate the stringstream separately because otherwise each use of
// ASSERT/EXPECT in a procedure adds over 200 bytes to the procedure's
// stack frame leading to huge stack frames in some cases; gcc does not reuse
// the stack space.
Message::Message() : ss_(new ::std::stringstream) {
  // By default, we want there to be enough precision when printing
  // a double to a Message.
  *ss_ << std::setprecision(std::numeric_limits<double>::digits10 + 2);
}

// These two overloads allow streaming a wide C string to a Message
// using the UTF-8 encoding.
Message& Message::operator <<(const wchar_t* wide_c_str) {
  return *this << internal::String::ShowWideCString(wide_c_str);
}
Message& Message::operator <<(wchar_t* wide_c_str) {
  return *this << internal::String::ShowWideCString(wide_c_str);
}

#if GTEST_HAS_STD_WSTRING
// Converts the given wide string to a narrow string using the UTF-8
// encoding, and streams the result to this Message object.
Message& Message::operator <<(const ::std::wstring& wstr) {
  internal::StreamWideCharsToMessage(wstr.c_str(), wstr.length(), this);
  return *this;
}
#endif  // GTEST_HAS_STD_WSTRING

// Gets the text streamed to this object so far as an std::string.
// Each '\0' character in the buffer is replaced with "\\0".
std::string Message::GetString() const {
  return internal::StringStreamToString(ss_.get());
}


namespace internal {

namespace edit_distance {
std::vector<EditType> CalculateOptimalEdits(const std::vector<size_t>& left,
											const std::vector<size_t>& right) {
  std::vector<std::vector<double> > costs(
	  left.size() + 1, std::vector<double>(right.size() + 1));
  std::vector<std::vector<EditType> > best_move(
	  left.size() + 1, std::vector<EditType>(right.size() + 1));

  // Populate for empty right.
  for (size_t l_i = 0; l_i < costs.size(); ++l_i) {
	costs[l_i][0] = static_cast<double>(l_i);
	best_move[l_i][0] = kRemove;
  }
  // Populate for empty left.
  for (size_t r_i = 1; r_i < costs[0].size(); ++r_i) {
	costs[0][r_i] = static_cast<double>(r_i);
	best_move[0][r_i] = kAdd;
  }

  for (size_t l_i = 0; l_i < left.size(); ++l_i) {
	for (size_t r_i = 0; r_i < right.size(); ++r_i) {
	  if (left[l_i] == right[r_i]) {
		// Found a match. Consume it.
		costs[l_i + 1][r_i + 1] = costs[l_i][r_i];
		best_move[l_i + 1][r_i + 1] = kMatch;
		continue;
	  }

	  const double add = costs[l_i + 1][r_i];
	  const double remove = costs[l_i][r_i + 1];
	  const double replace = costs[l_i][r_i];
	  if (add < remove && add < replace) {
		costs[l_i + 1][r_i + 1] = add + 1;
		best_move[l_i + 1][r_i + 1] = kAdd;
	  } else if (remove < add && remove < replace) {
		costs[l_i + 1][r_i + 1] = remove + 1;
		best_move[l_i + 1][r_i + 1] = kRemove;
	  } else {
		// We make replace a little more expensive than add/remove to lower
		// their priority.
		costs[l_i + 1][r_i + 1] = replace + 1.00001;
		best_move[l_i + 1][r_i + 1] = kReplace;
	  }
	}
  }

  // Reconstruct the best path. We do it in reverse order.
  std::vector<EditType> best_path;
  for (size_t l_i = left.size(), r_i = right.size(); l_i > 0 || r_i > 0;) {
	EditType move = best_move[l_i][r_i];
	best_path.push_back(move);
	l_i -= move != kAdd;
	r_i -= move != kRemove;
  }
  std::reverse(best_path.begin(), best_path.end());
  return best_path;
}

namespace {

// Helper class to convert string into ids with deduplication.
class InternalStrings {
 public:
  size_t GetId(const std::string& str) {
	IdMap::iterator it = ids_.find(str);
	if (it != ids_.end()) return it->second;
	size_t id = ids_.size();
	return ids_[str] = id;
  }

 private:
  typedef std::map<std::string, size_t> IdMap;
  IdMap ids_;
};

}  // namespace

std::vector<EditType> CalculateOptimalEdits(
	const std::vector<std::string>& left,
	const std::vector<std::string>& right) {
  std::vector<size_t> left_ids, right_ids;
  {
	InternalStrings intern_table;
	for (size_t i = 0; i < left.size(); ++i) {
	  left_ids.push_back(intern_table.GetId(left[i]));
	}
	for (size_t i = 0; i < right.size(); ++i) {
	  right_ids.push_back(intern_table.GetId(right[i]));
	}
  }
  return CalculateOptimalEdits(left_ids, right_ids);
}

namespace {

// Helper class that holds the state for one hunk and prints it out to the
// stream.
// It reorders adds/removes when possible to group all removes before all
// adds. It also adds the hunk header before printint into the stream.
class Hunk {
public:
	Hunk( size_t left_start, size_t right_start )
		:
			left_start_( left_start ),
			right_start_( right_start ),
			adds_(),
			removes_(),
			common_()
	{}

	void PushLine( char edit, char const *line ) {
		switch ( edit ) {
			case ' ':
				++common_;
				FlushEdits();
				hunk_.push_back( ::std::make_pair( ' ', line ) );
				break;

			case '-':
				++removes_;
				hunk_removes_.push_back( ::std::make_pair( '-', line ) );
				break;

			case '+':
				++adds_;
				hunk_adds_.push_back( ::std::make_pair( '+', line ) );
				break;
		}
	}

  void PrintTo(std::ostream* os) {
	PrintHeader(os);
	FlushEdits();
	for (std::list<std::pair<char, const char*> >::const_iterator it =
			 hunk_.begin();
		 it != hunk_.end(); ++it) {
	  *os << it->first << it->second << "\n";
	}
  }

  bool has_edits() const { return adds_ || removes_; }

 private:
  void FlushEdits() {
	hunk_.splice(hunk_.end(), hunk_removes_);
	hunk_.splice(hunk_.end(), hunk_adds_);
  }

  // Print a unified diff header for one hunk.
  // The format is
  //   "@@ -<left_start>,<left_length> +<right_start>,<right_length> @@"
  // where the left/right parts are omitted if unnecessary.
  void PrintHeader(std::ostream* ss) const {
	*ss << "@@ ";
	if (removes_) {
	  *ss << "-" << left_start_ << "," << (removes_ + common_);
	}
	if (removes_ && adds_) {
	  *ss << " ";
	}
	if (adds_) {
	  *ss << "+" << right_start_ << "," << (adds_ + common_);
	}
	*ss << " @@\n";
  }

  size_t left_start_, right_start_;
  size_t adds_, removes_, common_;
  std::list<std::pair<char, const char*> > hunk_, hunk_adds_, hunk_removes_;
};

}  // namespace

// Create a list of diff hunks in Unified diff format.
// Each hunk has a header generated by PrintHeader above plus a body with
// lines prefixed with ' ' for no change, '-' for deletion and '+' for
// addition.
// 'context' represents the desired unchanged prefix/suffix around the diff.
// If two hunks are close enough that their contexts overlap, then they are
// joined into one hunk.
std::string CreateUnifiedDiff(const std::vector<std::string>& left,
							  const std::vector<std::string>& right,
							  size_t context) {
  const std::vector<EditType> edits = CalculateOptimalEdits(left, right);

  size_t l_i = 0, r_i = 0, edit_i = 0;
  std::stringstream ss;
  while (edit_i < edits.size()) {
	// Find first edit.
	while (edit_i < edits.size() && edits[edit_i] == kMatch) {
	  ++l_i;
	  ++r_i;
	  ++edit_i;
	}

	// Find the first line to include in the hunk.
	const size_t prefix_context = std::min(l_i, context);
	Hunk hunk(l_i - prefix_context + 1, r_i - prefix_context + 1);
	for (size_t i = prefix_context; i > 0; --i) {
	  hunk.PushLine(' ', left[l_i - i].c_str());
	}

	// Iterate the edits until we found enough suffix for the hunk or the input
	// is over.
	size_t n_suffix = 0;
	for (; edit_i < edits.size(); ++edit_i) {
	  if (n_suffix >= context) {
		// Continue only if the next hunk is very close.
		auto it = edits.begin() + static_cast<int>(edit_i);
		while (it != edits.end() && *it == kMatch) ++it;
		if (it == edits.end() ||
			static_cast<size_t>(it - edits.begin()) - edit_i >= context) {
		  // There is no next edit or it is too far away.
		  break;
		}
	  }

	  EditType edit = edits[edit_i];
	  // Reset count when a non match is found.
	  n_suffix = edit == kMatch ? n_suffix + 1 : 0;

	  if (edit == kMatch || edit == kRemove || edit == kReplace) {
		hunk.PushLine(edit == kMatch ? ' ' : '-', left[l_i].c_str());
	  }
	  if (edit == kAdd || edit == kReplace) {
		hunk.PushLine('+', right[r_i].c_str());
	  }

	  // Advance indices, depending on edit type.
	  l_i += edit != kAdd;
	  r_i += edit != kRemove;
	}

	if (!hunk.has_edits()) {
	  // We are done. We don't want this hunk.
	  break;
	}

	hunk.PrintTo(&ss);
  }
  return ss.str();
}

}  // namespace edit_distance

namespace {

// The string representation of the values received in EqFailure() are already
// escaped. Split them on escaped '\n' boundaries. Leave all other escaped
// characters the same.
std::vector<std::string> SplitEscapedString(const std::string& str) {
  std::vector<std::string> lines;
  size_t start = 0, end = str.size();
  if (end > 2 && str[0] == '"' && str[end - 1] == '"') {
	++start;
	--end;
  }
  bool escaped = false;
  for (size_t i = start; i + 1 < end; ++i) {
	if (escaped) {
	  escaped = false;
	  if (str[i] == 'n') {
		lines.push_back(str.substr(start, i - start - 1));
		start = i + 1;
	  }
	} else {
	  escaped = str[i] == '\\';
	}
  }
  lines.push_back(str.substr(start, end - start));
  return lines;
}

}  // namespace

// Constructs and returns the message for an equality assertion
// (e.g. ASSERT_EQ, EXPECT_STREQ, etc) failure.
//
// The first four parameters are the expressions used in the assertion
// and their values, as strings.  For example, for ASSERT_EQ(foo, bar)
// where foo is 5 and bar is 6, we have:
//
//   lhs_expression: "foo"
//   rhs_expression: "bar"
//   lhs_value:      "5"
//   rhs_value:      "6"
//
// The ignoring_case parameter is true if and only if the assertion is a
// *_STRCASEEQ*.  When it's true, the string "Ignoring case" will
// be inserted into the message.
::jmsd::cutf::AssertionResult EqFailure(const char* lhs_expression,
						  const char* rhs_expression,
						  const std::string& lhs_value,
						  const std::string& rhs_value,
						  bool ignoring_case) {
  Message msg;
  msg << "Expected equality of these values:";
  msg << "\n  " << lhs_expression;
  if (lhs_value != lhs_expression) {
	msg << "\n    Which is: " << lhs_value;
  }
  msg << "\n  " << rhs_expression;
  if (rhs_value != rhs_expression) {
	msg << "\n    Which is: " << rhs_value;
  }

  if (ignoring_case) {
	msg << "\nIgnoring case";
  }

  if (!lhs_value.empty() && !rhs_value.empty()) {
	const std::vector<std::string> lhs_lines =
		SplitEscapedString(lhs_value);
	const std::vector<std::string> rhs_lines =
		SplitEscapedString(rhs_value);
	if (lhs_lines.size() > 1 || rhs_lines.size() > 1) {
	  msg << "\nWith diff:\n"
		  << edit_distance::CreateUnifiedDiff(lhs_lines, rhs_lines);
	}
  }

  return ::jmsd::cutf::AssertionResult::AssertionFailure() << msg;
}

// Constructs a failure message for Boolean assertions such as EXPECT_TRUE.
std::string GetBoolAssertionFailureMessage(
	const ::jmsd::cutf::AssertionResult& assertion_result,
	const char* expression_text,
	const char* actual_predicate_value,
	const char* expected_predicate_value) {
  const char* actual_message = assertion_result.message();
  Message msg;
  msg << "Value of: " << expression_text
	  << "\n  Actual: " << actual_predicate_value;
  if (actual_message[0] != '\0')
	msg << " (" << actual_message << ")";
  msg << "\nExpected: " << expected_predicate_value;
  return msg.GetString();
}

// Helper function for implementing ASSERT_NEAR.
::jmsd::cutf::AssertionResult DoubleNearPredFormat(const char* expr1,
									 const char* expr2,
									 const char* abs_error_expr,
									 double val1,
									 double val2,
									 double abs_error) {
  const double diff = fabs(val1 - val2);
  if (diff <= abs_error) return ::jmsd::cutf::AssertionResult::AssertionSuccess();

  return ::jmsd::cutf::AssertionResult::AssertionFailure()
	  << "The difference between " << expr1 << " and " << expr2
	  << " is " << diff << ", which exceeds " << abs_error_expr << ", where\n"
	  << expr1 << " evaluates to " << val1 << ",\n"
	  << expr2 << " evaluates to " << val2 << ", and\n"
	  << abs_error_expr << " evaluates to " << abs_error << ".";
}


// Helper template for implementing FloatLE() and DoubleLE().
template <typename RawType>
::jmsd::cutf::AssertionResult FloatingPointLE(const char* expr1,
								const char* expr2,
								RawType val1,
								RawType val2) {
  // Returns success if val1 is less than val2,
  if (val1 < val2) {
	return ::jmsd::cutf::AssertionResult::AssertionSuccess();
  }

  // or if val1 is almost equal to val2.
  const FloatingPoint<RawType> lhs(val1), rhs(val2);
  if (lhs.AlmostEquals(rhs)) {
	return ::jmsd::cutf::AssertionResult::AssertionSuccess();
  }

  // Note that the above two checks will both fail if either val1 or
  // val2 is NaN, as the IEEE floating-point standard requires that
  // any predicate involving a NaN must return false.

  ::std::stringstream val1_ss;
  val1_ss << std::setprecision(std::numeric_limits<RawType>::digits10 + 2)
		  << val1;

  ::std::stringstream val2_ss;
  val2_ss << std::setprecision(std::numeric_limits<RawType>::digits10 + 2)
		  << val2;

  return ::jmsd::cutf::AssertionResult::AssertionFailure()
	  << "Expected: (" << expr1 << ") <= (" << expr2 << ")\n"
	  << "  Actual: " << StringStreamToString(&val1_ss) << " vs "
	  << StringStreamToString(&val2_ss);
}

}  // namespace internal

// Asserts that val1 is less than, or almost equal to, val2.  Fails
// otherwise.  In particular, it fails if either val1 or val2 is NaN.
::jmsd::cutf::AssertionResult FloatLE(const char* expr1, const char* expr2,
						float val1, float val2) {
  return internal::FloatingPointLE<float>(expr1, expr2, val1, val2);
}

// Asserts that val1 is less than, or almost equal to, val2.  Fails
// otherwise.  In particular, it fails if either val1 or val2 is NaN.
::jmsd::cutf::AssertionResult DoubleLE(const char* expr1, const char* expr2,
						 double val1, double val2) {
  return internal::FloatingPointLE<double>(expr1, expr2, val1, val2);
}

namespace internal {

// The helper function for {ASSERT|EXPECT}_EQ with int or enum
// arguments.
::jmsd::cutf::AssertionResult CmpHelperEQ(const char* lhs_expression,
							const char* rhs_expression,
							BiggestInt lhs,
							BiggestInt rhs) {
  if (lhs == rhs) {
	return ::jmsd::cutf::AssertionResult::AssertionSuccess();
  }

  return EqFailure(lhs_expression,
				   rhs_expression,
				   FormatForComparisonFailureMessage(lhs, rhs),
				   FormatForComparisonFailureMessage(rhs, lhs),
				   false);
}

// A macro for implementing the helper functions needed to implement
// ASSERT_?? and EXPECT_?? with integer or enum arguments.  It is here
// just to avoid copy-and-paste of similar code.
#define GTEST_IMPL_CMP_HELPER_(op_name, op)\
::jmsd::cutf::AssertionResult CmpHelper##op_name(const char* expr1, const char* expr2, \
								   BiggestInt val1, BiggestInt val2) {\
  if (val1 op val2) {\
	return ::jmsd::cutf::AssertionResult::AssertionSuccess();\
  } else {\
	return ::jmsd::cutf::AssertionResult::AssertionFailure() \
		<< "Expected: (" << expr1 << ") " #op " (" << expr2\
		<< "), actual: " << FormatForComparisonFailureMessage(val1, val2)\
		<< " vs " << FormatForComparisonFailureMessage(val2, val1);\
  }\
}

// Implements the helper function for {ASSERT|EXPECT}_NE with int or
// enum arguments.
GTEST_IMPL_CMP_HELPER_(NE, !=)
// Implements the helper function for {ASSERT|EXPECT}_LE with int or
// enum arguments.
GTEST_IMPL_CMP_HELPER_(LE, <=)
// Implements the helper function for {ASSERT|EXPECT}_LT with int or
// enum arguments.
GTEST_IMPL_CMP_HELPER_(LT, < )
// Implements the helper function for {ASSERT|EXPECT}_GE with int or
// enum arguments.
GTEST_IMPL_CMP_HELPER_(GE, >=)
// Implements the helper function for {ASSERT|EXPECT}_GT with int or
// enum arguments.
GTEST_IMPL_CMP_HELPER_(GT, > )

#undef GTEST_IMPL_CMP_HELPER_

// The helper function for {ASSERT|EXPECT}_STREQ.
::jmsd::cutf::AssertionResult CmpHelperSTREQ(const char* lhs_expression,
							   const char* rhs_expression,
							   const char* lhs,
							   const char* rhs) {
  if (String::CStringEquals(lhs, rhs)) {
	return ::jmsd::cutf::AssertionResult::AssertionSuccess();
  }

  return EqFailure(lhs_expression,
				   rhs_expression,
				   PrintToString(lhs),
				   PrintToString(rhs),
				   false);
}

// The helper function for {ASSERT|EXPECT}_STRCASEEQ.
::jmsd::cutf::AssertionResult CmpHelperSTRCASEEQ(const char* lhs_expression,
								   const char* rhs_expression,
								   const char* lhs,
								   const char* rhs) {
  if (String::CaseInsensitiveCStringEquals(lhs, rhs)) {
	return ::jmsd::cutf::AssertionResult::AssertionSuccess();
  }

  return EqFailure(lhs_expression,
				   rhs_expression,
				   PrintToString(lhs),
				   PrintToString(rhs),
				   true);
}

// The helper function for {ASSERT|EXPECT}_STRNE.
::jmsd::cutf::AssertionResult CmpHelperSTRNE(const char* s1_expression,
							   const char* s2_expression,
							   const char* s1,
							   const char* s2) {
  if (!String::CStringEquals(s1, s2)) {
	return ::jmsd::cutf::AssertionResult::AssertionSuccess();
  } else {
	return ::jmsd::cutf::AssertionResult::AssertionFailure() << "Expected: (" << s1_expression << ") != ("
							  << s2_expression << "), actual: \""
							  << s1 << "\" vs \"" << s2 << "\"";
  }
}

// The helper function for {ASSERT|EXPECT}_STRCASENE.
::jmsd::cutf::AssertionResult CmpHelperSTRCASENE(const char* s1_expression,
								   const char* s2_expression,
								   const char* s1,
								   const char* s2) {
  if (!String::CaseInsensitiveCStringEquals(s1, s2)) {
	return ::jmsd::cutf::AssertionResult::AssertionSuccess();
  } else {
	return ::jmsd::cutf::AssertionResult::AssertionFailure()
		<< "Expected: (" << s1_expression << ") != ("
		<< s2_expression << ") (ignoring case), actual: \""
		<< s1 << "\" vs \"" << s2 << "\"";
  }
}

}  // namespace internal

namespace {

// Helper functions for implementing IsSubString() and IsNotSubstring().

// This group of overloaded functions return true if and only if needle
// is a substring of haystack.  NULL is considered a substring of
// itself only.

bool IsSubstringPred(const char* needle, const char* haystack) {
  if (needle == nullptr || haystack == nullptr) return needle == haystack;

  return strstr(haystack, needle) != nullptr;
}

bool IsSubstringPred(const wchar_t* needle, const wchar_t* haystack) {
  if (needle == nullptr || haystack == nullptr) return needle == haystack;

  return wcsstr(haystack, needle) != nullptr;
}

// StringType here can be either ::std::string or ::std::wstring.
template <typename StringType>
bool IsSubstringPred(const StringType& needle,
					 const StringType& haystack) {
  return haystack.find(needle) != StringType::npos;
}

// This function implements either IsSubstring() or IsNotSubstring(),
// depending on the value of the expected_to_be_substring parameter.
// StringType here can be const char*, const wchar_t*, ::std::string,
// or ::std::wstring.
template <typename StringType>
::jmsd::cutf::AssertionResult IsSubstringImpl(
	bool expected_to_be_substring,
	const char* needle_expr, const char* haystack_expr,
	const StringType& needle, const StringType& haystack) {
  if (IsSubstringPred(needle, haystack) == expected_to_be_substring)
	return ::jmsd::cutf::AssertionResult::AssertionSuccess();

  const bool is_wide_string = sizeof(needle[0]) > 1;
  const char* const begin_string_quote = is_wide_string ? "L\"" : "\"";
  return ::jmsd::cutf::AssertionResult::AssertionFailure()
	  << "Value of: " << needle_expr << "\n"
	  << "  Actual: " << begin_string_quote << needle << "\"\n"
	  << "Expected: " << (expected_to_be_substring ? "" : "not ")
	  << "a substring of " << haystack_expr << "\n"
	  << "Which is: " << begin_string_quote << haystack << "\"";
}

}  // namespace

// IsSubstring() and IsNotSubstring() check whether needle is a
// substring of haystack (NULL is considered a substring of itself
// only), and return an appropriate error message when they fail.

::jmsd::cutf::AssertionResult IsSubstring(
	const char* needle_expr, const char* haystack_expr,
	const char* needle, const char* haystack) {
  return IsSubstringImpl(true, needle_expr, haystack_expr, needle, haystack);
}

::jmsd::cutf::AssertionResult IsSubstring(
	const char* needle_expr, const char* haystack_expr,
	const wchar_t* needle, const wchar_t* haystack) {
  return IsSubstringImpl(true, needle_expr, haystack_expr, needle, haystack);
}

::jmsd::cutf::AssertionResult IsNotSubstring(
	const char* needle_expr, const char* haystack_expr,
	const char* needle, const char* haystack) {
  return IsSubstringImpl(false, needle_expr, haystack_expr, needle, haystack);
}

::jmsd::cutf::AssertionResult IsNotSubstring(
	const char* needle_expr, const char* haystack_expr,
	const wchar_t* needle, const wchar_t* haystack) {
  return IsSubstringImpl(false, needle_expr, haystack_expr, needle, haystack);
}

::jmsd::cutf::AssertionResult IsSubstring(
	const char* needle_expr, const char* haystack_expr,
	const ::std::string& needle, const ::std::string& haystack) {
  return IsSubstringImpl(true, needle_expr, haystack_expr, needle, haystack);
}

::jmsd::cutf::AssertionResult IsNotSubstring(
	const char* needle_expr, const char* haystack_expr,
	const ::std::string& needle, const ::std::string& haystack) {
  return IsSubstringImpl(false, needle_expr, haystack_expr, needle, haystack);
}

#if GTEST_HAS_STD_WSTRING
::jmsd::cutf::AssertionResult IsSubstring(
	const char* needle_expr, const char* haystack_expr,
	const ::std::wstring& needle, const ::std::wstring& haystack) {
  return IsSubstringImpl(true, needle_expr, haystack_expr, needle, haystack);
}

::jmsd::cutf::AssertionResult IsNotSubstring(
	const char* needle_expr, const char* haystack_expr,
	const ::std::wstring& needle, const ::std::wstring& haystack) {
  return IsSubstringImpl(false, needle_expr, haystack_expr, needle, haystack);
}
#endif  // GTEST_HAS_STD_WSTRING

namespace internal {

#if GTEST_OS_WINDOWS

namespace {

// Helper function for IsHRESULT{SuccessFailure} predicates
::jmsd::cutf::AssertionResult HRESULTFailureHelper(const char* expr,
									 const char* expected,
									 long hr) {  // NOLINT
# if GTEST_OS_WINDOWS_MOBILE || GTEST_OS_WINDOWS_TV_TITLE

  // Windows CE doesn't support FormatMessage.
  const char error_text[] = "";

# else

  // Looks up the human-readable system message for the HRESULT code
  // and since we're not passing any params to FormatMessage, we don't
  // want inserts expanded.
  const DWORD kFlags = FORMAT_MESSAGE_FROM_SYSTEM |
					   FORMAT_MESSAGE_IGNORE_INSERTS;
  const DWORD kBufSize = 4096;
  // Gets the system's human readable message string for this HRESULT.
  char error_text[kBufSize] = { '\0' };
  DWORD message_length = ::FormatMessageA(kFlags,
										  0,   // no source, we're asking system
										  static_cast<DWORD>(hr),  // the error
										  0,   // no line width restrictions
										  error_text,  // output buffer
										  kBufSize,    // buf size
										  nullptr);  // no arguments for inserts
  // Trims tailing white space (FormatMessage leaves a trailing CR-LF)
  for (; message_length && IsSpace(error_text[message_length - 1]);
		  --message_length) {
	error_text[message_length - 1] = '\0';
  }

# endif  // GTEST_OS_WINDOWS_MOBILE

  const std::string error_hex("0x" + String::FormatHexInt(hr));
  return ::jmsd::cutf::AssertionResult::AssertionFailure()
	  << "Expected: " << expr << " " << expected << ".\n"
	  << "  Actual: " << error_hex << " " << error_text << "\n";
}

}  // namespace

::jmsd::cutf::AssertionResult IsHRESULTSuccess(const char* expr, long hr) {  // NOLINT
  if (SUCCEEDED(hr)) {
	return ::jmsd::cutf::AssertionResult::AssertionSuccess();
  }
  return HRESULTFailureHelper(expr, "succeeds", hr);
}

::jmsd::cutf::AssertionResult IsHRESULTFailure(const char* expr, long hr) {  // NOLINT
  if (FAILED(hr)) {
	return ::jmsd::cutf::AssertionResult::AssertionSuccess();
  }
  return HRESULTFailureHelper(expr, "fails", hr);
}

#endif  // GTEST_OS_WINDOWS

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
constexpr uint32_t kMaxCodePoint1 = (static_cast<uint32_t>(1) <<  7) - 1;

// The maximum code-point a two-byte UTF-8 sequence can represent.
constexpr uint32_t kMaxCodePoint2 = (static_cast<uint32_t>(1) << (5 + 6)) - 1;

// The maximum code-point a three-byte UTF-8 sequence can represent.
constexpr uint32_t kMaxCodePoint3 = (static_cast<uint32_t>(1) << (4 + 2*6)) - 1;

// The maximum code-point a four-byte UTF-8 sequence can represent.
constexpr uint32_t kMaxCodePoint4 = (static_cast<uint32_t>(1) << (3 + 3*6)) - 1;

// Chops off the n lowest bits from a bit pattern.  Returns the n
// lowest bits.  As a side effect, the original bit pattern will be
// shifted to the right by n bits.
inline uint32_t ChopLowBits(uint32_t* bits, int n) {
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
std::string CodePointToUtf8(uint32_t code_point) {
  if (code_point > kMaxCodePoint4) {
	return "(Invalid Unicode 0x" + String::FormatHexUInt32(code_point) + ")";
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
inline bool IsUtf16SurrogatePair(wchar_t first, wchar_t second) {
  return sizeof(wchar_t) == 2 &&
	  (first & 0xFC00) == 0xD800 && (second & 0xFC00) == 0xDC00;
}

// Creates a Unicode code point from UTF16 surrogate pair.
inline uint32_t CreateCodePointFromUtf16SurrogatePair(wchar_t first,
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
std::string WideStringToUtf8(const wchar_t* str, int num_chars) {
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
  return StringStreamToString(&stream);
}

// Converts a wide C string to an std::string using the UTF-8 encoding.
// NULL will be converted to "(null)".
std::string String::ShowWideCString(const wchar_t * wide_c_str) {
  if (wide_c_str == nullptr) return "(null)";

  return internal::WideStringToUtf8(wide_c_str, -1);
}

// Compares two wide C strings.  Returns true if and only if they have the
// same content.
//
// Unlike wcscmp(), this function can handle NULL argument(s).  A NULL
// C string is considered different to any non-NULL C string,
// including the empty string.
bool String::WideCStringEquals(const wchar_t * lhs, const wchar_t * rhs) {
  if (lhs == nullptr) return rhs == nullptr;

  if (rhs == nullptr) return false;

  return wcscmp(lhs, rhs) == 0;
}

// Helper function for *_STREQ on wide strings.
::jmsd::cutf::AssertionResult CmpHelperSTREQ(const char* lhs_expression,
							   const char* rhs_expression,
							   const wchar_t* lhs,
							   const wchar_t* rhs) {
  if (String::WideCStringEquals(lhs, rhs)) {
	return ::jmsd::cutf::AssertionResult::AssertionSuccess();
  }

  return EqFailure(lhs_expression,
				   rhs_expression,
				   PrintToString(lhs),
				   PrintToString(rhs),
				   false);
}

// Helper function for *_STRNE on wide strings.
::jmsd::cutf::AssertionResult CmpHelperSTRNE(const char* s1_expression,
							   const char* s2_expression,
							   const wchar_t* s1,
							   const wchar_t* s2) {
  if (!String::WideCStringEquals(s1, s2)) {
	return ::jmsd::cutf::AssertionResult::AssertionSuccess();
  }

  return ::jmsd::cutf::AssertionResult::AssertionFailure() << "Expected: (" << s1_expression << ") != ("
							<< s2_expression << "), actual: "
							<< PrintToString(s1)
							<< " vs " << PrintToString(s2);
}

// Compares two C strings, ignoring case.  Returns true if and only if they have
// the same content.
//
// Unlike strcasecmp(), this function can handle NULL argument(s).  A
// NULL C string is considered different to any non-NULL C string,
// including the empty string.
bool String::CaseInsensitiveCStringEquals(const char * lhs, const char * rhs) {
  if (lhs == nullptr) return rhs == nullptr;
  if (rhs == nullptr) return false;
  return posix::StrCaseCmp(lhs, rhs) == 0;
}

// Compares two wide C strings, ignoring case.  Returns true if and only if they
// have the same content.
//
// Unlike wcscasecmp(), this function can handle NULL argument(s).
// A NULL C string is considered different to any non-NULL wide C string,
// including the empty string.
// NB: The implementations on different platforms slightly differ.
// On windows, this method uses _wcsicmp which compares according to LC_CTYPE
// environment variable. On GNU platform this method uses wcscasecmp
// which compares according to LC_CTYPE category of the current locale.
// On MacOS X, it uses towlower, which also uses LC_CTYPE category of the
// current locale.
bool String::CaseInsensitiveWideCStringEquals(const wchar_t* lhs,
											  const wchar_t* rhs) {
  if (lhs == nullptr) return rhs == nullptr;

  if (rhs == nullptr) return false;

#if GTEST_OS_WINDOWS
  return _wcsicmp(lhs, rhs) == 0;
#elif GTEST_OS_LINUX && !GTEST_OS_LINUX_ANDROID
  return wcscasecmp(lhs, rhs) == 0;
#else
  // Android, Mac OS X and Cygwin don't define wcscasecmp.
  // Other unknown OSes may not define it either.
  wint_t left, right;
  do {
	left = towlower(static_cast<wint_t>(*lhs++));
	right = towlower(static_cast<wint_t>(*rhs++));
  } while (left && left == right);
  return left == right;
#endif  // OS selector
}

// Returns true if and only if str ends with the given suffix, ignoring case.
// Any string is considered to end with an empty suffix.
bool String::EndsWithCaseInsensitive(
	const std::string& str, const std::string& suffix) {
  const size_t str_len = str.length();
  const size_t suffix_len = suffix.length();
  return (str_len >= suffix_len) &&
		 CaseInsensitiveCStringEquals(str.c_str() + str_len - suffix_len,
									  suffix.c_str());
}

// Formats an int value as "%02d".
std::string String::FormatIntWidth2(int value) {
  std::stringstream ss;
  ss << std::setfill('0') << std::setw(2) << value;
  return ss.str();
}

// Formats an int value as "%X".
std::string String::FormatHexUInt32(uint32_t value) {
  std::stringstream ss;
  ss << std::hex << std::uppercase << value;
  return ss.str();
}

// Formats an int value as "%X".
std::string String::FormatHexInt(int value) {
  return FormatHexUInt32(static_cast<uint32_t>(value));
}

// Formats a byte as "%02X".
std::string String::FormatByte(unsigned char value) {
  std::stringstream ss;
  ss << std::setfill('0') << std::setw(2) << std::hex << std::uppercase
	 << static_cast<unsigned int>(value);
  return ss.str();
}

// Converts the buffer in a stringstream to an std::string, converting NUL
// bytes to "\\0" along the way.
std::string StringStreamToString(::std::stringstream* ss) {
  const ::std::string& str = ss->str();
  const char* const start = str.c_str();
  const char* const end = start + str.length();

  std::string result;
  result.reserve(static_cast<size_t>(2 * (end - start)));
  for (const char* ch = start; ch != end; ++ch) {
	if (*ch == '\0') {
	  result += "\\0";  // Replaces NUL with "\\0";
	} else {
	  result += *ch;
	}
  }

  return result;
}

// Appends the user-supplied message to the Google-Test-generated message.
std::string AppendUserMessage(const std::string& gtest_msg,
							  const Message& user_msg) {
  // Appends the user message if it's non-empty.
  const std::string user_msg_string = user_msg.GetString();
  if (user_msg_string.empty()) {
	return gtest_msg;
  }

  return gtest_msg + "\n" + user_msg_string;
}

}  // namespace internal


namespace internal {

void ReportFailureInUnknownLocation(TestPartResult::Type result_type,
									const std::string& message) {
  // This function is a friend of UnitTest and as such has access to
  // AddTestPartResult.
  ::jmsd::cutf::UnitTest::GetInstance()->AddTestPartResult(
	  result_type,
	  nullptr,  // No info about the source file where the exception occurred.
	  -1,       // We have no info on which line caused the exception.
	  message,
	  "");  // No stack trace, either.
}

// The following routines generate an XML representation of a UnitTest
// object.
// GOOGLETEST_CM0009 DO NOT DELETE
//
// This is how Google Test concepts map to the DTD:
//
// <testsuites name="AllTests">        <-- corresponds to a UnitTest object
//   <testsuite name="testcase-name">  <-- corresponds to a TestSuite object
//     <testcase name="test-name">     <-- corresponds to a TestInfo object
//       <failure message="...">...</failure>
//       <failure message="...">...</failure>
//       <failure message="...">...</failure>
//                                     <-- individual assertion failures
//     </testcase>
//   </testsuite>
// </testsuites>

// Formats the given time in milliseconds as seconds.
std::string FormatTimeInMillisAsSeconds(TimeInMillis ms) {
  ::std::stringstream ss;
  ss << (static_cast<double>(ms) * 1e-3);
  return ss.str();
}

static bool PortableLocaltime(time_t seconds, struct tm* out) {
#if defined(_MSC_VER)
  return localtime_s(out, &seconds) == 0;
#elif defined(__MINGW32__) || defined(__MINGW64__)
  // MINGW <time.h> provides neither localtime_r nor localtime_s, but uses
  // Windows' localtime(), which has a thread-local tm buffer.
  struct tm* tm_ptr = localtime(&seconds);  // NOLINT
  if (tm_ptr == nullptr) return false;
  *out = *tm_ptr;
  return true;
#else
  return localtime_r(&seconds, out) != nullptr;
#endif
}

// Converts the given epoch time in milliseconds to a date string in the ISO
// 8601 format, without the timezone information.
std::string FormatEpochTimeInMillisAsIso8601(TimeInMillis ms) {
  struct tm time_struct;
  if (!PortableLocaltime(static_cast<time_t>(ms / 1000), &time_struct))
	return "";
  // YYYY-MM-DDThh:mm:ss
  return StreamableToString(time_struct.tm_year + 1900) + "-" +
	  String::FormatIntWidth2(time_struct.tm_mon + 1) + "-" +
	  String::FormatIntWidth2(time_struct.tm_mday) + "T" +
	  String::FormatIntWidth2(time_struct.tm_hour) + ":" +
	  String::FormatIntWidth2(time_struct.tm_min) + ":" +
	  String::FormatIntWidth2(time_struct.tm_sec);
}

// This class generates an JSON output file.
class JsonUnitTestResultPrinter : public ::jmsd::cutf::EmptyTestEventListener {
 public:
  explicit JsonUnitTestResultPrinter(const char* output_file);

  void OnTestIterationEnd(const ::jmsd::cutf::UnitTest& unit_test, int iteration) override;

  // Prints an JSON summary of all unit tests.
  static void PrintJsonTestList(::std::ostream* stream,
								const std::vector<::jmsd::cutf::TestSuite*>& test_suites);

 private:
  // Returns an JSON-escaped copy of the input string str.
  static std::string EscapeJson(const std::string& str);

  //// Verifies that the given attribute belongs to the given element and
  //// streams the attribute as JSON.
  static void OutputJsonKey(std::ostream* stream,
							const std::string& element_name,
							const std::string& name,
							const std::string& value,
							const std::string& indent,
							bool comma = true);
  static void OutputJsonKey(std::ostream* stream,
							const std::string& element_name,
							const std::string& name,
							int value,
							const std::string& indent,
							bool comma = true);

  // Streams a JSON representation of a TestInfo object.
  static void OutputJsonTestInfo(::std::ostream* stream,
								 const char* test_suite_name,
								 const ::jmsd::cutf::TestInfo& test_info);

  // Prints a JSON representation of a TestSuite object
  static void PrintJsonTestSuite(::std::ostream* stream,
								 const ::jmsd::cutf::TestSuite& test_suite);

  // Prints a JSON summary of unit_test to output stream out.
  static void PrintJsonUnitTest(::std::ostream* stream,
								const ::jmsd::cutf::UnitTest& unit_test);

  // Produces a string representing the test properties in a result as
  // a JSON dictionary.
  static std::string TestPropertiesAsJson(const ::jmsd::cutf::TestResult& result,
										  const std::string& indent);

  // The output file.
  const std::string output_file_;

  GTEST_DISALLOW_COPY_AND_ASSIGN_(JsonUnitTestResultPrinter);
};

// Creates a new JsonUnitTestResultPrinter.
JsonUnitTestResultPrinter::JsonUnitTestResultPrinter(const char* output_file)
	: output_file_(output_file) {
  if (output_file_.empty()) {
	GTEST_LOG_(FATAL) << "JSON output file may not be null";
  }
}

void JsonUnitTestResultPrinter::OnTestIterationEnd(const ::jmsd::cutf::UnitTest& unit_test,
												  int /*iteration*/) {
  FILE* jsonout = OpenFileForWriting(output_file_);
  std::stringstream stream;
  PrintJsonUnitTest(&stream, unit_test);
  fprintf(jsonout, "%s", StringStreamToString(&stream).c_str());
  fclose(jsonout);
}

// Returns an JSON-escaped copy of the input string str.
std::string JsonUnitTestResultPrinter::EscapeJson(const std::string& str) {
  Message m;

  for (size_t i = 0; i < str.size(); ++i) {
	const char ch = str[i];
	switch (ch) {
	  case '\\':
	  case '"':
	  case '/':
		m << '\\' << ch;
		break;
	  case '\b':
		m << "\\b";
		break;
	  case '\t':
		m << "\\t";
		break;
	  case '\n':
		m << "\\n";
		break;
	  case '\f':
		m << "\\f";
		break;
	  case '\r':
		m << "\\r";
		break;
	  default:
		if (ch < ' ') {
		  m << "\\u00" << String::FormatByte(static_cast<unsigned char>(ch));
		} else {
		  m << ch;
		}
		break;
	}
  }

  return m.GetString();
}

// The following routines generate an JSON representation of a UnitTest
// object.

// Formats the given time in milliseconds as seconds.
static std::string FormatTimeInMillisAsDuration(TimeInMillis ms) {
  ::std::stringstream ss;
  ss << (static_cast<double>(ms) * 1e-3) << "s";
  return ss.str();
}

// Converts the given epoch time in milliseconds to a date string in the
// RFC3339 format, without the timezone information.
static std::string FormatEpochTimeInMillisAsRFC3339(TimeInMillis ms) {
  struct tm time_struct;
  if (!PortableLocaltime(static_cast<time_t>(ms / 1000), &time_struct))
	return "";
  // YYYY-MM-DDThh:mm:ss
  return StreamableToString(time_struct.tm_year + 1900) + "-" +
	  String::FormatIntWidth2(time_struct.tm_mon + 1) + "-" +
	  String::FormatIntWidth2(time_struct.tm_mday) + "T" +
	  String::FormatIntWidth2(time_struct.tm_hour) + ":" +
	  String::FormatIntWidth2(time_struct.tm_min) + ":" +
	  String::FormatIntWidth2(time_struct.tm_sec) + "Z";
}

static inline std::string Indent(size_t width) {
  return std::string(width, ' ');
}

void JsonUnitTestResultPrinter::OutputJsonKey(
	std::ostream* stream,
	const std::string& element_name,
	const std::string& name,
	const std::string& value,
	const std::string& indent,
	bool comma) {
  const std::vector<std::string>& allowed_names =
	  ::jmsd::cutf::GetReservedOutputAttributesForElement(element_name);

  GTEST_CHECK_(std::find(allowed_names.begin(), allowed_names.end(), name) !=
				   allowed_names.end())
	  << "Key \"" << name << "\" is not allowed for value \"" << element_name
	  << "\".";

  *stream << indent << "\"" << name << "\": \"" << EscapeJson(value) << "\"";
  if (comma)
	*stream << ",\n";
}

void JsonUnitTestResultPrinter::OutputJsonKey(
	std::ostream* stream,
	const std::string& element_name,
	const std::string& name,
	int value,
	const std::string& indent,
	bool comma) {
  const std::vector<std::string>& allowed_names =
	  ::jmsd::cutf::GetReservedOutputAttributesForElement(element_name);

  GTEST_CHECK_(std::find(allowed_names.begin(), allowed_names.end(), name) !=
				   allowed_names.end())
	  << "Key \"" << name << "\" is not allowed for value \"" << element_name
	  << "\".";

  *stream << indent << "\"" << name << "\": " << StreamableToString(value);
  if (comma)
	*stream << ",\n";
}

// Prints a JSON representation of a TestInfo object.
void JsonUnitTestResultPrinter::OutputJsonTestInfo(::std::ostream* stream,
												   const char* test_suite_name,
												   const ::jmsd::cutf::TestInfo& test_info) {
  const ::jmsd::cutf::TestResult& result = *test_info.result();
  const std::string kTestsuite = "testcase";
  const std::string kIndent = Indent(10);

  *stream << Indent(8) << "{\n";
  OutputJsonKey(stream, kTestsuite, "name", test_info.name(), kIndent);

  if (test_info.value_param() != nullptr) {
	OutputJsonKey(stream, kTestsuite, "value_param", test_info.value_param(),
				  kIndent);
  }
  if (test_info.type_param() != nullptr) {
	OutputJsonKey(stream, kTestsuite, "type_param", test_info.type_param(),
				  kIndent);
  }
  if (GTEST_FLAG(list_tests)) {
	OutputJsonKey(stream, kTestsuite, "file", test_info.file(), kIndent);
	OutputJsonKey(stream, kTestsuite, "line", test_info.line(), kIndent, false);
	*stream << "\n" << Indent(8) << "}";
	return;
  }

  OutputJsonKey(stream, kTestsuite, "status",
				test_info.should_run() ? "RUN" : "NOTRUN", kIndent);
  OutputJsonKey(stream, kTestsuite, "result",
				test_info.should_run()
					? (result.Skipped() ? "SKIPPED" : "COMPLETED")
					: "SUPPRESSED",
				kIndent);
  OutputJsonKey(stream, kTestsuite, "timestamp",
				FormatEpochTimeInMillisAsRFC3339(result.start_timestamp()),
				kIndent);
  OutputJsonKey(stream, kTestsuite, "time",
				FormatTimeInMillisAsDuration(result.elapsed_time()), kIndent);
  OutputJsonKey(stream, kTestsuite, "classname", test_suite_name, kIndent,
				false);
  *stream << TestPropertiesAsJson(result, kIndent);

  int failures = 0;
  for (int i = 0; i < result.total_part_count(); ++i) {
	const TestPartResult& part = result.GetTestPartResult(i);
	if (part.failed()) {
	  *stream << ",\n";
	  if (++failures == 1) {
		*stream << kIndent << "\"" << "failures" << "\": [\n";
	  }
	  const std::string location =
		  internal::FormatCompilerIndependentFileLocation(part.file_name(),
														  part.line_number());
	  const std::string message = EscapeJson(location + "\n" + part.message());
	  *stream << kIndent << "  {\n"
			  << kIndent << "    \"failure\": \"" << message << "\",\n"
			  << kIndent << "    \"type\": \"\"\n"
			  << kIndent << "  }";
	}
  }

  if (failures > 0)
	*stream << "\n" << kIndent << "]";
  *stream << "\n" << Indent(8) << "}";
}

// Prints an JSON representation of a TestSuite object
void JsonUnitTestResultPrinter::PrintJsonTestSuite(
	std::ostream* stream, const ::jmsd::cutf::TestSuite& test_suite) {
  const std::string kTestsuite = "testsuite";
  const std::string kIndent = Indent(6);

  *stream << Indent(4) << "{\n";
  OutputJsonKey(stream, kTestsuite, "name", test_suite.name(), kIndent);
  OutputJsonKey(stream, kTestsuite, "tests", test_suite.reportable_test_count(),
				kIndent);
  if (!GTEST_FLAG(list_tests)) {
	OutputJsonKey(stream, kTestsuite, "failures",
				  test_suite.failed_test_count(), kIndent);
	OutputJsonKey(stream, kTestsuite, "disabled",
				  test_suite.reportable_disabled_test_count(), kIndent);
	OutputJsonKey(stream, kTestsuite, "errors", 0, kIndent);
	OutputJsonKey(
		stream, kTestsuite, "timestamp",
		FormatEpochTimeInMillisAsRFC3339(test_suite.start_timestamp()),
		kIndent);
	OutputJsonKey(stream, kTestsuite, "time",
				  FormatTimeInMillisAsDuration(test_suite.elapsed_time()),
				  kIndent, false);
	*stream << TestPropertiesAsJson(test_suite.ad_hoc_test_result(), kIndent)
			<< ",\n";
  }

  *stream << kIndent << "\"" << kTestsuite << "\": [\n";

  bool comma = false;
  for (int i = 0; i < test_suite.total_test_count(); ++i) {
	if (test_suite.GetTestInfo(i)->is_reportable()) {
	  if (comma) {
		*stream << ",\n";
	  } else {
		comma = true;
	  }
	  OutputJsonTestInfo(stream, test_suite.name(), *test_suite.GetTestInfo(i));
	}
  }
  *stream << "\n" << kIndent << "]\n" << Indent(4) << "}";
}

// Prints a JSON summary of unit_test to output stream out.
void JsonUnitTestResultPrinter::PrintJsonUnitTest(std::ostream* stream,
												  const ::jmsd::cutf::UnitTest& unit_test) {
  const std::string kTestsuites = "testsuites";
  const std::string kIndent = Indent(2);
  *stream << "{\n";

  OutputJsonKey(stream, kTestsuites, "tests", unit_test.reportable_test_count(),
				kIndent);
  OutputJsonKey(stream, kTestsuites, "failures", unit_test.failed_test_count(),
				kIndent);
  OutputJsonKey(stream, kTestsuites, "disabled",
				unit_test.reportable_disabled_test_count(), kIndent);
  OutputJsonKey(stream, kTestsuites, "errors", 0, kIndent);
  if (GTEST_FLAG(shuffle)) {
	OutputJsonKey(stream, kTestsuites, "random_seed", unit_test.random_seed(),
				  kIndent);
  }
  OutputJsonKey(stream, kTestsuites, "timestamp",
				FormatEpochTimeInMillisAsRFC3339(unit_test.start_timestamp()),
				kIndent);
  OutputJsonKey(stream, kTestsuites, "time",
				FormatTimeInMillisAsDuration(unit_test.elapsed_time()), kIndent,
				false);

  *stream << TestPropertiesAsJson(unit_test.ad_hoc_test_result(), kIndent)
		  << ",\n";

  OutputJsonKey(stream, kTestsuites, "name", "AllTests", kIndent);
  *stream << kIndent << "\"" << kTestsuites << "\": [\n";

  bool comma = false;
  for (int i = 0; i < unit_test.total_test_suite_count(); ++i) {
	if (unit_test.GetTestSuite(i)->reportable_test_count() > 0) {
	  if (comma) {
		*stream << ",\n";
	  } else {
		comma = true;
	  }
	  PrintJsonTestSuite(stream, *unit_test.GetTestSuite(i));
	}
  }

  *stream << "\n" << kIndent << "]\n" << "}\n";
}

void JsonUnitTestResultPrinter::PrintJsonTestList(
	std::ostream* stream, const std::vector<::jmsd::cutf::TestSuite*>& test_suites) {
  const std::string kTestsuites = "testsuites";
  const std::string kIndent = Indent(2);
  *stream << "{\n";
  int total_tests = 0;
  for (auto test_suite : test_suites) {
	total_tests += test_suite->total_test_count();
  }
  OutputJsonKey(stream, kTestsuites, "tests", total_tests, kIndent);

  OutputJsonKey(stream, kTestsuites, "name", "AllTests", kIndent);
  *stream << kIndent << "\"" << kTestsuites << "\": [\n";

  for (size_t i = 0; i < test_suites.size(); ++i) {
	if (i != 0) {
	  *stream << ",\n";
	}
	PrintJsonTestSuite(stream, *test_suites[i]);
  }

  *stream << "\n"
		  << kIndent << "]\n"
		  << "}\n";
}
// Produces a string representing the test properties in a result as
// a JSON dictionary.
std::string JsonUnitTestResultPrinter::TestPropertiesAsJson(
	const ::jmsd::cutf::TestResult& result, const std::string& indent) {
  Message attributes;
  for (int i = 0; i < result.test_property_count(); ++i) {
	const ::jmsd::cutf::TestProperty& property = result.GetTestProperty(i);
	attributes << ",\n" << indent << "\"" << property.key() << "\": "
			   << "\"" << EscapeJson(property.value()) << "\"";
  }
  return attributes.GetString();
}

// End JsonUnitTestResultPrinter

#if GTEST_CAN_STREAM_RESULTS_

// Checks if str contains '=', '&', '%' or '\n' characters. If yes,
// replaces them by "%xx" where xx is their hexadecimal value. For
// example, replaces "=" with "%3D".  This algorithm is O(strlen(str))
// in both time and space -- important as the input str may contain an
// arbitrarily long test failure message and stack trace.
std::string StreamingListener::UrlEncode(const char* str) {
  std::string result;
  result.reserve(strlen(str) + 1);
  for (char ch = *str; ch != '\0'; ch = *++str) {
	switch (ch) {
	  case '%':
	  case '=':
	  case '&':
	  case '\n':
		result.append("%" + String::FormatByte(static_cast<unsigned char>(ch)));
		break;
	  default:
		result.push_back(ch);
		break;
	}
  }
  return result;
}

// End of class Streaming Listener
#endif  // GTEST_CAN_STREAM_RESULTS__

// class OsStackTraceGetter

const char* const OsStackTraceGetterInterface::kElidedFramesMarker =
	"... " GTEST_NAME_ " internal frames ...";

std::string OsStackTraceGetter::CurrentStackTrace(int max_depth, int skip_count)
	GTEST_LOCK_EXCLUDED_(mutex_) {
#if GTEST_HAS_ABSL
  std::string result;

  if (max_depth <= 0) {
	return result;
  }

  max_depth = std::min(max_depth, kMaxStackTraceDepth);

  std::vector<void*> raw_stack(max_depth);
  // Skips the frames requested by the caller, plus this function.
  const int raw_stack_size =
	  absl::GetStackTrace(&raw_stack[0], max_depth, skip_count + 1);

  void* caller_frame = nullptr;
  {
	MutexLock lock(&mutex_);
	caller_frame = caller_frame_;
  }

  for (int i = 0; i < raw_stack_size; ++i) {
	if (raw_stack[i] == caller_frame &&
		!GTEST_FLAG(show_internal_stack_frames)) {
	  // Add a marker to the trace and stop adding frames.
	  absl::StrAppend(&result, kElidedFramesMarker, "\n");
	  break;
	}

	char tmp[1024];
	const char* symbol = "(unknown)";
	if (absl::Symbolize(raw_stack[i], tmp, sizeof(tmp))) {
	  symbol = tmp;
	}

	char line[1024];
	snprintf(line, sizeof(line), "  %p: %s\n", raw_stack[i], symbol);
	result += line;
  }

  return result;

#else  // !GTEST_HAS_ABSL
  static_cast<void>(max_depth);
  static_cast<void>(skip_count);
  return "";
#endif  // GTEST_HAS_ABSL
}

void OsStackTraceGetter::UponLeavingGTest() GTEST_LOCK_EXCLUDED_(mutex_) {
#if GTEST_HAS_ABSL
  void* caller_frame = nullptr;
  if (absl::GetStackTrace(&caller_frame, 1, 3) <= 0) {
	caller_frame = nullptr;
  }

  MutexLock lock(&mutex_);
  caller_frame_ = caller_frame;
#endif  // GTEST_HAS_ABSL
}

// Returns the current OS stack trace as an std::string.
//
// The maximum number of stack frames to be included is specified by
// the gtest_stack_trace_depth flag.  The skip_count parameter
// specifies the number of top frames to be skipped, which doesn't
// count against the number of frames to be included.
//
// For example, if Foo() calls Bar(), which in turn calls
// GetCurrentOsStackTraceExceptTop(..., 1), Foo() will be included in
// the trace but Bar() and GetCurrentOsStackTraceExceptTop() won't.
std::string GetCurrentOsStackTraceExceptTop(::jmsd::cutf::UnitTest* /*unit_test*/,
											int skip_count) {
  // We pass skip_count + 1 to skip this wrapper function in addition
  // to what the user really wants to skip.
  return ::jmsd::cutf::internal::GetUnitTestImpl()->CurrentOsStackTraceExceptTop(skip_count + 1);
}

// Used by the GTEST_SUPPRESS_UNREACHABLE_CODE_WARNING_BELOW_ macro to
// suppress unreachable code warnings.
namespace {
class ClassUniqueToAlwaysTrue {};
}

bool IsTrue(bool condition) { return condition; }

bool AlwaysTrue() {
#if GTEST_HAS_EXCEPTIONS
  // This condition is always false so AlwaysTrue() never actually throws,
  // but it makes the compiler think that it may throw.
  if (IsTrue(false))
	throw ClassUniqueToAlwaysTrue();
#endif  // GTEST_HAS_EXCEPTIONS
  return true;
}

// If *pstr starts with the given prefix, modifies *pstr to be right
// past the prefix and returns true; otherwise leaves *pstr unchanged
// and returns false.  None of pstr, *pstr, and prefix can be NULL.
bool SkipPrefix(const char* prefix, const char** pstr) {
  const size_t prefix_len = strlen(prefix);
  if (strncmp(*pstr, prefix, prefix_len) == 0) {
	*pstr += prefix_len;
	return true;
  }
  return false;
}

// Parses a string as a command line flag.  The string should have
// the format "--flag=value".  When def_optional is true, the "=value"
// part can be omitted.
//
// Returns the value of the flag, or NULL if the parsing failed.
static const char* ParseFlagValue(const char* str, const char* flag,
								  bool def_optional) {
  // str and flag must not be NULL.
  if (str == nullptr || flag == nullptr) return nullptr;

  // The flag must start with "--" followed by GTEST_FLAG_PREFIX_.
  const std::string flag_str = std::string("--") + GTEST_FLAG_PREFIX_ + flag;
  const size_t flag_len = flag_str.length();
  if (strncmp(str, flag_str.c_str(), flag_len) != 0) return nullptr;

  // Skips the flag name.
  const char* flag_end = str + flag_len;

  // When def_optional is true, it's OK to not have a "=value" part.
  if (def_optional && (flag_end[0] == '\0')) {
	return flag_end;
  }

  // If def_optional is true and there are more characters after the
  // flag name, or if def_optional is false, there must be a '=' after
  // the flag name.
  if (flag_end[0] != '=') return nullptr;

  // Returns the string after "=".
  return flag_end + 1;
}

// Parses a string for a bool flag, in the form of either
// "--flag=value" or "--flag".
//
// In the former case, the value is taken as true as long as it does
// not start with '0', 'f', or 'F'.
//
// In the latter case, the value is taken as true.
//
// On success, stores the value of the flag in *value, and returns
// true.  On failure, returns false without changing *value.
static bool ParseBoolFlag(const char* str, const char* flag, bool* value) {
  // Gets the value of the flag as a string.
  const char* const value_str = ParseFlagValue(str, flag, true);

  // Aborts if the parsing failed.
  if (value_str == nullptr) return false;

  // Converts the string value to a bool.
  *value = !(*value_str == '0' || *value_str == 'f' || *value_str == 'F');
  return true;
}

// Parses a string for an int32_t flag, in the form of "--flag=value".
//
// On success, stores the value of the flag in *value, and returns
// true.  On failure, returns false without changing *value.
bool ParseInt32Flag(const char* str, const char* flag, int32_t* value) {
  // Gets the value of the flag as a string.
  const char* const value_str = ParseFlagValue(str, flag, false);

  // Aborts if the parsing failed.
  if (value_str == nullptr) return false;

  // Sets *value to the value of the flag.
  return ParseInt32(Message() << "The value of flag --" << flag,
					value_str, value);
}

// Parses a string for a string flag, in the form of "--flag=value".
//
// On success, stores the value of the flag in *value, and returns
// true.  On failure, returns false without changing *value.
template <typename String>
static bool ParseStringFlag(const char* str, const char* flag, String* value) {
  // Gets the value of the flag as a string.
  const char* const value_str = ParseFlagValue(str, flag, false);

  // Aborts if the parsing failed.
  if (value_str == nullptr) return false;

  // Sets *value to the value of the flag.
  *value = value_str;
  return true;
}

// Determines whether a string has a prefix that Google Test uses for its
// flags, i.e., starts with GTEST_FLAG_PREFIX_ or GTEST_FLAG_PREFIX_DASH_.
// If Google Test detects that a command line flag has its prefix but is not
// recognized, it will print its help message. Flags starting with
// GTEST_INTERNAL_PREFIX_ followed by "internal_" are considered Google Test
// internal flags and do not trigger the help message.
static bool HasGoogleTestFlagPrefix(const char* str) {
  return (SkipPrefix("--", &str) ||
		  SkipPrefix("-", &str) ||
		  SkipPrefix("/", &str)) &&
		 !SkipPrefix(GTEST_FLAG_PREFIX_ "internal_", &str) &&
		 (SkipPrefix(GTEST_FLAG_PREFIX_, &str) ||
		  SkipPrefix(GTEST_FLAG_PREFIX_DASH_, &str));
}

static const char kColorEncodedHelpMessage[] =
"This program contains tests written using " GTEST_NAME_ ". You can use the\n"
"following command line flags to control its behavior:\n"
"\n"
"Test Selection:\n"
"  @G--" GTEST_FLAG_PREFIX_ "list_tests@D\n"
"      List the names of all tests instead of running them. The name of\n"
"      TEST(Foo, Bar) is \"Foo.Bar\".\n"
"  @G--" GTEST_FLAG_PREFIX_ "filter=@YPOSTIVE_PATTERNS"
	"[@G-@YNEGATIVE_PATTERNS]@D\n"
"      Run only the tests whose name matches one of the positive patterns but\n"
"      none of the negative patterns. '?' matches any single character; '*'\n"
"      matches any substring; ':' separates two patterns.\n"
"  @G--" GTEST_FLAG_PREFIX_ "also_run_disabled_tests@D\n"
"      Run all disabled tests too.\n"
"\n"
"Test Execution:\n"
"  @G--" GTEST_FLAG_PREFIX_ "repeat=@Y[COUNT]@D\n"
"      Run the tests repeatedly; use a negative count to repeat forever.\n"
"  @G--" GTEST_FLAG_PREFIX_ "shuffle@D\n"
"      Randomize tests' orders on every iteration.\n"
"  @G--" GTEST_FLAG_PREFIX_ "random_seed=@Y[NUMBER]@D\n"
"      Random number seed to use for shuffling test orders (between 1 and\n"
"      99999, or 0 to use a seed based on the current time).\n"
"\n"
"Test Output:\n"
"  @G--" GTEST_FLAG_PREFIX_ "color=@Y(@Gyes@Y|@Gno@Y|@Gauto@Y)@D\n"
"      Enable/disable colored output. The default is @Gauto@D.\n"
"  -@G-" GTEST_FLAG_PREFIX_ "print_time=0@D\n"
"      Don't print the elapsed time of each test.\n"
"  @G--" GTEST_FLAG_PREFIX_ "output=@Y(@Gjson@Y|@Gxml@Y)[@G:@YDIRECTORY_PATH@G"
	GTEST_PATH_SEP_ "@Y|@G:@YFILE_PATH]@D\n"
"      Generate a JSON or XML report in the given directory or with the given\n"
"      file name. @YFILE_PATH@D defaults to @Gtest_detail.xml@D.\n"
# if GTEST_CAN_STREAM_RESULTS_
"  @G--" GTEST_FLAG_PREFIX_ "stream_result_to=@YHOST@G:@YPORT@D\n"
"      Stream test results to the given server.\n"
# endif  // GTEST_CAN_STREAM_RESULTS_
"\n"
"Assertion Behavior:\n"
# if GTEST_HAS_DEATH_TEST && !GTEST_OS_WINDOWS
"  @G--" GTEST_FLAG_PREFIX_ "death_test_style=@Y(@Gfast@Y|@Gthreadsafe@Y)@D\n"
"      Set the default death test style.\n"
# endif  // GTEST_HAS_DEATH_TEST && !GTEST_OS_WINDOWS
"  @G--" GTEST_FLAG_PREFIX_ "break_on_failure@D\n"
"      Turn assertion failures into debugger break-points.\n"
"  @G--" GTEST_FLAG_PREFIX_ "throw_on_failure@D\n"
"      Turn assertion failures into C++ exceptions for use by an external\n"
"      test framework.\n"
"  @G--" GTEST_FLAG_PREFIX_ "catch_exceptions=0@D\n"
"      Do not report exceptions as test failures. Instead, allow them\n"
"      to crash the program or throw a pop-up (on Windows).\n"
"\n"
"Except for @G--" GTEST_FLAG_PREFIX_ "list_tests@D, you can alternatively set "
	"the corresponding\n"
"environment variable of a flag (all letters in upper-case). For example, to\n"
"disable colored text output, you can either specify @G--" GTEST_FLAG_PREFIX_
	"color=no@D or set\n"
"the @G" GTEST_FLAG_PREFIX_UPPER_ "COLOR@D environment variable to @Gno@D.\n"
"\n"
"For more information, please read the " GTEST_NAME_ " documentation at\n"
"@G" GTEST_PROJECT_URL_ "@D. If you find a bug in " GTEST_NAME_ "\n"
"(not one in your own code or tests), please report it to\n"
"@G<" GTEST_DEV_EMAIL_ ">@D.\n";

static bool ParseGoogleTestFlag(const char* const arg) {
  return ParseBoolFlag(arg, kAlsoRunDisabledTestsFlag,
					   &GTEST_FLAG(also_run_disabled_tests)) ||
	  ParseBoolFlag(arg, kBreakOnFailureFlag,
					&GTEST_FLAG(break_on_failure)) ||
	  ParseBoolFlag(arg, kCatchExceptionsFlag,
					&GTEST_FLAG(catch_exceptions)) ||
	  ParseStringFlag(arg, kColorFlag, &GTEST_FLAG(color)) ||
	  ParseStringFlag(arg, kDeathTestStyleFlag,
					  &GTEST_FLAG(death_test_style)) ||
	  ParseBoolFlag(arg, kDeathTestUseFork,
					&GTEST_FLAG(death_test_use_fork)) ||
	  ParseStringFlag(arg, kFilterFlag, &GTEST_FLAG(filter)) ||
	  ParseStringFlag(arg, kInternalRunDeathTestFlag,
					  &GTEST_FLAG(internal_run_death_test)) ||
	  ParseBoolFlag(arg, kListTestsFlag, &GTEST_FLAG(list_tests)) ||
	  ParseStringFlag(arg, kOutputFlag, &GTEST_FLAG(output)) ||
	  ParseBoolFlag(arg, kPrintTimeFlag, &GTEST_FLAG(print_time)) ||
	  ParseBoolFlag(arg, kPrintUTF8Flag, &GTEST_FLAG(print_utf8)) ||
	  ParseInt32Flag(arg, kRandomSeedFlag, &GTEST_FLAG(random_seed)) ||
	  ParseInt32Flag(arg, kRepeatFlag, &GTEST_FLAG(repeat)) ||
	  ParseBoolFlag(arg, kShuffleFlag, &GTEST_FLAG(shuffle)) ||
	  ParseInt32Flag(arg, kStackTraceDepthFlag,
					 &GTEST_FLAG(stack_trace_depth)) ||
	  ParseStringFlag(arg, kStreamResultToFlag,
					  &GTEST_FLAG(stream_result_to)) ||
	  ParseBoolFlag(arg, kThrowOnFailureFlag,
					&GTEST_FLAG(throw_on_failure));
}

#if GTEST_USE_OWN_FLAGFILE_FLAG_
static void LoadFlagsFromFile(const std::string& path) {
  FILE* flagfile = posix::FOpen(path.c_str(), "r");
  if (!flagfile) {
	GTEST_LOG_(FATAL) << "Unable to open file \"" << GTEST_FLAG(flagfile)
					  << "\"";
  }
  std::string contents(ReadEntireFile(flagfile));
  posix::FClose(flagfile);
  std::vector<std::string> lines;
  SplitString(contents, '\n', &lines);
  for (size_t i = 0; i < lines.size(); ++i) {
	if (lines[i].empty())
	  continue;
	if (!ParseGoogleTestFlag(lines[i].c_str()))
	  g_help_flag = true;
  }
}
#endif  // GTEST_USE_OWN_FLAGFILE_FLAG_

// Parses the command line for Google Test flags, without initializing
// other parts of Google Test.  The type parameter CharType can be
// instantiated to either char or wchar_t.
template <typename CharType>
void ParseGoogleTestFlagsOnlyImpl(int* argc, CharType** argv) {
  for (int i = 1; i < *argc; i++) {
	const std::string arg_string = StreamableToString(argv[i]);
	const char* const arg = arg_string.c_str();

	using internal::ParseBoolFlag;
	using internal::ParseInt32Flag;
	using internal::ParseStringFlag;

	bool remove_flag = false;
	if (ParseGoogleTestFlag(arg)) {
	  remove_flag = true;
#if GTEST_USE_OWN_FLAGFILE_FLAG_
	} else if (ParseStringFlag(arg, kFlagfileFlag, &GTEST_FLAG(flagfile))) {
	  LoadFlagsFromFile(GTEST_FLAG(flagfile));
	  remove_flag = true;
#endif  // GTEST_USE_OWN_FLAGFILE_FLAG_
	} else if (arg_string == "--help" || arg_string == "-h" ||
			   arg_string == "-?" || arg_string == "/?" ||
			   HasGoogleTestFlagPrefix(arg)) {
	  // Both help flag and unrecognized Google Test flags (excluding
	  // internal ones) trigger help display.
	  g_help_flag = true;
	}

	if (remove_flag) {
	  // Shift the remainder of the argv list left by one.  Note
	  // that argv has (*argc + 1) elements, the last one always being
	  // NULL.  The following loop moves the trailing NULL element as
	  // well.
	  for (int j = i; j != *argc; j++) {
		argv[j] = argv[j + 1];
	  }

	  // Decrements the argument count.
	  (*argc)--;

	  // We also need to decrement the iterator as we just removed
	  // an element.
	  i--;
	}
  }

  if (g_help_flag) {
	// We print the help here instead of in RUN_ALL_TESTS(), as the
	// latter may not be called at all if the user is using Google
	// Test with another testing framework.
	::jmsd::cutf::internal::PrintColorEncoded(kColorEncodedHelpMessage);
  }
}

// Parses the command line for Google Test flags, without initializing
// other parts of Google Test.
void ParseGoogleTestFlagsOnly(int* argc, char** argv) {
  ParseGoogleTestFlagsOnlyImpl(argc, argv);

  // Fix the value of *_NSGetArgc() on macOS, but if and only if
  // *_NSGetArgv() == argv
  // Only applicable to char** version of argv
#if GTEST_OS_MAC
#ifndef GTEST_OS_IOS
  if (*_NSGetArgv() == argv) {
	*_NSGetArgc() = *argc;
  }
#endif
#endif
}
void ParseGoogleTestFlagsOnly(int* argc, wchar_t** argv) {
  ParseGoogleTestFlagsOnlyImpl(argc, argv);
}

// The internal implementation of InitGoogleTest().
//
// The type parameter CharType can be instantiated to either char or
// wchar_t.
template <typename CharType>
void InitGoogleTestImpl(int* argc, CharType** argv) {
  // We don't want to run the initialization code twice.
  if (GTestIsInitialized()) return;

  if (*argc <= 0) return;

  g_argvs.clear();
  for (int i = 0; i != *argc; i++) {
	g_argvs.push_back(StreamableToString(argv[i]));
  }

#if GTEST_HAS_ABSL
  absl::InitializeSymbolizer(g_argvs[0].c_str());
#endif  // GTEST_HAS_ABSL

  ParseGoogleTestFlagsOnly(argc, argv);
  ::jmsd::cutf::internal::GetUnitTestImpl()->PostFlagParsingInit();
}

}  // namespace internal

// Initializes Google Test.  This must be called before calling
// RUN_ALL_TESTS().  In particular, it parses a command line for the
// flags that Google Test recognizes.  Whenever a Google Test flag is
// seen, it is removed from argv, and *argc is decremented.
//
// No value is returned.  Instead, the Google Test flag variables are
// updated.
//
// Calling the function for the second time has no user-visible effect.
void InitGoogleTest(int* argc, char** argv) {
#if defined(GTEST_CUSTOM_INIT_GOOGLE_TEST_FUNCTION_)
  GTEST_CUSTOM_INIT_GOOGLE_TEST_FUNCTION_(argc, argv);
#else  // defined(GTEST_CUSTOM_INIT_GOOGLE_TEST_FUNCTION_)
  internal::InitGoogleTestImpl(argc, argv);
#endif  // defined(GTEST_CUSTOM_INIT_GOOGLE_TEST_FUNCTION_)
}

// This overloaded version can be used in Windows programs compiled in
// UNICODE mode.
void InitGoogleTest(int* argc, wchar_t** argv) {
#if defined(GTEST_CUSTOM_INIT_GOOGLE_TEST_FUNCTION_)
  GTEST_CUSTOM_INIT_GOOGLE_TEST_FUNCTION_(argc, argv);
#else  // defined(GTEST_CUSTOM_INIT_GOOGLE_TEST_FUNCTION_)
  internal::InitGoogleTestImpl(argc, argv);
#endif  // defined(GTEST_CUSTOM_INIT_GOOGLE_TEST_FUNCTION_)
}

// This overloaded version can be used on Arduino/embedded platforms where
// there is no argc/argv.
void InitGoogleTest() {
  // Since Arduino doesn't have a command line, fake out the argc/argv arguments
  int argc = 1;
  const auto arg0 = "dummy";
  char* argv0 = const_cast<char*>(arg0);
  char** argv = &argv0;

#if defined(GTEST_CUSTOM_INIT_GOOGLE_TEST_FUNCTION_)
  GTEST_CUSTOM_INIT_GOOGLE_TEST_FUNCTION_(&argc, argv);
#else  // defined(GTEST_CUSTOM_INIT_GOOGLE_TEST_FUNCTION_)
  internal::InitGoogleTestImpl(&argc, argv);
#endif  // defined(GTEST_CUSTOM_INIT_GOOGLE_TEST_FUNCTION_)
}

std::string TempDir() {
#if defined(GTEST_CUSTOM_TEMPDIR_FUNCTION_)
  return GTEST_CUSTOM_TEMPDIR_FUNCTION_();
#endif

#if GTEST_OS_WINDOWS_MOBILE
  return "\\temp\\";
#elif GTEST_OS_WINDOWS
  const char* temp_dir = internal::posix::GetEnv("TEMP");
  if (temp_dir == nullptr || temp_dir[0] == '\0')
	return "\\temp\\";
  else if (temp_dir[strlen(temp_dir) - 1] == '\\')
	return temp_dir;
  else
	return std::string(temp_dir) + "\\";
#elif GTEST_OS_LINUX_ANDROID
  return "/sdcard/";
#else
  return "/tmp/";
#endif  // GTEST_OS_WINDOWS_MOBILE
}


}  // namespace testing
