#pragma once

// The Google C++ Testing and Mocking Framework (Google Test)
//
// This header file defines the public API for Google Test.  It should be
// included by any test program that uses Google Test.
//
// IMPORTANT NOTE: Due to limitation of the C++ language, we have to
// leave some internal implementation details in this header file.
// They are clearly marked by comments like this:
//
//   // INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
//
// Such code is NOT meant to be used by a user directly, and is subject
// to CHANGE WITHOUT NOTICE.  Therefore DO NOT DEPEND ON IT in a user
// program!
//
// Acknowledgment: Google Test borrowed the idea of automatic test
// registration from Barthelemy Dagenais' (barthelemy@prologique.com)
// easyUnit framework.

// GOOGLETEST_CM0001 DO NOT DELETE

#include "Unit_test.h"


#include <cstddef>
#include <limits>
#include <memory>
#include <ostream>
#include <type_traits>
#include <vector>

#include "gtest/internal/gtest-internal.h"
#include "gtest/internal/gtest-string.h"
#include "gtest/gtest-death-test.h"
#include "gtest/gtest-matchers.h"
#include "gtest/gtest-message.h"
#include "gtest/gtest-param-test.h"
#include "gtest/gtest-printers.h"
#include "gtest/gtest_prod.h"
#include "gtest/gtest-test-part.h"
#include "gtest/gtest-typed-test.h"


#include "Test_property.h"
#include "Assertion_result.h"
#include "Test.h"
#include "internal/Exception_handling.h"

#include "Unit_test.hxx"
#include "internal/Unit_test_impl.hxx"
#include "internal/Default_global_test_part_result_reporter.hxx"


GTEST_DISABLE_MSC_WARNINGS_PUSH_(4251 \
/* class A needs to have dll-interface to be used by clients of class B */)

namespace testing {

// Silence C4100 (unreferenced formal parameter) and 4805
// unsafe mix of type 'const int' and type 'const bool'
#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable:4805)
# pragma warning(disable:4100)
#endif


namespace internal {

////class AssertHelper;
////class DefaultGlobalTestPartResultReporter;
//class ExecDeathTest;
//class NoExecDeathTest;
//class FinalSuccessChecker;
//class GTestFlagSaver;
//class StreamingListenerTest;
////class TestResultAccessor;
//class TestEventListenersAccessor;
//class TestEventRepeater;
//class UnitTestRecordPropertyTestHelper;
////class WindowsDeathTest;
////class FuchsiaDeathTest;

//void ReportFailureInUnknownLocation(TestPartResult::Type result_type,
//									const std::string& message);

}  // namespace internal
}  // namespace testing

// Includes the auto-generated header that implements a family of generic
// predicate assertion macros. This include comes late because it relies on
// APIs declared above.
#include "gtest/gtest_pred_impl.h"

namespace testing {

// An Environment object is capable of setting up and tearing down an
// environment.  You should subclass this to define your own
// environment(s).
//
// An Environment object does the set-up and tear-down in virtual
// methods SetUp() and TearDown() instead of the constructor and the
// destructor, as:
//
//   1. You cannot safely throw from a destructor.  This is a problem
//      as in some cases Google Test is used where exceptions are enabled, and
//      we may want to implement ASSERT_* using exceptions where they are
//      available.
//   2. You cannot use ASSERT_* directly in a constructor or
//      destructor.
class Environment {
 public:
  // The d'tor is virtual as we need to subclass Environment.
  virtual ~Environment() {}

  // Override this to define how to set up the environment.
  virtual void SetUp() {}

  // Override this to define how to tear down the environment.
  virtual void TearDown() {}
 private:
  // If you see an error about overriding the following function or
  // about it being private, you have mis-spelled SetUp() as Setup().
  struct Setup_should_be_spelled_SetUp {};
  virtual Setup_should_be_spelled_SetUp* Setup() { return nullptr; }
};

#if GTEST_HAS_EXCEPTIONS

// Exception which can be thrown from TestEventListener::OnTestPartResult.
class GTEST_API_ AssertionException
	: public ::jmsd::cutf::internal::GoogleTestFailureException {
 public:
  explicit AssertionException(const TestPartResult& result)
	  : GoogleTestFailureException(result) {}
};

#endif  // GTEST_HAS_EXCEPTIONS


// Initializes Google Test.  This must be called before calling
// RUN_ALL_TESTS().  In particular, it parses a command line for the
// flags that Google Test recognizes.  Whenever a Google Test flag is
// seen, it is removed from argv, and *argc is decremented.
//
// No value is returned.  Instead, the Google Test flag variables are
// updated.
//
// Calling the function for the second time has no user-visible effect.
GTEST_API_ void InitGoogleTest(int* argc, char** argv);

// This overloaded version can be used in Windows programs compiled in
// UNICODE mode.
GTEST_API_ void InitGoogleTest(int* argc, wchar_t** argv);

// This overloaded version can be used on Arduino/embedded platforms where
// there is no argc/argv.
GTEST_API_ void InitGoogleTest();

namespace internal {

// Separate the error generating code from the code path to reduce the stack
// frame size of CmpHelperEQ. This helps reduce the overhead of some sanitizers
// when calling EXPECT_* in a tight loop.
template <typename T1, typename T2>
::jmsd::cutf::AssertionResult CmpHelperEQFailure(const char* lhs_expression,
								   const char* rhs_expression,
								   const T1& lhs, const T2& rhs) {
  return EqFailure(lhs_expression,
				   rhs_expression,
				   FormatForComparisonFailureMessage(lhs, rhs),
				   FormatForComparisonFailureMessage(rhs, lhs),
				   false);
}

// This block of code defines operator==/!=
// to block lexical scope lookup.
// It prevents using invalid operator==/!= defined at namespace scope.
struct faketype {};
inline bool operator==(faketype, faketype) { return true; }
inline bool operator!=(faketype, faketype) { return false; }

// The helper function for {ASSERT|EXPECT}_EQ.
template <typename T1, typename T2>
::jmsd::cutf::AssertionResult CmpHelperEQ(const char* lhs_expression,
							const char* rhs_expression,
							const T1& lhs,
							const T2& rhs) {
  if (lhs == rhs) {
	return AssertionSuccess();
  }

  return CmpHelperEQFailure(lhs_expression, rhs_expression, lhs, rhs);
}

// With this overloaded version, we allow anonymous enums to be used
// in {ASSERT|EXPECT}_EQ when compiled with gcc 4, as anonymous enums
// can be implicitly cast to BiggestInt.
GTEST_API_ ::jmsd::cutf::AssertionResult CmpHelperEQ(const char* lhs_expression,
									   const char* rhs_expression,
									   BiggestInt lhs,
									   BiggestInt rhs);

class EqHelper {
 public:
  // This templatized version is for the general case.
  template <
	  typename T1, typename T2,
	  // Disable this overload for cases where one argument is a pointer
	  // and the other is the null pointer constant.
	  typename std::enable_if<!std::is_integral<T1>::value ||
							  !std::is_pointer<T2>::value>::type* = nullptr>
  static ::jmsd::cutf::AssertionResult Compare(const char* lhs_expression,
								 const char* rhs_expression, const T1& lhs,
								 const T2& rhs) {
	return CmpHelperEQ(lhs_expression, rhs_expression, lhs, rhs);
  }

