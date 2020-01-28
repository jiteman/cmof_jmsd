#pragma once
// The Google C++ Testing and Mocking Framework (Google Test)
//
// This header file declares functions and macros used internally by
// Google Test.  They are subject to change without notice.

// GOOGLETEST_CM0001 DO NOT DELETE


#include "gtest-filepath.h"
#include "gtest-string.h"
#include "gtest-type-util.h"

#include "Make_and_register_test_info.h"

//#include "gtest/gtest-message.h"

#include "Streamable_to_string.hin"

#include "gtest/Test_info.hxx"
#include "gtest/Unit_test.hxx"
#include "gtest/Assertion_result.hxx"
#include "gtest/Test.hxx"


#include <ctype.h>
#include <float.h>
#include <string.h>
#include <cstdint>
#include <iomanip>
#include <limits>
#include <map>
#include <set>
#include <string>
#include <type_traits>
#include <vector>


#include "gtest/internal/gtest-port.h"


#if GTEST_OS_LINUX
# include <stdlib.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>
#endif  // GTEST_OS_LINUX

#if GTEST_HAS_EXCEPTIONS
# include <stdexcept>
#endif


// Due to C++ preprocessor weirdness, we need double indirection to
// concatenate two tokens when one of them is __LINE__.  Writing
//
//   foo ## __LINE__
//
// will result in the token foo__LINE__, instead of foo followed by
// the current line number.  For more details, see
// http://www.parashift.com/c++-faq-lite/misc-technical-issues.html#faq-39.6
#define GTEST_CONCAT_TOKEN_(foo, bar) GTEST_CONCAT_TOKEN_IMPL_(foo, bar)
#define GTEST_CONCAT_TOKEN_IMPL_(foo, bar) foo ## bar

// Stringifies its argument.
#define GTEST_STRINGIFY_(name) #name

namespace proto2 { class Message; }

namespace testing {


template <typename T>
::std::string PrintToString(const T& value);

namespace internal {

struct TraceInfo;                      // Information about a trace point.
class TestInfoImpl;                    // Opaque implementation of TestInfo

// An IgnoredValue object can be implicitly constructed from ANY value.
class IgnoredValue {
  struct Sink {};
 public:
  // This constructor template allows any value to be implicitly
  // converted to IgnoredValue.  The object has no data member and
  // doesn't try to remember anything about the argument.  We
  // deliberately omit the 'explicit' keyword in order to allow the
  // conversion to be implicit.
  // Disable the conversion if T already has a magical conversion operator.
  // Otherwise we get ambiguity.
  template <typename T,
			typename std::enable_if<!std::is_convertible<T, Sink>::value,
									int>::type = 0>
  IgnoredValue(const T& /* ignored */) {}  // NOLINT(runtime/explicit)
};

// Appends the user-supplied message to the Google-Test-generated message.
JMSD_DEPRECATED_GTEST_API_ std::string AppendUserMessage(
	const std::string& gtest_msg, const ::jmsd::cutf::Message& user_msg);

// Calculate the diff between 'left' and 'right' and return it in unified diff
// format.
// If not null, stores in 'total_line_count' the total number of lines found
// in left + right.
JMSD_DEPRECATED_GTEST_API_ std::string DiffStrings(const std::string& left,
								   const std::string& right,
								   size_t* total_line_count);

// Constructs a failure message for Boolean assertions such as EXPECT_TRUE.
JMSD_DEPRECATED_GTEST_API_ std::string GetBoolAssertionFailureMessage(
	const ::jmsd::cutf::AssertionResult& assertion_result,
	const char* expression_text,
	const char* actual_predicate_value,
	const char* expected_predicate_value);

template <typename T>
class TypeIdHelper {
 public:
  // dummy_ must not have a const type.  Otherwise an overly eager
  // compiler (e.g. MSVC 7.1 & 8.0) may try to merge
  // TypeIdHelper<T>::dummy_ for different Ts as an "optimization".
  static bool dummy_;
};

template <typename T>
bool TypeIdHelper<T>::dummy_ = false;

// GetTypeId<T>() returns the ID of type T.  Different values will be
// returned for different types.  Calling the function twice with the
// same type argument is guaranteed to return the same ID.
template <typename T>
TypeId GetTypeId() {
  // The compiler is required to allocate a different
  // TypeIdHelper<T>::dummy_ variable for each T used to instantiate
  // the template.  Therefore, the address of dummy_ is guaranteed to
  // be unique.
  return &(TypeIdHelper<T>::dummy_);
}

// Returns the type ID of ::testing::Test.  Always call this instead
// of GetTypeId< ::testing::Test>() to get the type ID of
// ::testing::Test, as the latter may give the wrong result due to a
// suspected linker bug when compiling Google Test as a Mac OS X
// framework.
JMSD_DEPRECATED_GTEST_API_ TypeId GetTestTypeId();

// Defines the abstract factory interface that creates instances
// of a Test object.
class TestFactoryBase {
 public:
  virtual ~TestFactoryBase() {}

  // Creates a test instance to run. The instance is both created and destroyed
  // within TestInfoImpl::Run()
  virtual ::jmsd::cutf::Test* CreateTest() = 0;

 protected:
  TestFactoryBase() {}

