#pragma once

#include "Test_info.hxx"


#include "Test_result.h"

#include "internal/gtest-port.h"


namespace jmsd {
namespace cutf {


// A TestInfo object stores the following information about a test:
//
//   Test suite name
//   Test name
//   Whether the test should be run
//   A function pointer that creates the test object when invoked
//   Test result
//
// The constructor of TestInfo registers itself with the UnitTest
// singleton such that the RUN_ALL_TESTS() macro knows which tests to
// run.
class GTEST_API_ TestInfo {
 public:
  // Destructs a TestInfo object.  This function is not virtual, so
  // don't inherit from TestInfo.
  ~TestInfo();

  // Returns the test suite name.
  const char* test_suite_name() const { return test_suite_name_.c_str(); }

// Legacy API is deprecated but still available
#ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_
  const char* test_case_name() const { return test_suite_name(); }
#endif  // GTEST_REMOVE_LEGACY_TEST_CASEAPI_

  // Returns the test name.
  const char* name() const { return name_.c_str(); }

  // Returns the name of the parameter type, or NULL if this is not a typed
  // or a type-parameterized test.
  const char* type_param() const {
	if (type_param_.get() != nullptr) return type_param_->c_str();
	return nullptr;
  }

  // Returns the text representation of the value parameter, or NULL if this
  // is not a value-parameterized test.
  const char* value_param() const {
	if (value_param_.get() != nullptr) return value_param_->c_str();
	return nullptr;
  }

  // Returns the file name where this test is defined.
  const char* file() const { return location_.file.c_str(); }

  // Returns the line where this test is defined.
  int line() const { return location_.line; }

  // Return true if this test should not be run because it's in another shard.
  bool is_in_another_shard() const { return is_in_another_shard_; }

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
  bool should_run() const { return should_run_; }

  // Returns true if and only if this test will appear in the XML report.
  bool is_reportable() const {
	// The XML report includes tests matching the filter, excluding those
	// run in other shards.
	return matches_filter_ && !is_in_another_shard_;
  }

  // Returns the result of the test.
  const TestResult* result() const { return &result_; }

 private:
#if GTEST_HAS_DEATH_TEST
  friend class internal::DefaultDeathTestFactory;
#endif  // GTEST_HAS_DEATH_TEST
  friend class Test;
  friend class TestSuite;
  friend class internal::UnitTestImpl;
  friend class internal::StreamingListenerTest;
  friend TestInfo* internal::MakeAndRegisterTestInfo(
	  const char* test_suite_name, const char* name, const char* type_param,
	  const char* value_param, internal::CodeLocation code_location,
	  internal::TypeId fixture_class_id, internal::SetUpTestSuiteFunc set_up_tc,
	  internal::TearDownTestSuiteFunc tear_down_tc,
	  internal::TestFactoryBase* factory);

  // Constructs a TestInfo object. The newly constructed instance assumes
  // ownership of the factory object.
  TestInfo(const std::string& test_suite_name, const std::string& name,
		   const char* a_type_param,   // NULL if not a type-parameterized test
		   const char* a_value_param,  // NULL if not a value-parameterized test
		   internal::CodeLocation a_code_location,
		   internal::TypeId fixture_class_id,
		   internal::TestFactoryBase* factory);

  // Increments the number of death tests encountered in this test so
  // far.
  int increment_death_test_count() {
	return result_.increment_death_test_count();
  }

  // Creates the test object, runs it, records its result, and then
  // deletes it.
  void Run();

  static void ClearTestResult(TestInfo* test_info) {
	test_info->result_.Clear();
  }

  // These fields are immutable properties of the test.
  const std::string test_suite_name_;    // test suite name
  const std::string name_;               // Test name
  // Name of the parameter type, or NULL if this is not a typed or a
  // type-parameterized test.
  const std::unique_ptr<const ::std::string> type_param_;
  // Text representation of the value parameter, or NULL if this is not a
  // value-parameterized test.
  const std::unique_ptr<const ::std::string> value_param_;
  internal::CodeLocation location_;
  const internal::TypeId fixture_class_id_;  // ID of the test fixture class
  bool should_run_;           // True if and only if this test should run
  bool is_disabled_;          // True if and only if this test is disabled
  bool matches_filter_;       // True if this test matches the
							  // user-specified filter.
  bool is_in_another_shard_;  // Will be run in another shard.
  internal::TestFactoryBase* const factory_;  // The factory that creates
											  // the test object

  // This field is mutable and needs to be reset before running the
  // test for the second time.
  TestResult result_;

  GTEST_DISALLOW_COPY_AND_ASSIGN_(TestInfo);
};


} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