  // With this overloaded version, we allow anonymous enums to be used
  // in {ASSERT|EXPECT}_EQ when compiled with gcc 4, as anonymous
  // enums can be implicitly cast to BiggestInt.
  //
  // Even though its body looks the same as the above version, we
  // cannot merge the two, as it will make anonymous enums unhappy.
  static ::jmsd::cutf::AssertionResult Compare(const char* lhs_expression,
								 const char* rhs_expression,
								 BiggestInt lhs,
								 BiggestInt rhs) {
	return CmpHelperEQ(lhs_expression, rhs_expression, lhs, rhs);
  }

  template <typename T>
  static ::jmsd::cutf::AssertionResult Compare(
	  const char* lhs_expression, const char* rhs_expression,
	  // Handle cases where '0' is used as a null pointer literal.
	  std::nullptr_t /* lhs */, T* rhs) {
	// We already know that 'lhs' is a null pointer.
	return CmpHelperEQ(lhs_expression, rhs_expression, static_cast<T*>(nullptr),
					   rhs);
  }
};

// Separate the error generating code from the code path to reduce the stack
// frame size of CmpHelperOP. This helps reduce the overhead of some sanitizers
// when calling EXPECT_OP in a tight loop.
template <typename T1, typename T2>
::jmsd::cutf::AssertionResult CmpHelperOpFailure(const char* expr1, const char* expr2,
								   const T1& val1, const T2& val2,
								   const char* op) {
  return AssertionFailure()
		 << "Expected: (" << expr1 << ") " << op << " (" << expr2
		 << "), actual: " << FormatForComparisonFailureMessage(val1, val2)
		 << " vs " << FormatForComparisonFailureMessage(val2, val1);
}

// A macro for implementing the helper functions needed to implement
// ASSERT_?? and EXPECT_??.  It is here just to avoid copy-and-paste
// of similar code.
//
// For each templatized helper function, we also define an overloaded
// version for BiggestInt in order to reduce code bloat and allow
// anonymous enums to be used with {ASSERT|EXPECT}_?? when compiled
// with gcc 4.
//
// INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.

#define GTEST_IMPL_CMP_HELPER_(op_name, op) \
template <typename T1, typename T2> \
::jmsd::cutf::AssertionResult CmpHelper##op_name(const char* expr1, const char* expr2, \
								   const T1& val1, const T2& val2) { \
  if (val1 op val2) { \
	return AssertionSuccess(); \
  } else { \
	return CmpHelperOpFailure(expr1, expr2, val1, val2, #op); \
  } \
} \
GTEST_API_ ::jmsd::cutf::AssertionResult CmpHelper##op_name( \
	const char* expr1, const char* expr2, BiggestInt val1, BiggestInt val2)

// INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.

// Implements the helper function for {ASSERT|EXPECT}_NE
GTEST_IMPL_CMP_HELPER_(NE, !=);
// Implements the helper function for {ASSERT|EXPECT}_LE
GTEST_IMPL_CMP_HELPER_(LE, <=);
// Implements the helper function for {ASSERT|EXPECT}_LT
GTEST_IMPL_CMP_HELPER_(LT, <);
// Implements the helper function for {ASSERT|EXPECT}_GE
GTEST_IMPL_CMP_HELPER_(GE, >=);
// Implements the helper function for {ASSERT|EXPECT}_GT
GTEST_IMPL_CMP_HELPER_(GT, >);

#undef GTEST_IMPL_CMP_HELPER_

// The helper function for {ASSERT|EXPECT}_STREQ.
//
// INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
GTEST_API_ ::jmsd::cutf::AssertionResult CmpHelperSTREQ(const char* s1_expression,
										  const char* s2_expression,
										  const char* s1,
										  const char* s2);

// The helper function for {ASSERT|EXPECT}_STRCASEEQ.
//
// INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
GTEST_API_ ::jmsd::cutf::AssertionResult CmpHelperSTRCASEEQ(const char* s1_expression,
											  const char* s2_expression,
											  const char* s1,
											  const char* s2);

// The helper function for {ASSERT|EXPECT}_STRNE.
//
// INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
GTEST_API_ ::jmsd::cutf::AssertionResult CmpHelperSTRNE(const char* s1_expression,
										  const char* s2_expression,
										  const char* s1,
										  const char* s2);

// The helper function for {ASSERT|EXPECT}_STRCASENE.
//
// INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
GTEST_API_ ::jmsd::cutf::AssertionResult CmpHelperSTRCASENE(const char* s1_expression,
											  const char* s2_expression,
											  const char* s1,
											  const char* s2);


// Helper function for *_STREQ on wide strings.
//
// INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
GTEST_API_ ::jmsd::cutf::AssertionResult CmpHelperSTREQ(const char* s1_expression,
										  const char* s2_expression,
										  const wchar_t* s1,
										  const wchar_t* s2);

// Helper function for *_STRNE on wide strings.
//
// INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
GTEST_API_ ::jmsd::cutf::AssertionResult CmpHelperSTRNE(const char* s1_expression,
										  const char* s2_expression,
										  const wchar_t* s1,
										  const wchar_t* s2);

}  // namespace internal