 private:
  GTEST_DISALLOW_COPY_AND_ASSIGN_(TestFactoryBase);
};

// This class provides implementation of TeastFactoryBase interface.
// It is used in TEST and TEST_F macros.
template <class TestClass>
class TestFactoryImpl : public TestFactoryBase {
 public:
  ::jmsd::cutf::Test* CreateTest() override { return new TestClass; }
};

#if GTEST_OS_WINDOWS

// Predicate-formatters for implementing the HRESULT checking macros
// {ASSERT|EXPECT}_HRESULT_{SUCCEEDED|FAILED}
// We pass a long instead of HRESULT to avoid causing an
// include dependency for the HRESULT type.
JMSD_DEPRECATED_GTEST_API_ ::jmsd::cutf::AssertionResult IsHRESULTSuccess(const char* expr, long hr);
JMSD_DEPRECATED_GTEST_API_ ::jmsd::cutf::AssertionResult IsHRESULTFailure(const char* expr, long hr);

#endif  // GTEST_OS_WINDOWS

struct CodeLocation {
  CodeLocation(const std::string& a_file, int a_line)
	  : file(a_file), line(a_line) {}

  std::string file;
  int line;
};

//  Helper to identify which setup function for TestCase / TestSuite to call.
//  Only one function is allowed, either TestCase or TestSute but not both.

// Utility functions to help SuiteApiResolver
using SetUpTearDownSuiteFuncType = void (*)();

inline SetUpTearDownSuiteFuncType GetNotDefaultOrNull(
	SetUpTearDownSuiteFuncType a, SetUpTearDownSuiteFuncType def) {
  return a == def ? nullptr : a;
}

template <typename T>
//  Note that SuiteApiResolver inherits from T because
//  SetUpTestSuite()/TearDownTestSuite() could be protected. Ths way
//  SuiteApiResolver can access them.
struct SuiteApiResolver : T {
  // testing::Test is only forward declared at this point. So we make it a
  // dependend class for the compiler to be OK with it.
  using Test =
	  typename std::conditional<sizeof(T) != 0, ::jmsd::cutf::Test, void>::type;

  static SetUpTearDownSuiteFuncType GetSetUpCaseOrSuite(const char* filename,
														int line_num) {
	SetUpTearDownSuiteFuncType test_case_fp =
#ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI
		GetNotDefaultOrNull(&T::SetUpTestCase, &Test::SetUpTestCase);
#else
		nullptr;
#endif // #ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI

	SetUpTearDownSuiteFuncType test_suite_fp =
		GetNotDefaultOrNull(&T::SetUpTestSuite, &Test::SetUpTestSuite);

	GTEST_CHECK_(!test_case_fp || !test_suite_fp)
		<< "Test can not provide both SetUpTestSuite and SetUpTestCase, please "
		   "make sure there is only one present at "
		<< filename << ":" << line_num;

	return test_case_fp != nullptr ? test_case_fp : test_suite_fp;
  }

  static SetUpTearDownSuiteFuncType GetTearDownCaseOrSuite(const char* filename,
														   int line_num) {
	SetUpTearDownSuiteFuncType test_case_fp =
#ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI
		GetNotDefaultOrNull(&T::TearDownTestCase, &Test::TearDownTestCase);
#else
		nullptr;
#endif // #ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI

	SetUpTearDownSuiteFuncType test_suite_fp =
		GetNotDefaultOrNull(&T::TearDownTestSuite, &Test::TearDownTestSuite);

	GTEST_CHECK_(!test_case_fp || !test_suite_fp)
		<< "Test can not provide both TearDownTestSuite and TearDownTestCase,"
		   " please make sure there is only one present at"
		<< filename << ":" << line_num;

	return test_case_fp != nullptr ? test_case_fp : test_suite_fp;
  }
};

// If *pstr starts with the given prefix, modifies *pstr to be right
// past the prefix and returns true; otherwise leaves *pstr unchanged
// and returns false.  None of pstr, *pstr, and prefix can be NULL.
JMSD_DEPRECATED_GTEST_API_ bool SkipPrefix(const char* prefix, const char** pstr);

#if GTEST_HAS_TYPED_TEST || GTEST_HAS_TYPED_TEST_P

GTEST_DISABLE_MSC_WARNINGS_PUSH_(4251 \
/* class A needs to have dll-interface to be used by clients of class B */)

// State of the definition of a type-parameterized test suite.
class JMSD_DEPRECATED_GTEST_API_ TypedTestSuitePState {
 public:
  TypedTestSuitePState() : registered_(false) {}

  // Adds the given test name to defined_test_names_ and return true
  // if the test suite hasn't been registered; otherwise aborts the
  // program.
  bool AddTestName(const char* file, int line, const char* case_name,
				   const char* test_name) {
	if (registered_) {
	  fprintf(stderr,
			  "%s Test %s must be defined before "
			  "REGISTER_TYPED_TEST_SUITE_P(%s, ...).\n",
			  FormatFileLocation(file, line).c_str(), test_name, case_name);
	  fflush(stderr);
	  posix::Abort();
	}
	registered_tests_.insert(
		::std::make_pair(test_name, CodeLocation(file, line)));
	return true;
  }

  bool TestExists(const std::string& test_name) const {
	return registered_tests_.count(test_name) > 0;
  }

