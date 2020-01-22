#include "Test_info.h"


//#include "internal/gtest-internal.h"

#include "Test_event_listener.h"
#include "Test_event_listeners.h"
#include "internal/Unit_test_impl.h"
#include "internal/Exception_handling.hin"


namespace jmsd {
namespace cutf {


// class TestInfo

// Constructs a TestInfo object. It assumes ownership of the test factory
// object.
TestInfo::TestInfo(
	const std::string& a_test_suite_name,
	const std::string& a_name, const char* a_type_param,
	const char* a_value_param,
	::testing::internal::CodeLocation a_code_location,
	::testing::internal::TypeId fixture_class_id,
	::testing::internal::TestFactoryBase* factory )
	:
		test_suite_name_(a_test_suite_name),
		name_(a_name),
		type_param_(a_type_param ? new std::string(a_type_param) : nullptr),
		value_param_(a_value_param ? new std::string(a_value_param) : nullptr),
		location_(a_code_location),
		fixture_class_id_(fixture_class_id),
		should_run_(false),
		is_disabled_(false),
		matches_filter_(false),
		factory_(factory),
		result_( new TestResult )
{}

// Destructs a TestInfo object.
TestInfo::~TestInfo() {
	delete factory_;
}

// Returns the test suite name.
const char* TestInfo::test_suite_name() const {
	return test_suite_name_.c_str();
}

// Legacy API is deprecated but still available
#ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_
const char* TestInfo::test_case_name() const {
	return test_suite_name();
}
#endif  // GTEST_REMOVE_LEGACY_TEST_CASEAPI_

// Returns the test name.
const char* TestInfo::name() const {
	return name_.c_str();
}

// Returns the name of the parameter type, or NULL if this is not a typed
// or a type-parameterized test.
const char* TestInfo::type_param() const {
	if (type_param_.get() != nullptr) return type_param_->c_str();

	return nullptr;
}

// Returns the text representation of the value parameter, or NULL if this
// is not a value-parameterized test.
const char* TestInfo::value_param() const {
	if (value_param_.get() != nullptr) return value_param_->c_str();

	return nullptr;
}

// Returns the file name where this test is defined.
const char* TestInfo::file() const {
	return location_.file.c_str();
}

// Returns the line where this test is defined.
int TestInfo::line() const {
	return location_.line;
}

// Return true if this test should not be run because it's in another shard.
bool TestInfo::is_in_another_shard() const {
	return is_in_another_shard_;
}

// Returns true if this test should run, that is if the test is not
// disabled (or it is disabled but the also_run_disabled_tests flag has
// been specified) and its full name matches the user-specified filter.
//
// Google Test allows the user to filter the tests by their full names.
// The full name of a test Bar in test suite Foo is defined as
// "Foo.Bar".  Only the tests that match the filter will run.
//
// A filter is a colon-separated list of glob (not regex) patterns,
// optionally followed by a '-' and a colon-separated list of
// negative patterns (tests to exclude).  A test is run if it
// matches one of the positive patterns and does not match any of
// the negative patterns.
//
// For example, *A*:Foo.* is a filter that matches any string that
// contains the character 'A' or starts with "Foo.".
bool TestInfo::should_run() const {
	return should_run_;
}

// Returns true if and only if this test will appear in the XML report.
bool TestInfo::is_reportable() const {
	// The XML report includes tests matching the filter, excluding those
	// run in other shards.
	return matches_filter_ && !is_in_another_shard_;
}

// Returns the result of the test.
const TestResult* TestInfo::result() const {
	return result_.get();
}

namespace {

// A predicate that checks the test name of a TestInfo against a known
// value.
//
// This is used for implementation of the TestSuite class only.  We put
// it in the anonymous namespace to prevent polluting the outer
// namespace.
//
// TestNameIs is copyable.
class TestNameIs {
 public:
  // Constructor.
  //
  // TestNameIs has NO default constructor.
  explicit TestNameIs(const char* name)
	  : name_(name) {}

  // Returns true if and only if the test name of test_info matches name_.
  bool operator()(const TestInfo * test_info) const {
	return test_info && test_info->name() == name_;
  }

 private:
  std::string name_;
};

}  // namespace

namespace internal {

// This method expands all parameterized tests registered with macros TEST_P
// and INSTANTIATE_TEST_SUITE_P into regular tests and registers those.
// This will be done just once during the program runtime.
void UnitTestImpl::RegisterParameterizedTests() {
  if (!parameterized_tests_registered_) {
	parameterized_test_registry_.RegisterTests();
	parameterized_tests_registered_ = true;
  }
}

}  // namespace internal

// Increments the number of death tests encountered in this test so far.
int TestInfo::increment_death_test_count() {
	return result_->increment_death_test_count();
}

// Creates the test object, runs it, records its result, and then
// deletes it.
void TestInfo::Run() {
  if (!should_run_) return;

  // Tells UnitTest where to store test result.
  internal::UnitTestImpl* const impl = internal::GetUnitTestImpl();
  impl->set_current_test_info(this);

  TestEventListener* repeater = UnitTest::GetInstance()->listeners().repeater();

  // Notifies the unit test event listeners that a test is about to start.
  repeater->OnTestStart(*this);

  ::testing::internal::TimeInMillis const start = ::testing::internal::GetTimeInMillis();

  impl->os_stack_trace_getter()->UponLeavingGTest();

  // Creates the test object.
  Test* const test = internal::HandleExceptionsInMethodIfSupported( factory_, &::testing::internal::TestFactoryBase::CreateTest, "the test fixture's constructor");

  // Runs the test if the constructor didn't generate a fatal failure or invoke
  // GTEST_SKIP().
  // Note that the object will not be null
  if ( !Test::HasFatalFailure() && !Test::IsSkipped()) {
	// This doesn't throw as all user code that can throw are wrapped into
	// exception handling code.
	test->Run();
  }

  if (test != nullptr) {
	// Deletes the test object.
	impl->os_stack_trace_getter()->UponLeavingGTest();
	internal::HandleExceptionsInMethodIfSupported(
		test, &Test::DeleteSelf_, "the test fixture's destructor");
  }

  result_->set_start_timestamp(start);
  result_->set_elapsed_time(::testing::internal::GetTimeInMillis() - start);

  // Notifies the unit test event listener that a test has just finished.
  repeater->OnTestEnd(*this);

  // Tells UnitTest to stop associating assertion results to this
  // test.
  impl->set_current_test_info(nullptr);
}

// static
void TestInfo::ClearTestResult(TestInfo* test_info) {
	test_info->result_->Clear();
}


} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