// IsSubstring() and IsNotSubstring() are intended to be used as the
// first argument to {EXPECT,ASSERT}_PRED_FORMAT2(), not by
// themselves.  They check whether needle is a substring of haystack
// (NULL is considered a substring of itself only), and return an
// appropriate error message when they fail.
//
// The {needle,haystack}_expr arguments are the stringified
// expressions that generated the two real arguments.
GTEST_API_ ::jmsd::cutf::AssertionResult IsSubstring(
	const char* needle_expr, const char* haystack_expr,
	const char* needle, const char* haystack);
GTEST_API_ ::jmsd::cutf::AssertionResult IsSubstring(
	const char* needle_expr, const char* haystack_expr,
	const wchar_t* needle, const wchar_t* haystack);
GTEST_API_ ::jmsd::cutf::AssertionResult IsNotSubstring(
	const char* needle_expr, const char* haystack_expr,
	const char* needle, const char* haystack);
GTEST_API_ ::jmsd::cutf::AssertionResult IsNotSubstring(
	const char* needle_expr, const char* haystack_expr,
	const wchar_t* needle, const wchar_t* haystack);
GTEST_API_ ::jmsd::cutf::AssertionResult IsSubstring(
	const char* needle_expr, const char* haystack_expr,
	const ::std::string& needle, const ::std::string& haystack);
GTEST_API_ ::jmsd::cutf::AssertionResult IsNotSubstring(
	const char* needle_expr, const char* haystack_expr,
	const ::std::string& needle, const ::std::string& haystack);

#if GTEST_HAS_STD_WSTRING
GTEST_API_ ::jmsd::cutf::AssertionResult IsSubstring(
	const char* needle_expr, const char* haystack_expr,
	const ::std::wstring& needle, const ::std::wstring& haystack);
GTEST_API_ ::jmsd::cutf::AssertionResult IsNotSubstring(
	const char* needle_expr, const char* haystack_expr,
	const ::std::wstring& needle, const ::std::wstring& haystack);
#endif  // GTEST_HAS_STD_WSTRING

namespace internal {

// Helper template function for comparing floating-points.
//
// Template parameter:
//
//   RawType: the raw floating-point type (either float or double)
//
// INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
template <typename RawType>
::jmsd::cutf::AssertionResult CmpHelperFloatingPointEQ(const char* lhs_expression,
										 const char* rhs_expression,
										 RawType lhs_value,
										 RawType rhs_value) {
  const FloatingPoint<RawType> lhs(lhs_value), rhs(rhs_value);

  if (lhs.AlmostEquals(rhs)) {
	return AssertionSuccess();
  }

  ::std::stringstream lhs_ss;
  lhs_ss << std::setprecision(std::numeric_limits<RawType>::digits10 + 2)
		 << lhs_value;

  ::std::stringstream rhs_ss;
  rhs_ss << std::setprecision(std::numeric_limits<RawType>::digits10 + 2)
		 << rhs_value;

  return EqFailure(lhs_expression,
				   rhs_expression,
				   StringStreamToString(&lhs_ss),
				   StringStreamToString(&rhs_ss),
				   false);
}

// Helper function for implementing ASSERT_NEAR.
//
// INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
GTEST_API_ ::jmsd::cutf::AssertionResult DoubleNearPredFormat(const char* expr1,
												const char* expr2,
												const char* abs_error_expr,
												double val1,
												double val2,
												double abs_error);

// INTERNAL IMPLEMENTATION - DO NOT USE IN USER CODE.
// A class that enables one to stream messages to assertion macros
class GTEST_API_ AssertHelper {
 public:
  // Constructor.
  AssertHelper(TestPartResult::Type type,
			   const char* file,
			   int line,
			   const char* message);
  ~AssertHelper();

  // Message assignment is a semantic trick to enable assertion
  // streaming; see the GTEST_MESSAGE_ macro below.
  void operator=(const Message& message) const;

 private:
  // We put our data in a struct so that the size of the AssertHelper class can
  // be as small as possible.  This is important because gcc is incapable of
  // re-using stack space even for temporary variables, so every EXPECT_EQ
  // reserves stack space for another AssertHelper.
  struct AssertHelperData {
	AssertHelperData(TestPartResult::Type t,
					 const char* srcfile,
					 int line_num,
					 const char* msg)
		: type(t), file(srcfile), line(line_num), message(msg) { }

	TestPartResult::Type const type;
	const char* const file;
	int const line;
	std::string const message;

   private:
	GTEST_DISALLOW_COPY_AND_ASSIGN_(AssertHelperData);
  };