  const CodeLocation& GetCodeLocation(const std::string& test_name) const {
	RegisteredTestsMap::const_iterator it = registered_tests_.find(test_name);
	GTEST_CHECK_(it != registered_tests_.end());
	return it->second;
  }

  // Verifies that registered_tests match the test names in
  // defined_test_names_; returns registered_tests if successful, or
  // aborts the program otherwise.
  const char* VerifyRegisteredTestNames(
	  const char* file, int line, const char* registered_tests);

 private:
  typedef ::std::map<std::string, CodeLocation> RegisteredTestsMap;

  bool registered_;
  RegisteredTestsMap registered_tests_;
};

//  Legacy API is deprecated but still available
#ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_
using TypedTestCasePState = TypedTestSuitePState;
#endif  //  GTEST_REMOVE_LEGACY_TEST_CASEAPI_

GTEST_DISABLE_MSC_WARNINGS_POP_()  //  4251

// Skips to the first non-space char after the first comma in 'str';
// returns NULL if no comma is found in 'str'.
inline const char* SkipComma(const char* str) {
  const char* comma = strchr(str, ',');
  if (comma == nullptr) {
	return nullptr;
  }
  while (IsSpace(*(++comma))) {}
  return comma;
}

// Returns the prefix of 'str' before the first comma in it; returns
// the entire string if it contains no comma.
inline std::string GetPrefixUntilComma(const char* str) {
  const char* comma = strchr(str, ',');
  return comma == nullptr ? str : std::string(str, comma);
}

// Splits a given string on a given delimiter, populating a given
// vector with the fields.
void SplitString(const ::std::string& str, char delimiter,
				 ::std::vector< ::std::string>* dest);

// The default argument to the template below for the case when the user does
// not provide a name generator.
struct DefaultNameGenerator {
  template <typename T>
  static std::string GetName(int i) {
	return ::jmsd::cutf::internal::StreamableToString( i );
  }
};

template <typename Provided = DefaultNameGenerator>
struct NameGeneratorSelector {
  typedef Provided type;
};

template <typename NameGenerator>
void GenerateNamesRecursively(internal::None, std::vector<std::string>*, int) {}

template <typename NameGenerator, typename Types>
void GenerateNamesRecursively(Types, std::vector<std::string>* result, int i) {
  result->push_back(NameGenerator::template GetName<typename Types::Head>(i));
  GenerateNamesRecursively<NameGenerator>(typename Types::Tail(), result,
										  i + 1);
}

template <typename NameGenerator, typename Types>
std::vector<std::string> GenerateNames() {
  std::vector<std::string> result;
  GenerateNamesRecursively<NameGenerator>(Types(), &result, 0);
  return result;
}

// TypeParameterizedTest<Fixture, TestSel, Types>::Register()
// registers a list of type-parameterized tests with Google Test.  The
// return value is insignificant - we just need to return something
// such that we can call this function in a namespace scope.
//
// Implementation note: The GTEST_TEMPLATE_ macro declares a template
// template parameter.  It's defined in gtest-type-util.h.
template <GTEST_TEMPLATE_ Fixture, class TestSel, typename Types>
class TypeParameterizedTest {
 public:
  // 'index' is the index of the test in the type list 'Types'
  // specified in INSTANTIATE_TYPED_TEST_SUITE_P(Prefix, TestSuite,
  // Types).  Valid values for 'index' are [0, N - 1] where N is the
  // length of Types.
  static bool Register(const char* prefix, const CodeLocation& code_location,
					   const char* case_name, const char* test_names, int index,
					   const std::vector<std::string>& type_names =
						   GenerateNames<DefaultNameGenerator, Types>()) {
	typedef typename Types::Head Type;
	typedef Fixture<Type> FixtureClass;
	typedef typename GTEST_BIND_(TestSel, Type) TestClass;

	// First, registers the first type-parameterized test in the type list.
	::jmsd::cutf::internal::MakeAndRegisterTestInfo(
		(std::string(prefix) + (prefix[0] == '\0' ? "" : "/") + case_name +
		 "/" + type_names[static_cast<size_t>(index)])
			.c_str(),
		StripTrailingSpaces(GetPrefixUntilComma(test_names)).c_str(),
		GetTypeName<Type>().c_str(),
		nullptr,  // No value parameter.
		code_location, GetTypeId<FixtureClass>(),
		SuiteApiResolver<TestClass>::GetSetUpCaseOrSuite(
			code_location.file.c_str(), code_location.line),
		SuiteApiResolver<TestClass>::GetTearDownCaseOrSuite(
			code_location.file.c_str(), code_location.line),
		new TestFactoryImpl<TestClass>);

	// Next, recurses (at compile time) with the tail of the type list.
	return TypeParameterizedTest<Fixture, TestSel,
								 typename Types::Tail>::Register(prefix,
																 code_location,
																 case_name,
																 test_names,
																 index + 1,
																 type_names);
  }
};

// The base case for the compile time recursion.
template <GTEST_TEMPLATE_ Fixture, class TestSel>
class TypeParameterizedTest<Fixture, TestSel, internal::None> {
 public:
  static bool Register(const char* /*prefix*/, const CodeLocation&,
					   const char* /*case_name*/, const char* /*test_names*/,
					   int /*index*/,
					   const std::vector<std::string>& =
						   std::vector<std::string>() /*type_names*/) {
	return true;
  }
};

// TypeParameterizedTestSuite<Fixture, Tests, Types>::Register()
// registers *all combinations* of 'Tests' and 'Types' with Google
// Test.  The return value is insignificant - we just need to return
// something such that we can call this function in a namespace scope.
template <GTEST_TEMPLATE_ Fixture, typename Tests, typename Types>
class TypeParameterizedTestSuite {
 public:
  static bool Register(const char* prefix, CodeLocation code_location,
					   const TypedTestSuitePState* state, const char* case_name,
					   const char* test_names,
					   const std::vector<std::string>& type_names =
						   GenerateNames<DefaultNameGenerator, Types>()) {
	std::string test_name = StripTrailingSpaces(
		GetPrefixUntilComma(test_names));
	if (!state->TestExists(test_name)) {
	  fprintf(stderr, "Failed to get code location for test %s.%s at %s.",
			  case_name, test_name.c_str(),
			  FormatFileLocation(code_location.file.c_str(),
								 code_location.line).c_str());
	  fflush(stderr);
	  posix::Abort();
	}
	const CodeLocation& test_location = state->GetCodeLocation(test_name);

	typedef typename Tests::Head Head;

	// First, register the first test in 'Test' for each type in 'Types'.
	TypeParameterizedTest<Fixture, Head, Types>::Register(
		prefix, test_location, case_name, test_names, 0, type_names);

	// Next, recurses (at compile time) with the tail of the test list.
	return TypeParameterizedTestSuite<Fixture, typename Tests::Tail,
									  Types>::Register(prefix, code_location,
													   state, case_name,
													   SkipComma(test_names),
													   type_names);
  }
};

// The base case for the compile time recursion.
template <GTEST_TEMPLATE_ Fixture, typename Types>
class TypeParameterizedTestSuite<Fixture, internal::None, Types> {
 public:
  static bool Register(const char* /*prefix*/, const CodeLocation&,
					   const TypedTestSuitePState* /*state*/,
					   const char* /*case_name*/, const char* /*test_names*/,
					   const std::vector<std::string>& =
						   std::vector<std::string>() /*type_names*/) {
	return true;
  }
};

#endif  // GTEST_HAS_TYPED_TEST || GTEST_HAS_TYPED_TEST_P

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
JMSD_DEPRECATED_GTEST_API_ std::string GetCurrentOsStackTraceExceptTop(
	::jmsd::cutf::UnitTest* unit_test, int skip_count);

// Helpers for suppressing warnings on unreachable code or constant
// condition.

// Always returns true.
JMSD_DEPRECATED_GTEST_API_ bool AlwaysTrue();

// Always returns false.
inline bool AlwaysFalse() { return !AlwaysTrue(); }

// Helper for suppressing false warning from Clang on a const char*
// variable declared in a conditional expression always being NULL in
// the else branch.
struct JMSD_DEPRECATED_GTEST_API_ ConstCharPtr {
  ConstCharPtr(const char* str) : value(str) {}
  operator bool() const { return true; }
  const char* value;
};

// Helper for declaring std::string within 'if' statement
// in pre C++17 build environment.
struct JMSD_DEPRECATED_GTEST_API_ TrueWithString {
  TrueWithString() = default;
  explicit TrueWithString(const char* str) : value(str) {}
  explicit TrueWithString(const std::string& str) : value(str) {}
  explicit operator bool() const { return true; }
  std::string value;
};

// Turns const U&, U&, const U, and U all into U.
#define GTEST_REMOVE_REFERENCE_AND_CONST_(T) \
  typename std::remove_const<typename std::remove_reference<T>::type>::type

// IsAProtocolMessage<T>::value is a compile-time bool constant that's
// true if and only if T is type proto2::Message or a subclass of it.
template <typename T>
struct IsAProtocolMessage
	: public std::is_convertible<const T*, const ::proto2::Message*> {};

// When the compiler sees expression IsContainerTest<C>(0), if C is an
// STL-style container class, the first overload of IsContainerTest
// will be viable (since both C::iterator* and C::const_iterator* are
// valid types and NULL can be implicitly converted to them).  It will
// be picked over the second overload as 'int' is a perfect match for
// the type of argument 0.  If C::iterator or C::const_iterator is not
// a valid type, the first overload is not viable, and the second
// overload will be picked.  Therefore, we can determine whether C is
// a container class by checking the type of IsContainerTest<C>(0).
// The value of the expression is insignificant.
//
// In C++11 mode we check the existence of a const_iterator and that an
// iterator is properly implemented for the container.
//
// For pre-C++11 that we look for both C::iterator and C::const_iterator.
// The reason is that C++ injects the name of a class as a member of the
// class itself (e.g. you can refer to class iterator as either
// 'iterator' or 'iterator::iterator').  If we look for C::iterator
// only, for example, we would mistakenly think that a class named
// iterator is an STL container.
//
// Also note that the simpler approach of overloading
// IsContainerTest(typename C::const_iterator*) and
// IsContainerTest(...) doesn't work with Visual Age C++ and Sun C++.
typedef int IsContainer;
template <class C,
		  class Iterator = decltype(::std::declval<const C&>().begin()),
		  class = decltype(::std::declval<const C&>().end()),
		  class = decltype(++::std::declval<Iterator&>()),
		  class = decltype(*::std::declval<Iterator>()),
		  class = typename C::const_iterator>
IsContainer IsContainerTest(int /* dummy */) {
  return 0;
}

typedef char IsNotContainer;
template <class C>
IsNotContainer IsContainerTest(long /* dummy */) { return '\0'; }

// Trait to detect whether a type T is a hash table.
// The heuristic used is that the type contains an inner type `hasher` and does
// not contain an inner type `reverse_iterator`.
// If the container is iterable in reverse, then order might actually matter.
template <typename T>
struct IsHashTable {
 private:
  template <typename U>
  static char test(typename U::hasher*, typename U::reverse_iterator*);
  template <typename U>
  static int test(typename U::hasher*, ...);
  template <typename U>
  static char test(...);

