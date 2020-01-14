#pragma once

#include "Test_suite.hxx"


#include "Test_result.h"

#include "internal/gtest-internal.h"

#include "internal/gtest-port.h"


#include "internal/Unit_test_impl.hxx"


namespace jmsd {
namespace cutf {


// A test suite, which consists of a vector of TestInfos.
//
// TestSuite is not copyable.
class GTEST_API_ TestSuite {
 public:
  // Creates a TestSuite with the given name.
  //
  // TestSuite does NOT have a default constructor.  Always use this
  // constructor to create a TestSuite object.
  //
  // Arguments:
  //
  //   name:         name of the test suite
  //   a_type_param: the name of the test's type parameter, or NULL if
  //                 this is not a type-parameterized test.
  //   set_up_tc:    pointer to the function that sets up the test suite
  //   tear_down_tc: pointer to the function that tears down the test suite
  TestSuite(const char* name, const char* a_type_param,
			::testing::internal::SetUpTestSuiteFunc set_up_tc,
			::testing::internal::TearDownTestSuiteFunc tear_down_tc);

  // Destructor of TestSuite.
  virtual ~TestSuite();

  // Gets the name of the TestSuite.
  const char* name() const { return name_.c_str(); }

  // Returns the name of the parameter type, or NULL if this is not a
  // type-parameterized test suite.
  const char* type_param() const {
	if (type_param_.get() != nullptr) return type_param_->c_str();
	return nullptr;
  }

  // Returns true if any test in this test suite should run.
  bool should_run() const { return should_run_; }

  // Gets the number of successful tests in this test suite.
  int successful_test_count() const;

  // Gets the number of skipped tests in this test suite.
  int skipped_test_count() const;

  // Gets the number of failed tests in this test suite.
  int failed_test_count() const;

  // Gets the number of disabled tests that will be reported in the XML report.
  int reportable_disabled_test_count() const;

  // Gets the number of disabled tests in this test suite.
  int disabled_test_count() const;

  // Gets the number of tests to be printed in the XML report.
  int reportable_test_count() const;

  // Get the number of tests in this test suite that should run.
  int test_to_run_count() const;

  // Gets the number of all tests in this test suite.
  int total_test_count() const;

  // Returns true if and only if the test suite passed.
  bool Passed() const { return !Failed(); }

  // Returns true if and only if the test suite failed.
  bool Failed() const {
	return failed_test_count() > 0 || ad_hoc_test_result().Failed();
  }

  // Returns the elapsed time, in milliseconds.
  ::testing::internal::TimeInMillis elapsed_time() const { return elapsed_time_; }

  // Gets the time of the test suite start, in ms from the start of the
  // UNIX epoch.
  ::testing::internal::TimeInMillis start_timestamp() const { return start_timestamp_; }

  // Returns the i-th test among all the tests. i can range from 0 to
  // total_test_count() - 1. If i is not in that range, returns NULL.
  const ::testing::TestInfo* GetTestInfo(int i) const;

  // Returns the TestResult that holds test properties recorded during
  // execution of SetUpTestSuite and TearDownTestSuite.
  const TestResult& ad_hoc_test_result() const { return ad_hoc_test_result_; }

 private:
  friend Test;
  friend internal::UnitTestImpl;

  // Gets the (mutable) vector of TestInfos in this TestSuite.
  std::vector<::testing::TestInfo*>& test_info_list() { return test_info_list_; }

  // Gets the (immutable) vector of TestInfos in this TestSuite.
  const std::vector<::testing::TestInfo*>& test_info_list() const {
	return test_info_list_;
  }

  // Returns the i-th test among all the tests. i can range from 0 to
  // total_test_count() - 1. If i is not in that range, returns NULL.
  ::testing::TestInfo* GetMutableTestInfo(int i);

  // Sets the should_run member.
  void set_should_run(bool should) { should_run_ = should; }

  // Adds a TestInfo to this test suite.  Will delete the TestInfo upon
  // destruction of the TestSuite object.
  void AddTestInfo(::testing::TestInfo * test_info);

  // Clears the results of all tests in this test suite.
  void ClearResult();

  // Clears the results of all tests in the given test suite.
  static void ClearTestSuiteResult(TestSuite* test_suite) {
	test_suite->ClearResult();
  }

  // Runs every test in this TestSuite.
  void Run();

  // Runs SetUpTestSuite() for this TestSuite.  This wrapper is needed
  // for catching exceptions thrown from SetUpTestSuite().
  void RunSetUpTestSuite() {
	if (set_up_tc_ != nullptr) {
	  (*set_up_tc_)();
	}
  }

  // Runs TearDownTestSuite() for this TestSuite.  This wrapper is
  // needed for catching exceptions thrown from TearDownTestSuite().
  void RunTearDownTestSuite() {
	if (tear_down_tc_ != nullptr) {
	  (*tear_down_tc_)();
	}
  }

  // Returns true if and only if test passed.
  static bool TestPassed(const ::testing::TestInfo* test_info) {
	return test_info->should_run() && test_info->result()->Passed();
  }

  // Returns true if and only if test skipped.
  static bool TestSkipped(const ::testing::TestInfo* test_info) {
	return test_info->should_run() && test_info->result()->Skipped();
  }

  // Returns true if and only if test failed.
  static bool TestFailed(const ::testing::TestInfo* test_info) {
	return test_info->should_run() && test_info->result()->Failed();
  }

  // Returns true if and only if the test is disabled and will be reported in
  // the XML report.
  static bool TestReportableDisabled(const ::testing::TestInfo* test_info) {
	return test_info->is_reportable() && test_info->is_disabled_;
  }

  // Returns true if and only if test is disabled.
  static bool TestDisabled(const ::testing::TestInfo* test_info) {
	return test_info->is_disabled_;
  }

  // Returns true if and only if this test will appear in the XML report.
  static bool TestReportable(const ::testing::TestInfo* test_info) {
	return test_info->is_reportable();
  }

  // Returns true if the given test should run.
  static bool ShouldRunTest(const ::testing::TestInfo* test_info) {
	return test_info->should_run();
  }

  // Shuffles the tests in this test suite.
  void ShuffleTests( ::testing::internal::Random* random);

  // Restores the test order to before the first shuffle.
  void UnshuffleTests();

  // Name of the test suite.
  std::string name_;
  // Name of the parameter type, or NULL if this is not a typed or a
  // type-parameterized test.
  const std::unique_ptr<const ::std::string> type_param_;
  // The vector of TestInfos in their original order.  It owns the
  // elements in the vector.
  std::vector<::testing::TestInfo*> test_info_list_;
  // Provides a level of indirection for the test list to allow easy
  // shuffling and restoring the test order.  The i-th element in this
  // vector is the index of the i-th test in the shuffled test list.
  std::vector<int> test_indices_;
  // Pointer to the function that sets up the test suite.
  ::testing::internal::SetUpTestSuiteFunc set_up_tc_;
  // Pointer to the function that tears down the test suite.
  ::testing::internal::TearDownTestSuiteFunc tear_down_tc_;
  // True if and only if any test in this test suite should run.
  bool should_run_;
  // The start time, in milliseconds since UNIX Epoch.
  ::testing::internal::TimeInMillis start_timestamp_;
  // Elapsed time, in milliseconds.
  ::testing::internal::TimeInMillis elapsed_time_;
  // Holds test properties recorded during execution of SetUpTestSuite and
  // TearDownTestSuite.
  TestResult ad_hoc_test_result_;

  // We disallow copying TestSuites.
  GTEST_DISALLOW_COPY_AND_ASSIGN_(TestSuite);
};


} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