  AssertHelperData* const data_;

  GTEST_DISALLOW_COPY_AND_ASSIGN_(AssertHelper);
};

enum GTestColor { COLOR_DEFAULT, COLOR_RED, COLOR_GREEN, COLOR_YELLOW };

GTEST_API_ GTEST_ATTRIBUTE_PRINTF_(2, 3) void ColoredPrintf(GTestColor color,
															const char* fmt,
															...);

}  // namespace internal

// The pure interface class that all value-parameterized tests inherit from.
// A value-parameterized class must inherit from both ::testing::Test and
// ::testing::WithParamInterface. In most cases that just means inheriting
// from ::testing::TestWithParam, but more complicated test hierarchies
// may need to inherit from Test and WithParamInterface at different levels.
//
// This interface has support for accessing the test parameter value via
// the GetParam() method.
//
// Use it with one of the parameter generator defining functions, like Range(),
// Values(), ValuesIn(), Bool(), and Combine().
//
// class FooTest : public ::testing::TestWithParam<int> {
//  protected:
//   FooTest() {
//     // Can use GetParam() here.
//   }
//   ~FooTest() override {
//     // Can use GetParam() here.
//   }
//   void SetUp() override {
//     // Can use GetParam() here.
//   }
//   void TearDown override {
//     // Can use GetParam() here.
//   }
// };
// TEST_P(FooTest, DoesBar) {
//   // Can use GetParam() method here.
//   Foo foo;
//   ASSERT_TRUE(foo.DoesBar(GetParam()));
// }
// INSTANTIATE_TEST_SUITE_P(OneToTenRange, FooTest, ::testing::Range(1, 10));

template <typename T>
class WithParamInterface {
 public:
  typedef T ParamType;
  virtual ~WithParamInterface() {}

  // The current parameter value. Is also available in the test fixture's
  // constructor.
  static const ParamType& GetParam() {
	GTEST_CHECK_(parameter_ != nullptr)
		<< "GetParam() can only be called inside a value-parameterized test "
		<< "-- did you intend to write TEST_P instead of TEST_F?";
	return *parameter_;
  }

 private:
  // Sets parameter value. The caller is responsible for making sure the value
  // remains alive and unchanged throughout the current test.
  static void SetParam(const ParamType* parameter) {
	parameter_ = parameter;
  }

  // Static value used for accessing parameter during a test lifetime.
  static const ParamType* parameter_;

  // TestClass must be a subclass of WithParamInterface<T> and Test.
  template <class TestClass> friend class internal::ParameterizedTestFactory;
};

template <typename T>
const T* WithParamInterface<T>::parameter_ = nullptr;

// Most value-parameterized classes can ignore the existence of
// WithParamInterface, and can just inherit from ::testing::TestWithParam.

template <typename T>
class TestWithParam : public ::jmsd::cutf::Test, public WithParamInterface<T> {
};

// Macros for indicating success/failure in test code.

// Skips test in runtime.
// Skipping test aborts current function.
// Skipped tests are neither successful nor failed.
#define GTEST_SKIP() GTEST_SKIP_("")

// ADD_FAILURE unconditionally adds a failure to the current test.
// SUCCEED generates a success - it doesn't automatically make the
// current test successful, as a test is only successful when it has
// no failure.
//
// EXPECT_* verifies that a certain condition is satisfied.  If not,
// it behaves like ADD_FAILURE.  In particular:
//
//   EXPECT_TRUE  verifies that a Boolean condition is true.
//   EXPECT_FALSE verifies that a Boolean condition is false.
//
// FAIL and ASSERT_* are similar to ADD_FAILURE and EXPECT_*, except
// that they will also abort the current function on failure.  People
// usually want the fail-fast behavior of FAIL and ASSERT_*, but those
// writing data-driven tests often find themselves using ADD_FAILURE
// and EXPECT_* more.

// Generates a nonfatal failure with a generic message.
#define ADD_FAILURE() GTEST_NONFATAL_FAILURE_("Failed")

// Generates a nonfatal failure at the given source file location with
// a generic message.
#define ADD_FAILURE_AT(file, line) \
  GTEST_MESSAGE_AT_(file, line, "Failed", \
					::testing::TestPartResult::kNonFatalFailure)

// Generates a fatal failure with a generic message.
#define GTEST_FAIL() GTEST_FATAL_FAILURE_("Failed")

// Like GTEST_FAIL(), but at the given source file location.
#define GTEST_FAIL_AT(file, line)         \
  GTEST_MESSAGE_AT_(file, line, "Failed", \
					::testing::TestPartResult::kFatalFailure)

// Define this macro to 1 to omit the definition of FAIL(), which is a
// generic name and clashes with some other libraries.
#if !GTEST_DONT_DEFINE_FAIL
# define FAIL() GTEST_FAIL()
#endif

// Generates a success with a generic message.
#define GTEST_SUCCEED() GTEST_SUCCESS_("Succeeded")

// Define this macro to 1 to omit the definition of SUCCEED(), which
// is a generic name and clashes with some other libraries.
#if !GTEST_DONT_DEFINE_SUCCEED
# define SUCCEED() GTEST_SUCCEED()
#endif

// Macros for testing exceptions.
//
//    * {ASSERT|EXPECT}_THROW(statement, expected_exception):
//         Tests that the statement throws the expected exception.
//    * {ASSERT|EXPECT}_NO_THROW(statement):
//         Tests that the statement doesn't throw any exception.
//    * {ASSERT|EXPECT}_ANY_THROW(statement):
//         Tests that the statement throws an exception.

#define EXPECT_THROW(statement, expected_exception) \
  GTEST_TEST_THROW_(statement, expected_exception, GTEST_NONFATAL_FAILURE_)