 public:
  static const bool value = sizeof(test<T>(nullptr, nullptr)) == sizeof(int);
};

template <typename T>
const bool IsHashTable<T>::value;

template <typename C,
		  bool = sizeof(IsContainerTest<C>(0)) == sizeof(IsContainer)>
struct IsRecursiveContainerImpl;

template <typename C>
struct IsRecursiveContainerImpl<C, false> : public std::false_type {};

// Since the IsRecursiveContainerImpl depends on the IsContainerTest we need to
// obey the same inconsistencies as the IsContainerTest, namely check if
// something is a container is relying on only const_iterator in C++11 and
// is relying on both const_iterator and iterator otherwise
template <typename C>
struct IsRecursiveContainerImpl<C, true> {
  using value_type = decltype(*std::declval<typename C::const_iterator>());
  using type =
	  std::is_same<typename std::remove_const<
					   typename std::remove_reference<value_type>::type>::type,
				   C>;
};

// IsRecursiveContainer<Type> is a unary compile-time predicate that
// evaluates whether C is a recursive container type. A recursive container
// type is a container type whose value_type is equal to the container type
// itself. An example for a recursive container type is
// boost::filesystem::path, whose iterator has a value_type that is equal to
// boost::filesystem::path.
template <typename C>
struct IsRecursiveContainer : public IsRecursiveContainerImpl<C>::type {};

// Utilities for native arrays.

// ArrayEq() compares two k-dimensional native arrays using the
// elements' operator==, where k can be any integer >= 0.  When k is
// 0, ArrayEq() degenerates into comparing a single pair of values.

template <typename T, typename U>
bool ArrayEq(const T* lhs, size_t size, const U* rhs);

// This generic version is used when k is 0.
template <typename T, typename U>
inline bool ArrayEq(const T& lhs, const U& rhs) { return lhs == rhs; }

// This overload is used when k >= 1.
template <typename T, typename U, size_t N>
inline bool ArrayEq(const T(&lhs)[N], const U(&rhs)[N]) {
  return internal::ArrayEq(lhs, N, rhs);
}

// This helper reduces code bloat.  If we instead put its logic inside
// the previous ArrayEq() function, arrays with different sizes would
// lead to different copies of the template code.
template <typename T, typename U>
bool ArrayEq(const T* lhs, size_t size, const U* rhs) {
  for (size_t i = 0; i != size; i++) {
	if (!internal::ArrayEq(lhs[i], rhs[i]))
	  return false;
  }
  return true;
}

// Finds the first element in the iterator range [begin, end) that
// equals elem.  Element may be a native array type itself.
template <typename Iter, typename Element>
Iter ArrayAwareFind(Iter begin, Iter end, const Element& elem) {
  for (Iter it = begin; it != end; ++it) {
	if (internal::ArrayEq(*it, elem))
	  return it;
  }
  return end;
}

// CopyArray() copies a k-dimensional native array using the elements'
// operator=, where k can be any integer >= 0.  When k is 0,
// CopyArray() degenerates into copying a single value.

template <typename T, typename U>
void CopyArray(const T* from, size_t size, U* to);

// This generic version is used when k is 0.
template <typename T, typename U>
inline void CopyArray(const T& from, U* to) { *to = from; }

// This overload is used when k >= 1.
template <typename T, typename U, size_t N>
inline void CopyArray(const T(&from)[N], U(*to)[N]) {
  internal::CopyArray(from, N, *to);
}

// This helper reduces code bloat.  If we instead put its logic inside
// the previous CopyArray() function, arrays with different sizes
// would lead to different copies of the template code.
template <typename T, typename U>
void CopyArray(const T* from, size_t size, U* to) {
  for (size_t i = 0; i != size; i++) {
	internal::CopyArray(from[i], to + i);
  }
}

// The relation between an NativeArray object (see below) and the
// native array it represents.
// We use 2 different structs to allow non-copyable types to be used, as long
// as RelationToSourceReference() is passed.
struct RelationToSourceReference {};
struct RelationToSourceCopy {};

// Adapts a native array to a read-only STL-style container.  Instead
// of the complete STL container concept, this adaptor only implements
// members useful for Google Mock's container matchers.  New members
// should be added as needed.  To simplify the implementation, we only
// support Element being a raw type (i.e. having no top-level const or
// reference modifier).  It's the client's responsibility to satisfy
// this requirement.  Element can be an array type itself (hence
// multi-dimensional arrays are supported).
template <typename Element>
class NativeArray {
 public:
  // STL-style container typedefs.
  typedef Element value_type;
  typedef Element* iterator;
  typedef const Element* const_iterator;

