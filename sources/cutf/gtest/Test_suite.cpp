#include "Test_suite.h"


#include "Test_info.h"

#include "Test_event_listener.h"
#include "internal/Unit_test_impl.h"
#include "internal/Exception_handling.hin"
#include "internal/function_Stl_utilities.hin"
#include "internal/function_Delete.hin"


namespace jmsd {
namespace cutf {


// Gets the number of successful tests in this test suite.
int TestSuite::successful_test_count() const {
  return internal::function_Stl_utilities::CountIf(test_info_list_, TestPassed);
}

// Gets the number of successful tests in this test suite.
int TestSuite::skipped_test_count() const {
  return internal::function_Stl_utilities::CountIf(test_info_list_, TestSkipped);
}

// Gets the number of failed tests in this test suite.
int TestSuite::failed_test_count() const {
  return internal::function_Stl_utilities::CountIf(test_info_list_, TestFailed);
}

// Gets the number of disabled tests that will be reported in the XML report.
int TestSuite::reportable_disabled_test_count() const {
  return internal::function_Stl_utilities::CountIf(test_info_list_, TestReportableDisabled);
}

// Gets the number of disabled tests in this test suite.
int TestSuite::disabled_test_count() const {
  return internal::function_Stl_utilities::CountIf(test_info_list_, TestDisabled);
}

// Gets the number of tests to be printed in the XML report.
int TestSuite::reportable_test_count() const {
  return internal::function_Stl_utilities::CountIf(test_info_list_, TestReportable);
}

// Get the number of tests in this test suite that should run.
int TestSuite::test_to_run_count() const {
  return internal::function_Stl_utilities::CountIf(test_info_list_, ShouldRunTest);
}

// Gets the number of all tests.
int TestSuite::total_test_count() const {
  return static_cast<int>(test_info_list_.size());
}

// Returns true if and only if the test suite passed.
bool TestSuite::Passed() const {
	return !Failed();
}

// Returns true if and only if the test suite failed.
bool TestSuite::Failed() const {
	return failed_test_count() > 0 || ad_hoc_test_result().Failed();
}

// Returns the elapsed time, in milliseconds.
::testing::internal::TimeInMillis TestSuite::elapsed_time() const {
	return elapsed_time_;
}

// Gets the time of the test suite start, in ms from the start of the
// UNIX epoch.
::testing::internal::TimeInMillis TestSuite::start_timestamp() const {
	return start_timestamp_;
}

// Returns the TestResult that holds test properties recorded during
// execution of SetUpTestSuite and TearDownTestSuite.
const TestResult& TestSuite::ad_hoc_test_result() const {
	return *ad_hoc_test_result_;
}

// Gets the (mutable) vector of TestInfos in this TestSuite.
std::vector< TestInfo * > &TestSuite::test_info_list() {
	return test_info_list_;
}

// Gets the (immutable) vector of TestInfos in this TestSuite.
const std::vector< TestInfo * >& TestSuite::test_info_list() const {
	return test_info_list_;
}

// Sets the should_run member.
void TestSuite::set_should_run(bool const should) {
	should_run_ = should;
}

// static
void TestSuite::ClearTestSuiteResult(TestSuite* test_suite) {
	test_suite->ClearResult();
}

// Runs SetUpTestSuite() for this TestSuite.  This wrapper is needed
// for catching exceptions thrown from SetUpTestSuite().
void TestSuite::RunSetUpTestSuite() {
	if (set_up_tc_ != nullptr) {
		(*set_up_tc_)();
	}
}

// Runs TearDownTestSuite() for this TestSuite.  This wrapper is
// needed for catching exceptions thrown from TearDownTestSuite().
void TestSuite::RunTearDownTestSuite() {
	if (tear_down_tc_ != nullptr) {
		(*tear_down_tc_)();
	}
}

// Returns true if and only if test passed.
// static
bool TestSuite::TestPassed(const TestInfo* test_info) {
	return test_info->should_run() && test_info->result()->Passed();
}

// Returns true if and only if test skipped.
// static
bool TestSuite::TestSkipped(const TestInfo* test_info) {
	return test_info->should_run() && test_info->result()->Skipped();
}

// Returns true if and only if test failed.
// static
bool TestSuite::TestFailed(const TestInfo* test_info) {
	return test_info->should_run() && test_info->result()->Failed();
}

// Returns true if and only if the test is disabled and will be reported in
// the XML report.
// static
bool TestSuite::TestReportableDisabled(const TestInfo* test_info) {
	return test_info->is_reportable() && test_info->is_disabled_;
}

// Returns true if and only if test is disabled.
// static
bool TestSuite::TestDisabled(const TestInfo* test_info) {
	return test_info->is_disabled_;
}

// Returns true if and only if this test will appear in the XML report.
// static
bool TestSuite::TestReportable(const TestInfo* test_info) {
	return test_info->is_reportable();
}

// Returns true if the given test should run.
// static
bool TestSuite::ShouldRunTest(const TestInfo* test_info) {
	return test_info->should_run();
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
					 ::testing::internal::SetUpTestSuiteFunc set_up_tc,
					 ::testing::internal::TearDownTestSuiteFunc tear_down_tc)
	: name_(a_name),
	  type_param_(a_type_param ? new std::string(a_type_param) : nullptr),
	  set_up_tc_(set_up_tc),
	  tear_down_tc_(tear_down_tc),
	  should_run_(false),
	  start_timestamp_(0),
	  elapsed_time_(0),
	  ad_hoc_test_result_( new TestResult )
{}

// Destructor of TestSuite.
TestSuite::~TestSuite() {
  // Deletes every Test in the collection.
  internal::function_Stl_utilities::ForEach(test_info_list_, internal::function_Delete::Delete<TestInfo>);
}

// Gets the name of the TestSuite.
const char* TestSuite::name() const {
	return name_.c_str();
}

// Returns the name of the parameter type, or NULL if this is not a
// type-parameterized test suite.
const char* TestSuite::type_param() const {
	if (type_param_.get() != nullptr) return type_param_->c_str();

	return nullptr;
}

// Returns true if any test in this test suite should run.
bool TestSuite::should_run() const {
	return should_run_;
}

// Returns the i-th test among all the tests. i can range from 0 to
// total_test_count() - 1. If i is not in that range, returns NULL.
const TestInfo* TestSuite::GetTestInfo(int i) const {
  const int index = internal::function_Stl_utilities::GetElementOr(test_indices_, i, -1);
  return index < 0 ? nullptr : test_info_list_[static_cast<size_t>(index)];
}

// Returns the i-th test among all the tests. i can range from 0 to
// total_test_count() - 1. If i is not in that range, returns NULL.
TestInfo* TestSuite::GetMutableTestInfo(int i) {
  const int index = internal::function_Stl_utilities::GetElementOr(test_indices_, i, -1);
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

  internal::HandleExceptionsInMethodIfSupported( this, &TestSuite::RunSetUpTestSuite, "SetUpTestSuite()" );

  start_timestamp_ = ::testing::internal::GetTimeInMillis();

  for (int i = 0; i < total_test_count(); i++) {
	GetMutableTestInfo(i)->Run();
  }

  elapsed_time_ = ::testing::internal::GetTimeInMillis() - start_timestamp_;

  impl->os_stack_trace_getter()->UponLeavingGTest();

  internal::HandleExceptionsInMethodIfSupported( this, &TestSuite::RunTearDownTestSuite, "TearDownTestSuite()");

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
	ad_hoc_test_result_->Clear();
	internal::function_Stl_utilities::ForEach(test_info_list_, TestInfo::ClearTestResult);
}

// Shuffles the tests in this test suite.
void TestSuite::ShuffleTests( ::jmsd::cutf::internal::Random* random) {
	internal::function_Stl_utilities::Shuffle(random, &test_indices_);
}

// Restores the test order to before the first shuffle.
void TestSuite::UnshuffleTests() {
	for ( size_t i = 0; i < test_indices_.size(); i++ ) {
		test_indices_[ i ] = static_cast< int >( i );
	}
}

// Returns true if and only if the test suite passed.
// static
bool TestSuite::TestSuitePassed( TestSuite const *test_suite ) {
	return test_suite->should_run() && test_suite->Passed();
}

// Returns true if and only if the test suite failed.
// static
bool TestSuite::TestSuiteFailed( TestSuite const *test_suite ) {
	return test_suite->should_run() && test_suite->Failed();
}

// Returns true if and only if test_suite contains at least one test that
// should run.
// static
bool TestSuite::ShouldRunTestSuite( TestSuite const *test_suite ) {
	return test_suite->should_run();
}

// Iterates over a vector of TestSuites, keeping a running sum of the results of calling a given int-returning method on each.
// Returns the sum.
// static
int TestSuite::SumOverTestSuiteList( ::std::vector< TestSuite * > const &case_list, int ( TestSuite::*method )() const ) {
	int sum = 0;

	for ( size_t i = 0; i < case_list.size(); i++ ) {
		sum += ( case_list[ i ]->*method )();
	}

	return sum;
}


} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