#define EXPECT_NO_THROW(statement) \
  GTEST_TEST_NO_THROW_(statement, GTEST_NONFATAL_FAILURE_)
#define EXPECT_ANY_THROW(statement) \
  GTEST_TEST_ANY_THROW_(statement, GTEST_NONFATAL_FAILURE_)
#define ASSERT_THROW(statement, expected_exception) \
  GTEST_TEST_THROW_(statement, expected_exception, GTEST_FATAL_FAILURE_)
#define ASSERT_NO_THROW(statement) \
  GTEST_TEST_NO_THROW_(statement, GTEST_FATAL_FAILURE_)
#define ASSERT_ANY_THROW(statement) \
  GTEST_TEST_ANY_THROW_(statement, GTEST_FATAL_FAILURE_)

// Boolean assertions. Condition can be either a Boolean expression or an
// AssertionResult. For more information on how to use AssertionResult with
// these macros see comments on that class.
#define EXPECT_TRUE(condition) \
  GTEST_TEST_BOOLEAN_(condition, #condition, false, true, \
					  GTEST_NONFATAL_FAILURE_)
#define EXPECT_FALSE(condition) \
  GTEST_TEST_BOOLEAN_(!(condition), #condition, true, false, \
					  GTEST_NONFATAL_FAILURE_)
#define ASSERT_TRUE(condition) \
  GTEST_TEST_BOOLEAN_(condition, #condition, false, true, \
					  GTEST_FATAL_FAILURE_)
#define ASSERT_FALSE(condition) \
  GTEST_TEST_BOOLEAN_(!(condition), #condition, true, false, \
					  GTEST_FATAL_FAILURE_)

// Macros for testing equalities and inequalities.
//
//    * {ASSERT|EXPECT}_EQ(v1, v2): Tests that v1 == v2
//    * {ASSERT|EXPECT}_NE(v1, v2): Tests that v1 != v2
//    * {ASSERT|EXPECT}_LT(v1, v2): Tests that v1 < v2
//    * {ASSERT|EXPECT}_LE(v1, v2): Tests that v1 <= v2
//    * {ASSERT|EXPECT}_GT(v1, v2): Tests that v1 > v2
//    * {ASSERT|EXPECT}_GE(v1, v2): Tests that v1 >= v2
//
// When they are not, Google Test prints both the tested expressions and
// their actual values.  The values must be compatible built-in types,
// or you will get a compiler error.  By "compatible" we mean that the
// values can be compared by the respective operator.
//
// Note:
//
//   1. It is possible to make a user-defined type work with
//   {ASSERT|EXPECT}_??(), but that requires overloading the
//   comparison operators and is thus discouraged by the Google C++
//   Usage Guide.  Therefore, you are advised to use the
//   {ASSERT|EXPECT}_TRUE() macro to assert that two objects are
//   equal.
//
//   2. The {ASSERT|EXPECT}_??() macros do pointer comparisons on
//   pointers (in particular, C strings).  Therefore, if you use it
//   with two C strings, you are testing how their locations in memory
//   are related, not how their content is related.  To compare two C
//   strings by content, use {ASSERT|EXPECT}_STR*().
//
//   3. {ASSERT|EXPECT}_EQ(v1, v2) is preferred to
//   {ASSERT|EXPECT}_TRUE(v1 == v2), as the former tells you
//   what the actual value is when it fails, and similarly for the
//   other comparisons.
//
//   4. Do not depend on the order in which {ASSERT|EXPECT}_??()
//   evaluate their arguments, which is undefined.
//
//   5. These macros evaluate their arguments exactly once.
//
// Examples:
//
//   EXPECT_NE(Foo(), 5);
//   EXPECT_EQ(a_pointer, NULL);
//   ASSERT_LT(i, array_size);
//   ASSERT_GT(records.size(), 0) << "There is no record left.";

#define EXPECT_EQ(val1, val2) \
  EXPECT_PRED_FORMAT2(::testing::internal::EqHelper::Compare, val1, val2)
#define EXPECT_NE(val1, val2) \
  EXPECT_PRED_FORMAT2(::testing::internal::CmpHelperNE, val1, val2)
#define EXPECT_LE(val1, val2) \
  EXPECT_PRED_FORMAT2(::testing::internal::CmpHelperLE, val1, val2)
#define EXPECT_LT(val1, val2) \
  EXPECT_PRED_FORMAT2(::testing::internal::CmpHelperLT, val1, val2)
#define EXPECT_GE(val1, val2) \
  EXPECT_PRED_FORMAT2(::testing::internal::CmpHelperGE, val1, val2)
#define EXPECT_GT(val1, val2) \
  EXPECT_PRED_FORMAT2(::testing::internal::CmpHelperGT, val1, val2)

#define GTEST_ASSERT_EQ(val1, val2) \
  ASSERT_PRED_FORMAT2(::testing::internal::EqHelper::Compare, val1, val2)
#define GTEST_ASSERT_NE(val1, val2) \
  ASSERT_PRED_FORMAT2(::testing::internal::CmpHelperNE, val1, val2)
#define GTEST_ASSERT_LE(val1, val2) \
  ASSERT_PRED_FORMAT2(::testing::internal::CmpHelperLE, val1, val2)
#define GTEST_ASSERT_LT(val1, val2) \
  ASSERT_PRED_FORMAT2(::testing::internal::CmpHelperLT, val1, val2)
#define GTEST_ASSERT_GE(val1, val2) \
  ASSERT_PRED_FORMAT2(::testing::internal::CmpHelperGE, val1, val2)
#define GTEST_ASSERT_GT(val1, val2) \
  ASSERT_PRED_FORMAT2(::testing::internal::CmpHelperGT, val1, val2)