  // Constructs from a native array. References the source.
  NativeArray(const Element* array, size_t count, RelationToSourceReference) {
	InitRef(array, count);
  }

  // Constructs from a native array. Copies the source.
  NativeArray(const Element* array, size_t count, RelationToSourceCopy) {
	InitCopy(array, count);
  }

  // Copy constructor.
  NativeArray(const NativeArray& rhs) {
	(this->*rhs.clone_)(rhs.array_, rhs.size_);
  }

  ~NativeArray() {
	if (clone_ != &NativeArray::InitRef)
	  delete[] array_;
  }

  // STL-style container methods.
  size_t size() const { return size_; }
  const_iterator begin() const { return array_; }
  const_iterator end() const { return array_ + size_; }
  bool operator==(const NativeArray& rhs) const {
	return size() == rhs.size() &&
		ArrayEq(begin(), size(), rhs.begin());
  }

 private:
  static_assert(!std::is_const<Element>::value, "Type must not be const");
  static_assert(!std::is_reference<Element>::value,
				"Type must not be a reference");

  // Initializes this object with a copy of the input.
  void InitCopy(const Element* array, size_t a_size) {
	Element* const copy = new Element[a_size];
	CopyArray(array, a_size, copy);
	array_ = copy;
	size_ = a_size;
	clone_ = &NativeArray::InitCopy;
  }

