#include "Test_suite.h"


namespace jmsd {
namespace cutf {


// class TestSuite

// Gets the number of successful tests in this test suite.
int TestSuite::successful_test_count() const {
  return CountIf(test_info_list_, TestPassed);
}

// Gets the number of successful tests in this test suite.
int TestSuite::skipped_test_count() const {
  return CountIf(test_info_list_, TestSkipped);
}

// Gets the number of failed tests in this test suite.
int TestSuite::failed_test_count() const {
  return CountIf(test_info_list_, TestFailed);
}

// Gets the number of disabled tests that will be reported in the XML report.
int TestSuite::reportable_disabled_test_count() const {
  return CountIf(test_info_list_, TestReportableDisabled);
}

// Gets the number of disabled tests in this test suite.
int TestSuite::disabled_test_count() const {
  return CountIf(test_info_list_, TestDisabled);
}

// Gets the number of tests to be printed in the XML report.
int TestSuite::reportable_test_count() const {
  return CountIf(test_info_list_, TestReportable);
}

// Get the number of tests in this test suite that should run.
int TestSuite::test_to_run_count() const {
  return CountIf(test_info_list_, ShouldRunTest);
}

// Gets the number of all tests.
int TestSuite::total_test_count() const {
  return static_cast<int>(test_info_list_.size());
}

// Creates a TestSuite with the given name.
//
// Arguments:
//
//   name:         name of the test suite
//   a_type_param: the name of the test suite's type parameter, or NULL if
//                 this is not a typed or a type-parameterized test suite.
//   set_up_tc:    pointer to the function that sets up the test suite
//   tear_down_tc: pointer to the function that tears down the test suite
TestSuite::TestSuite(const char* a_name, const char* a_type_param,
					 internal::SetUpTestSuiteFunc set_up_tc,
					 internal::TearDownTestSuiteFunc tear_down_tc)
	: name_(a_name),
	  type_param_(a_type_param ? new std::string(a_type_param) : nullptr),
	  set_up_tc_(set_up_tc),
	  tear_down_tc_(tear_down_tc),
	  should_run_(false),
	  start_timestamp_(0),
	  elapsed_time_(0) {}

// Destructor of TestSuite.
TestSuite::~TestSuite() {
  // Deletes every Test in the collection.
  ForEach(test_info_list_, internal::Delete<TestInfo>);
}

// Returns the i-th test among all the tests. i can range from 0 to
// total_test_count() - 1. If i is not in that range, returns NULL.
const TestInfo* TestSuite::GetTestInfo(int i) const {
  const int index = GetElementOr(test_indices_, i, -1);
  return index < 0 ? nullptr : test_info_list_[static_cast<size_t>(index)];
}

// Returns the i-th test among all the tests. i can range from 0 to
// total_test_count() - 1. If i is not in that range, returns NULL.
TestInfo* TestSuite::GetMutableTestInfo(int i) {
  const int index = GetElementOr(test_indices_, i, -1);
  return index < 0 ? nullptr : test_info_list_[static_cast<size_t>(index)];
}

// Adds a test to this test suite.  Will delete the test upon
// destruction of the TestSuite object.
void TestSuite::AddTestInfo(TestInfo* test_info) {
  test_info_list_.push_back(test_info);
  test_indices_.push_back(static_cast<int>(test_indices_.size()));
}

// Runs every test in this TestSuite.
void TestSuite::Run() {
  if (!should_run_) return;

  internal::UnitTestImpl* const impl = internal::GetUnitTestImpl();
  impl->set_current_test_suite(this);

  TestEventListener* repeater = UnitTest::GetInstance()->listeners().repeater();

  // Call both legacy and the new API
  repeater->OnTestSuiteStart(*this);
//  Legacy API is deprecated but still available
#ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI
  repeater->OnTestCaseStart(*this);
#endif  //  GTEST_REMOVE_LEGACY_TEST_CASEAPI

  impl->os_stack_trace_getter()->UponLeavingGTest();
  internal::HandleExceptionsInMethodIfSupported(
	  this, &TestSuite::RunSetUpTestSuite, "SetUpTestSuite()");

  start_timestamp_ = internal::GetTimeInMillis();
  for (int i = 0; i < total_test_count(); i++) {
	GetMutableTestInfo(i)->Run();
  }
  elapsed_time_ = internal::GetTimeInMillis() - start_timestamp_;

  impl->os_stack_trace_getter()->UponLeavingGTest();
  internal::HandleExceptionsInMethodIfSupported(
	  this, &TestSuite::RunTearDownTestSuite, "TearDownTestSuite()");

  // Call both legacy and the new API
  repeater->OnTestSuiteEnd(*this);
//  Legacy API is deprecated but still available
#ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI
  repeater->OnTestCaseEnd(*this);
#endif  //  GTEST_REMOVE_LEGACY_TEST_CASEAPI

  impl->set_current_test_suite(nullptr);
}

// Clears the results of all tests in this test suite.
void TestSuite::ClearResult() {
  ad_hoc_test_result_.Clear();
  ForEach(test_info_list_, TestInfo::ClearTestResult);
}

// Shuffles the tests in this test suite.
void TestSuite::ShuffleTests(internal::Random* random) {
  Shuffle(random, &test_indices_);
}

// Restores the test order to before the first shuffle.
void TestSuite::UnshuffleTests() {
  for (size_t i = 0; i < test_indices_.size(); i++) {
	test_indices_[i] = static_cast<int>(i);
  }
}


} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