// Define macro GTEST_DONT_DEFINE_ASSERT_XY to 1 to omit the definition of
// ASSERT_XY(), which clashes with some users' own code.

#if !GTEST_DONT_DEFINE_ASSERT_EQ
# define ASSERT_EQ(val1, val2) GTEST_ASSERT_EQ(val1, val2)
#endif

#if !GTEST_DONT_DEFINE_ASSERT_NE
# define ASSERT_NE(val1, val2) GTEST_ASSERT_NE(val1, val2)
#endif

#if !GTEST_DONT_DEFINE_ASSERT_LE
# define ASSERT_LE(val1, val2) GTEST_ASSERT_LE(val1, val2)
#endif

#if !GTEST_DONT_DEFINE_ASSERT_LT
# define ASSERT_LT(val1, val2) GTEST_ASSERT_LT(val1, val2)
#endif

#if !GTEST_DONT_DEFINE_ASSERT_GE
# define ASSERT_GE(val1, val2) GTEST_ASSERT_GE(val1, val2)
#endif

#if !GTEST_DONT_DEFINE_ASSERT_GT
# define ASSERT_GT(val1, val2) GTEST_ASSERT_GT(val1, val2)
#endif

// C-string Comparisons.  All tests treat NULL and any non-NULL string
// as different.  Two NULLs are equal.
//
//    * {ASSERT|EXPECT}_STREQ(s1, s2):     Tests that s1 == s2
//    * {ASSERT|EXPECT}_STRNE(s1, s2):     Tests that s1 != s2
//    * {ASSERT|EXPECT}_STRCASEEQ(s1, s2): Tests that s1 == s2, ignoring case
//    * {ASSERT|EXPECT}_STRCASENE(s1, s2): Tests that s1 != s2, ignoring case
//
// For wide or narrow string objects, you can use the
// {ASSERT|EXPECT}_??() macros.
//
// Don't depend on the order in which the arguments are evaluated,
// which is undefined.
//
// These macros evaluate their arguments exactly once.

#define EXPECT_STREQ(s1, s2) \
  EXPECT_PRED_FORMAT2(::testing::internal::CmpHelperSTREQ, s1, s2)
#define EXPECT_STRNE(s1, s2) \
  EXPECT_PRED_FORMAT2(::testing::internal::CmpHelperSTRNE, s1, s2)
#define EXPECT_STRCASEEQ(s1, s2) \
  EXPECT_PRED_FORMAT2(::testing::internal::CmpHelperSTRCASEEQ, s1, s2)
#define EXPECT_STRCASENE(s1, s2)\
  EXPECT_PRED_FORMAT2(::testing::internal::CmpHelperSTRCASENE, s1, s2)

#define ASSERT_STREQ(s1, s2) \
  ASSERT_PRED_FORMAT2(::testing::internal::CmpHelperSTREQ, s1, s2)
#define ASSERT_STRNE(s1, s2) \
  ASSERT_PRED_FORMAT2(::testing::internal::CmpHelperSTRNE, s1, s2)
#define ASSERT_STRCASEEQ(s1, s2) \
  ASSERT_PRED_FORMAT2(::testing::internal::CmpHelperSTRCASEEQ, s1, s2)
#define ASSERT_STRCASENE(s1, s2)\
  ASSERT_PRED_FORMAT2(::testing::internal::CmpHelperSTRCASENE, s1, s2)

// Macros for comparing floating-point numbers.
//
//    * {ASSERT|EXPECT}_FLOAT_EQ(val1, val2):
//         Tests that two float values are almost equal.
//    * {ASSERT|EXPECT}_DOUBLE_EQ(val1, val2):
//         Tests that two double values are almost equal.
//    * {ASSERT|EXPECT}_NEAR(v1, v2, abs_error):
//         Tests that v1 and v2 are within the given distance to each other.
//
// Google Test uses ULP-based comparison to automatically pick a default
// error bound that is appropriate for the operands.  See the
// FloatingPoint template class in gtest-internal.h if you are
// interested in the implementation details.

#define EXPECT_FLOAT_EQ(val1, val2)\
  EXPECT_PRED_FORMAT2(::testing::internal::CmpHelperFloatingPointEQ<float>, \
					  val1, val2)

#define EXPECT_DOUBLE_EQ(val1, val2)\
  EXPECT_PRED_FORMAT2(::testing::internal::CmpHelperFloatingPointEQ<double>, \
					  val1, val2)

#define ASSERT_FLOAT_EQ(val1, val2)\
  ASSERT_PRED_FORMAT2(::testing::internal::CmpHelperFloatingPointEQ<float>, \
					  val1, val2)

#define ASSERT_DOUBLE_EQ(val1, val2)\
  ASSERT_PRED_FORMAT2(::testing::internal::CmpHelperFloatingPointEQ<double>, \
					  val1, val2)

#define EXPECT_NEAR(val1, val2, abs_error)\
  EXPECT_PRED_FORMAT3(::testing::internal::DoubleNearPredFormat, \
					  val1, val2, abs_error)

#define ASSERT_NEAR(val1, val2, abs_error)\
  ASSERT_PRED_FORMAT3(::testing::internal::DoubleNearPredFormat, \
					  val1, val2, abs_error)

// These predicate format functions work on floating-point values, and
// can be used in {ASSERT|EXPECT}_PRED_FORMAT2*(), e.g.
//
//   EXPECT_PRED_FORMAT2(testing::DoubleLE, Foo(), 5.0);

// Asserts that val1 is less than, or almost equal to, val2.  Fails
// otherwise.  In particular, it fails if either val1 or val2 is NaN.
GTEST_API_ ::jmsd::cutf::AssertionResult FloatLE(const char* expr1, const char* expr2,
								   float val1, float val2);