  // Initializes this object with a reference of the input.
  void InitRef(const Element* array, size_t a_size) {
	array_ = array;
	size_ = a_size;
	clone_ = &NativeArray::InitRef;
  }

  const Element* array_;
  size_t size_;
  void (NativeArray::*clone_)(const Element*, size_t);

  GTEST_DISALLOW_ASSIGN_(NativeArray);
};

// Backport of std::index_sequence.
template <size_t... Is>
struct IndexSequence {
  using type = IndexSequence;
};

// Double the IndexSequence, and one if plus_one is true.
template <bool plus_one, typename T, size_t sizeofT>
struct DoubleSequence;
template <size_t... I, size_t sizeofT>
struct DoubleSequence<true, IndexSequence<I...>, sizeofT> {
  using type = IndexSequence<I..., (sizeofT + I)..., 2 * sizeofT>;
};
template <size_t... I, size_t sizeofT>
struct DoubleSequence<false, IndexSequence<I...>, sizeofT> {
  using type = IndexSequence<I..., (sizeofT + I)...>;
};

// Backport of std::make_index_sequence.
// It uses O(ln(N)) instantiation depth.
template <size_t N>
struct MakeIndexSequence
	: DoubleSequence<N % 2 == 1, typename MakeIndexSequence<N / 2>::type,
					 N / 2>::type {};

template <>
struct MakeIndexSequence<0> : IndexSequence<> {};

template <size_t>
struct Ignore {
  Ignore(...);  // NOLINT
};

template <typename>
struct ElemFromListImpl;
template <size_t... I>
struct ElemFromListImpl<IndexSequence<I...>> {
  // We make Ignore a template to solve a problem with MSVC.
  // A non-template Ignore would work fine with `decltype(Ignore(I))...`, but
  // MSVC doesn't understand how to deal with that pack expansion.
  // Use `0 * I` to have a single instantiation of Ignore.
  template <typename R>
  static R Apply(Ignore<0 * I>..., R (*)(), ...);
};

template <size_t N, typename... T>
struct ElemFromList {
  using type =
	  decltype(ElemFromListImpl<typename MakeIndexSequence<N>::type>::Apply(
		  static_cast<T (*)()>(nullptr)...));
};

template <typename... T>
class FlatTuple;

template <typename Derived, size_t I>
struct FlatTupleElemBase;

template <typename... T, size_t I>
struct FlatTupleElemBase<FlatTuple<T...>, I> {
  using value_type = typename ElemFromList<I, T...>::type;
  FlatTupleElemBase() = default;
  explicit FlatTupleElemBase(value_type t) : value(std::move(t)) {}
  value_type value;
};

template <typename Derived, typename Idx>
struct FlatTupleBase;

template <size_t... Idx, typename... T>
struct FlatTupleBase<FlatTuple<T...>, IndexSequence<Idx...>>
	: FlatTupleElemBase<FlatTuple<T...>, Idx>... {
  using Indices = IndexSequence<Idx...>;
  FlatTupleBase() = default;
  explicit FlatTupleBase(T... t)
	  : FlatTupleElemBase<FlatTuple<T...>, Idx>(std::move(t))... {}
};

// Analog to std::tuple but with different tradeoffs.
// This class minimizes the template instantiation depth, thus allowing more
// elements than std::tuple would. std::tuple has been seen to require an
// instantiation depth of more than 10x the number of elements in some
// implementations.
// FlatTuple and ElemFromList are not recursive and have a fixed depth
// regardless of T...
// MakeIndexSequence, on the other hand, it is recursive but with an
// instantiation depth of O(ln(N)).
template <typename... T>
class FlatTuple
	: private FlatTupleBase<FlatTuple<T...>,
							typename MakeIndexSequence<sizeof...(T)>::type> {
  using Indices = typename FlatTupleBase<
	  FlatTuple<T...>, typename MakeIndexSequence<sizeof...(T)>::type>::Indices;

 public:
  FlatTuple() = default;
  explicit FlatTuple(T... t) : FlatTuple::FlatTupleBase(std::move(t)...) {}

  template <size_t I>
  const typename ElemFromList<I, T...>::type& Get() const {
	return static_cast<const FlatTupleElemBase<FlatTuple, I>*>(this)->value;
  }

  template <size_t I>
  typename ElemFromList<I, T...>::type& Get() {
	return static_cast<FlatTupleElemBase<FlatTuple, I>*>(this)->value;
  }
};

// Utility functions to be called with static_assert to induce deprecation
// warnings.
GTEST_INTERNAL_DEPRECATED(
	"INSTANTIATE_TEST_CASE_P is deprecated, please use "
	"INSTANTIATE_TEST_SUITE_P")
constexpr bool InstantiateTestCase_P_IsDeprecated() { return true; }

GTEST_INTERNAL_DEPRECATED(
	"TYPED_TEST_CASE_P is deprecated, please use "
	"TYPED_TEST_SUITE_P")
constexpr bool TypedTestCase_P_IsDeprecated() { return true; }

GTEST_INTERNAL_DEPRECATED(
	"TYPED_TEST_CASE is deprecated, please use "
	"TYPED_TEST_SUITE")
constexpr bool TypedTestCaseIsDeprecated() { return true; }

GTEST_INTERNAL_DEPRECATED(
	"REGISTER_TYPED_TEST_CASE_P is deprecated, please use "
	"REGISTER_TYPED_TEST_SUITE_P")
constexpr bool RegisterTypedTestCase_P_IsDeprecated() { return true; }

GTEST_INTERNAL_DEPRECATED(
	"INSTANTIATE_TYPED_TEST_CASE_P is deprecated, please use "
	"INSTANTIATE_TYPED_TEST_SUITE_P")
constexpr bool InstantiateTypedTestCase_P_IsDeprecated() { return true; }

}  // namespace internal
}  // namespace testing

