#pragma once

#include "Test_result.hxx"


#include "gtest-test-part.h"

#include "gtest-internal-inl.h"
#include "internal/gtest-port.h"

#include "Test_property.hxx"
#include "Test_info.hxx"
#include "Test_suite.hxx"
#include "internal/Default_global_test_part_result_reporter.hxx"
#include "internal/Test_result_accessor.hxx"
#include "internal/Unit_test_impl.hxx"
#include "internal/Windows_death_test.hxx"


namespace testing {
namespace internal {


class ExecDeathTest;


} // namespace internal
} // namespace testing



namespace jmsd {
namespace cutf {


// The result of a single Test.  This includes a list of
// TestPartResults, a list of TestProperties, a count of how many
// death tests there are in the Test, and how much time it took to run
// the Test.
//
// TestResult is not copyable.
class JMSD_DEPRECATED_GTEST_API_ TestResult {
 public:
  // Creates an empty TestResult.
  TestResult();

  // D'tor.  Do not inherit from TestResult.
  ~TestResult();

  // Gets the number of all test parts.  This is the sum of the number
  // of successful test parts and the number of failed test parts.
  int total_part_count() const;

  // Returns the number of the test properties.
  int test_property_count() const;

  // Returns true if and only if the test passed (i.e. no test part failed).
  bool Passed() const { return !Skipped() && !Failed(); }

  // Returns true if and only if the test was skipped.
  bool Skipped() const;

  // Returns true if and only if the test failed.
  bool Failed() const;

  // Returns true if and only if the test fatally failed.
  bool HasFatalFailure() const;

  // Returns true if and only if the test has a non-fatal failure.
  bool HasNonfatalFailure() const;

  // Returns the elapsed time, in milliseconds.
  ::testing::internal::TimeInMillis elapsed_time() const { return elapsed_time_; }

  // Gets the time of the test case start, in ms from the start of the
  // UNIX epoch.
  ::testing::internal::TimeInMillis start_timestamp() const { return start_timestamp_; }

  // Returns the i-th test part result among all the results. i can range from 0
  // to total_part_count() - 1. If i is not in that range, aborts the program.
  const ::testing::TestPartResult& GetTestPartResult(int i) const;

  // Returns the i-th test property. i can range from 0 to
  // test_property_count() - 1. If i is not in that range, aborts the
  // program.
  const TestProperty& GetTestProperty(int i) const;

 private:
  friend TestInfo;
  friend TestSuite;
  friend UnitTest;
  friend internal::DefaultGlobalTestPartResultReporter;
  friend internal::TestResultAccessor;
  friend internal::UnitTestImpl;

  friend internal::WindowsDeathTest;
  friend ::testing::internal::ExecDeathTest;
  //friend class internal::FuchsiaDeathTest;

  // Gets the vector of TestPartResults.
  const std::vector<::testing::TestPartResult>& test_part_results() const {
	return test_part_results_;
  }

  // Gets the vector of TestProperties.
  const std::vector< ::jmsd::cutf::TestProperty > &test_properties() const {
	return test_properties_;
  }

  // Sets the start time.
  void set_start_timestamp(::testing::internal::TimeInMillis start) { start_timestamp_ = start; }

  // Sets the elapsed time.
  void set_elapsed_time(::testing::internal::TimeInMillis elapsed) { elapsed_time_ = elapsed; }

  // Adds a test property to the list. The property is validated and may add
  // a non-fatal failure if invalid (e.g., if it conflicts with reserved
  // key names). If a property is already recorded for the same key, the
  // value will be updated, rather than storing multiple values for the same
  // key.  xml_element specifies the element for which the property is being
  // recorded and is used for validation.
  void RecordProperty(const std::string& xml_element,
					  const ::jmsd::cutf::TestProperty& test_property);

  // Adds a failure if the key is a reserved attribute of Google Test
  // testsuite tags.  Returns true if the property is valid.
  // FIXME: Validate attribute names are legal and human readable.
  static bool ValidateTestProperty(const std::string& xml_element,
								   const ::jmsd::cutf::TestProperty& test_property);

  // Adds a test part result to the list.
  void AddTestPartResult(const ::testing::TestPartResult& test_part_result);

  // Returns the death test count.
  int death_test_count() const { return death_test_count_; }

  // Increments the death test count, returning the new count.
  int increment_death_test_count() { return ++death_test_count_; }

  // Clears the test part results.
  void ClearTestPartResults();

  // Clears the object.
  void Clear();

  // Protects mutable state of the property vector and of owned
  // properties, whose values may be updated.
  ::testing::internal::Mutex test_properites_mutex_;

  // The vector of TestPartResults
  std::vector<::testing::TestPartResult> test_part_results_;
  // The vector of TestProperties
  std::vector< ::jmsd::cutf::TestProperty > test_properties_;
  // Running count of death tests.
  int death_test_count_;
  // The start time, in milliseconds since UNIX Epoch.
  ::testing::internal::TimeInMillis start_timestamp_;
  // The elapsed time, in milliseconds.
  ::testing::internal::TimeInMillis elapsed_time_;

  // We disallow copying TestResult.
  GTEST_DISALLOW_COPY_AND_ASSIGN_(TestResult);
};  // class TestResult


} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