GTEST_API_ ::jmsd::cutf::AssertionResult DoubleLE(const char* expr1, const char* expr2,
									double val1, double val2);


#if GTEST_OS_WINDOWS

// Macros that test for HRESULT failure and success, these are only useful
// on Windows, and rely on Windows SDK macros and APIs to compile.
//
//    * {ASSERT|EXPECT}_HRESULT_{SUCCEEDED|FAILED}(expr)
//
// When expr unexpectedly fails or succeeds, Google Test prints the
// expected result and the actual result with both a human-readable
// string representation of the error, if available, as well as the
// hex result code.
# define EXPECT_HRESULT_SUCCEEDED(expr) \
	EXPECT_PRED_FORMAT1(::testing::internal::IsHRESULTSuccess, (expr))

# define ASSERT_HRESULT_SUCCEEDED(expr) \
	ASSERT_PRED_FORMAT1(::testing::internal::IsHRESULTSuccess, (expr))

# define EXPECT_HRESULT_FAILED(expr) \
	EXPECT_PRED_FORMAT1(::testing::internal::IsHRESULTFailure, (expr))

# define ASSERT_HRESULT_FAILED(expr) \
	ASSERT_PRED_FORMAT1(::testing::internal::IsHRESULTFailure, (expr))

#endif  // GTEST_OS_WINDOWS

// Macros that execute statement and check that it doesn't generate new fatal
// failures in the current thread.
//
//   * {ASSERT|EXPECT}_NO_FATAL_FAILURE(statement);
//
// Examples:
//
//   EXPECT_NO_FATAL_FAILURE(Process());
//   ASSERT_NO_FATAL_FAILURE(Process()) << "Process() failed";
//
#define ASSERT_NO_FATAL_FAILURE(statement) \
	GTEST_TEST_NO_FATAL_FAILURE_(statement, GTEST_FATAL_FAILURE_)
#define EXPECT_NO_FATAL_FAILURE(statement) \
	GTEST_TEST_NO_FATAL_FAILURE_(statement, GTEST_NONFATAL_FAILURE_)

//// Causes a trace (including the given source file path and line number,
//// and the given message) to be included in every test failure message generated
//// by code in the scope of the lifetime of an instance of this class. The effect
//// is undone with the destruction of the instance.
////
//// The message argument can be anything streamable to std::ostream.
////
//// Example:
////   testing::ScopedTrace trace("file.cc", 123, "message");
////
//class GTEST_API_ ScopedTrace {
// public:
//  // The c'tor pushes the given source file location and message onto
//  // a trace stack maintained by Google Test.

//  // Template version. Uses Message() to convert the values into strings.
//  // Slow, but flexible.
//  template <typename T>
//  ScopedTrace(const char* file, int line, const T& message) {
//	PushTrace(file, line, (Message() << message).GetString());
//  }

//  // Optimize for some known types.
//  ScopedTrace(const char* file, int line, const char* message) {
//	PushTrace(file, line, message ? message : "(null)");
//  }

//  ScopedTrace(const char* file, int line, const std::string& message) {
//	PushTrace(file, line, message);
//  }

//  // The d'tor pops the info pushed by the c'tor.
//  //
//  // Note that the d'tor is not virtual in order to be efficient.
//  // Don't inherit from ScopedTrace!
//  ~ScopedTrace();

// private:
//  void PushTrace(const char* file, int line, std::string message);

//  GTEST_DISALLOW_COPY_AND_ASSIGN_(ScopedTrace);
//} GTEST_ATTRIBUTE_UNUSED_;  // A ScopedTrace object does its job in its
//							// c'tor and d'tor.  Therefore it doesn't
//							// need to be used otherwise.

// Causes a trace (including the source file path, the current line
// number, and the given message) to be included in every test failure
// message generated by code in the current scope.  The effect is
// undone when the control leaves the current scope.
//
// The message argument can be anything streamable to std::ostream.
//
// In the implementation, we include the current line number as part
// of the dummy variable name, thus allowing multiple SCOPED_TRACE()s
// to appear in the same block - as long as they are on different
// lines.
//
// Assuming that each thread maintains its own stack of traces.
// Therefore, a SCOPED_TRACE() would (correctly) only affect the
// assertions in its own thread.
#define SCOPED_TRACE(message) \
  ::testing::ScopedTrace GTEST_CONCAT_TOKEN_(gtest_trace_, __LINE__)(\
	__FILE__, __LINE__, (message))

// Compile-time assertion for type equality.
// StaticAssertTypeEq<type1, type2>() compiles if and only if type1 and type2
// are the same type.  The value it returns is not interesting.
//
// Instead of making StaticAssertTypeEq a class template, we make it a
// function template that invokes a helper class template.  This
// prevents a user from misusing StaticAssertTypeEq<T1, T2> by
// defining objects of that type.
//
// CAVEAT:
//
// When used inside a method of a class template,
// StaticAssertTypeEq<T1, T2>() is effective ONLY IF the method is
// instantiated.  For example, given:
//
//   template <typename T> class Foo {
//    public:
//     void Bar() { testing::StaticAssertTypeEq<int, T>(); }
//   };
//
// the code:
//
//   void Test1() { Foo<bool> foo; }
//
// will NOT generate a compiler error, as Foo<bool>::Bar() is never
// actually instantiated.  Instead, you need:
//
//   void Test2() { Foo<bool> foo; foo.Bar(); }
//
// to cause a compiler error.
template <typename T1, typename T2>
constexpr bool StaticAssertTypeEq() noexcept {
  static_assert(std::is_same<T1, T2>::value, "T1 and T2 are not the same type");
  return true;
}