#define GTEST_MESSAGE_AT_(file, line, message, result_type) \
  ::testing::internal::AssertHelper(result_type, file, line, message) \
	= ::jmsd::cutf::Message()

#define GTEST_MESSAGE_(message, result_type) \
  GTEST_MESSAGE_AT_(__FILE__, __LINE__, message, result_type)

#define GTEST_FATAL_FAILURE_(message) \
  return GTEST_MESSAGE_(message, ::testing::TestPartResult::kFatalFailure)

#define GTEST_NONFATAL_FAILURE_(message) \
  GTEST_MESSAGE_(message, ::testing::TestPartResult::kNonFatalFailure)

#define GTEST_SUCCESS_(message) \
  GTEST_MESSAGE_(message, ::testing::TestPartResult::kSuccess)

#define GTEST_SKIP_(message) \
  return GTEST_MESSAGE_(message, ::testing::TestPartResult::kSkip)

// Suppress MSVC warning 4072 (unreachable code) for the code following
// statement if it returns or throws (or doesn't return or throw in some
// situations).
#define GTEST_SUPPRESS_UNREACHABLE_CODE_WARNING_BELOW_(statement) \
  if (::testing::internal::AlwaysTrue()) { statement; }

#define GTEST_TEST_THROW_(statement, expected_exception, fail) \
  GTEST_AMBIGUOUS_ELSE_BLOCKER_ \
  if (::testing::internal::ConstCharPtr gtest_msg = "") { \
	bool gtest_caught_expected = false; \
	try { \
	  GTEST_SUPPRESS_UNREACHABLE_CODE_WARNING_BELOW_(statement); \
	} \
	catch (expected_exception const&) { \
	  gtest_caught_expected = true; \
	} \
	catch (...) { \
	  gtest_msg.value = \
		  "Expected: " #statement " throws an exception of type " \
		  #expected_exception ".\n  Actual: it throws a different type."; \
	  goto GTEST_CONCAT_TOKEN_(gtest_label_testthrow_, __LINE__); \
	} \
	if (!gtest_caught_expected) { \
	  gtest_msg.value = \
		  "Expected: " #statement " throws an exception of type " \
		  #expected_exception ".\n  Actual: it throws nothing."; \
	  goto GTEST_CONCAT_TOKEN_(gtest_label_testthrow_, __LINE__); \
	} \
  } else \
	GTEST_CONCAT_TOKEN_(gtest_label_testthrow_, __LINE__): \
	  fail(gtest_msg.value)

#if GTEST_HAS_EXCEPTIONS

#define GTEST_TEST_NO_THROW_CATCH_STD_EXCEPTION_() \
  catch (std::exception const& e) { \
	gtest_msg.value = ( \
	  "it throws std::exception-derived exception with description: \"" \
	); \
	gtest_msg.value += e.what(); \
	gtest_msg.value += "\"."; \
	goto GTEST_CONCAT_TOKEN_(gtest_label_testnothrow_, __LINE__); \
  }

#else  // GTEST_HAS_EXCEPTIONS

#define GTEST_TEST_NO_THROW_CATCH_STD_EXCEPTION_()

#endif  // GTEST_HAS_EXCEPTIONS

#define GTEST_TEST_NO_THROW_(statement, fail) \
  GTEST_AMBIGUOUS_ELSE_BLOCKER_ \
  if (::testing::internal::TrueWithString gtest_msg{}) { \
	try { \
	  GTEST_SUPPRESS_UNREACHABLE_CODE_WARNING_BELOW_(statement); \
	} \
	GTEST_TEST_NO_THROW_CATCH_STD_EXCEPTION_() \
	catch (...) { \
	  gtest_msg.value = "it throws."; \
	  goto GTEST_CONCAT_TOKEN_(gtest_label_testnothrow_, __LINE__); \
	} \
  } else \
	GTEST_CONCAT_TOKEN_(gtest_label_testnothrow_, __LINE__): \
	  fail(("Expected: " #statement " doesn't throw an exception.\n" \
			"  Actual: " + gtest_msg.value).c_str())

#define GTEST_TEST_ANY_THROW_(statement, fail) \
  GTEST_AMBIGUOUS_ELSE_BLOCKER_ \
  if (::testing::internal::AlwaysTrue()) { \
	bool gtest_caught_any = false; \
	try { \
	  GTEST_SUPPRESS_UNREACHABLE_CODE_WARNING_BELOW_(statement); \
	} \
	catch (...) { \
	  gtest_caught_any = true; \
	} \
	if (!gtest_caught_any) { \
	  goto GTEST_CONCAT_TOKEN_(gtest_label_testanythrow_, __LINE__); \
	} \
  } else \
	GTEST_CONCAT_TOKEN_(gtest_label_testanythrow_, __LINE__): \
	  fail("Expected: " #statement " throws an exception.\n" \
		   "  Actual: it doesn't.")


// Implements Boolean test assertions such as EXPECT_TRUE. expression can be
// either a boolean expression or an AssertionResult. text is a textual
// represenation of expression as it was passed into the EXPECT_TRUE.
#define GTEST_TEST_BOOLEAN_(expression, text, actual, expected, fail) \
  GTEST_AMBIGUOUS_ELSE_BLOCKER_ \
  if (const ::jmsd::cutf::AssertionResult gtest_ar_ = \
	  ::jmsd::cutf::AssertionResult(expression)) \
	; \
  else \
	fail(::testing::internal::GetBoolAssertionFailureMessage(\
		gtest_ar_, text, #actual, #expected).c_str())

#define GTEST_TEST_NO_FATAL_FAILURE_(statement, fail) \
  GTEST_AMBIGUOUS_ELSE_BLOCKER_ \
  if (::testing::internal::AlwaysTrue()) { \
	::testing::internal::HasNewFatalFailureHelper gtest_fatal_failure_checker; \
	GTEST_SUPPRESS_UNREACHABLE_CODE_WARNING_BELOW_(statement); \
	if (gtest_fatal_failure_checker.has_new_fatal_failure()) { \
	  goto GTEST_CONCAT_TOKEN_(gtest_label_testnofatal_, __LINE__); \
	} \
  } else \
	GTEST_CONCAT_TOKEN_(gtest_label_testnofatal_, __LINE__): \
	  fail("Expected: " #statement " doesn't generate new fatal " \
		   "failures in the current thread.\n" \
		   "  Actual: it does.")

// Expands to the name of the class that implements the given test.
#define GTEST_TEST_CLASS_NAME_(test_suite_name, test_name) \
  test_suite_name##_##test_name##_Test

// Helper macro for defining tests.
#define GTEST_TEST_(test_suite_name, test_name, parent_class, parent_id)      \
  static_assert(sizeof(GTEST_STRINGIFY_(test_suite_name)) > 1,                \
				"test_suite_name must not be empty");                         \
  static_assert(sizeof(GTEST_STRINGIFY_(test_name)) > 1,                      \
				"test_name must not be empty");                               \
  class GTEST_TEST_CLASS_NAME_(test_suite_name, test_name)                    \
	  : public parent_class {                                                 \
   public:                                                                    \
	GTEST_TEST_CLASS_NAME_(test_suite_name, test_name)() {}                   \
																			  \
   private:                                                                   \
	void TestBody() override;                                                 \
	static ::jmsd::cutf::TestInfo* const test_info_ GTEST_ATTRIBUTE_UNUSED_;     \
	GTEST_DISALLOW_COPY_AND_ASSIGN_(GTEST_TEST_CLASS_NAME_(test_suite_name,   \
														   test_name));       \
  };                                                                          \
																			  \
  ::jmsd::cutf::TestInfo* const GTEST_TEST_CLASS_NAME_(test_suite_name,          \
													test_name)::test_info_ =  \
	  ::jmsd::cutf::internal::MakeAndRegisterTestInfo(                           \
		  #test_suite_name, #test_name, nullptr, nullptr,                     \
		  ::testing::internal::CodeLocation(__FILE__, __LINE__), (parent_id), \
		  ::testing::internal::SuiteApiResolver<                              \
			  parent_class>::GetSetUpCaseOrSuite(__FILE__, __LINE__),         \
		  ::testing::internal::SuiteApiResolver<                              \
			  parent_class>::GetTearDownCaseOrSuite(__FILE__, __LINE__),      \
		  new ::testing::internal::TestFactoryImpl<GTEST_TEST_CLASS_NAME_(    \
			  test_suite_name, test_name)>);                                  \
  void GTEST_TEST_CLASS_NAME_(test_suite_name, test_name)::TestBody()