// Defines a test.
//
// The first parameter is the name of the test suite, and the second
// parameter is the name of the test within the test suite.
//
// The convention is to end the test suite name with "Test".  For
// example, a test suite for the Foo class can be named FooTest.
//
// Test code should appear between braces after an invocation of
// this macro.  Example:
//
//   TEST(FooTest, InitializesCorrectly) {
//     Foo foo;
//     EXPECT_TRUE(foo.StatusIsOK());
//   }

// Note that we call GetTestTypeId() instead of GetTypeId<
// ::testing::Test>() here to get the type ID of testing::Test.  This
// is to work around a suspected linker bug when using Google Test as
// a framework on Mac OS X.  The bug causes GetTypeId<
// ::testing::Test>() to return different values depending on whether
// the call is from the Google Test framework itself or from user test
// code.  GetTestTypeId() is guaranteed to always return the same
// value, as it always calls GetTypeId<>() from the Google Test
// framework.
#define GTEST_TEST(test_suite_name, test_name)             \
  GTEST_TEST_(test_suite_name, test_name, ::testing::Test, \
			  ::testing::internal::GetTestTypeId())

// Define this macro to 1 to omit the definition of TEST(), which
// is a generic name and clashes with some other libraries.
#if !GTEST_DONT_DEFINE_TEST
#define TEST(test_suite_name, test_name) GTEST_TEST(test_suite_name, test_name)
#endif

// Defines a test that uses a test fixture.
//
// The first parameter is the name of the test fixture class, which
// also doubles as the test suite name.  The second parameter is the
// name of the test within the test suite.
//
// A test fixture class must be declared earlier.  The user should put
// the test code between braces after using this macro.  Example:
//
//   class FooTest : public testing::Test {
//    protected:
//     void SetUp() override { b_.AddElement(3); }
//
//     Foo a_;
//     Foo b_;
//   };
//
//   TEST_F(FooTest, InitializesCorrectly) {
//     EXPECT_TRUE(a_.StatusIsOK());
//   }
//
//   TEST_F(FooTest, ReturnsElementCountCorrectly) {
//     EXPECT_EQ(a_.size(), 0);
//     EXPECT_EQ(b_.size(), 1);
//   }
//
// GOOGLETEST_CM0011 DO NOT DELETE
#define TEST_F(test_fixture, test_name)\
  GTEST_TEST_(test_fixture, test_name, test_fixture, \
			  ::testing::internal::GetTypeId<test_fixture>())

// Returns a path to temporary directory.
// Tries to determine an appropriate directory for the platform.
GTEST_API_ std::string TempDir();

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

// Dynamically registers a test with the framework.
//
// This is an advanced API only to be used when the `TEST` macros are
// insufficient. The macros should be preferred when possible, as they avoid
// most of the complexity of calling this function.
//
// The `factory` argument is a factory callable (move-constructible) object or
// function pointer that creates a new instance of the Test object. It
// handles ownership to the caller. The signature of the callable is
// `Fixture*()`, where `Fixture` is the test fixture class for the test. All
// tests registered with the same `test_suite_name` must return the same
// fixture type. This is checked at runtime.
//
// The framework will infer the fixture class from the factory and will call
// the `SetUpTestSuite` and `TearDownTestSuite` for it.
//
// Must be called before `RUN_ALL_TESTS()` is invoked, otherwise behavior is
// undefined.
//
// Use case example:
//
// class MyFixture : public ::testing::Test {
//  public:
//   // All of these optional, just like in regular macro usage.
//   static void SetUpTestSuite() { ... }
//   static void TearDownTestSuite() { ... }
//   void SetUp() override { ... }
//   void TearDown() override { ... }
// };
//
// class MyTest : public MyFixture {
//  public:
//   explicit MyTest(int data) : data_(data) {}
//   void TestBody() override { ... }
//
//  private:
//   int data_;
// };
//
// void RegisterMyTests(const std::vector<int>& values) {
//   for (int v : values) {
//     ::testing::RegisterTest(
//         "MyFixture", ("Test" + std::to_string(v)).c_str(), nullptr,
//         std::to_string(v).c_str(),
//         __FILE__, __LINE__,
//         // Important to use the fixture type as the return type here.
//         [=]() -> MyFixture* { return new MyTest(v); });
//   }
// }
// ...
// int main(int argc, char** argv) {
//   std::vector<int> values_to_test = LoadValuesFromConfig();
//   RegisterMyTests(values_to_test);
//   ...
//   return RUN_ALL_TESTS();
// }
//
template< int &... ExplicitParameterBarrier, typename Factory >
::jmsd::cutf::TestInfo* RegisterTest(
	const char* test_suite_name,
	const char* test_name,
	const char* type_param,
	const char* value_param,
	const char* file,
	int line,
	Factory factory )
{
  using TestT = typename ::std::remove_pointer< decltype( factory() ) >::type;

  class FactoryImpl : public internal::TestFactoryBase {
   public:
	explicit FactoryImpl(Factory f) : factory_(std::move(f)) {}
	::jmsd::cutf::Test* CreateTest() override { return factory_(); }

   private:
	Factory factory_;
  };

  return internal::MakeAndRegisterTestInfo(
	  test_suite_name, test_name, type_param, value_param,
	  internal::CodeLocation(file, line), internal::GetTypeId<TestT>(),
	  internal::SuiteApiResolver<TestT>::GetSetUpCaseOrSuite(file, line),
	  internal::SuiteApiResolver<TestT>::GetTearDownCaseOrSuite(file, line),
	  new FactoryImpl{std::move(factory)});
}

}  // namespace testing


GTEST_DISABLE_MSC_WARNINGS_POP_()  //  4251
