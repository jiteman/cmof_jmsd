#pragma once

#include "Unit_test.hxx"


#include "gtest-test-part.h"

#include "gtest-internal-inl.h"

#include "gtest/internal/gtest-port.h"

#include <string>


#include "Scoped_trace.hxx"
#include "Test_event_listeners.hxx"
#include "Environment.hxx"
#include "function_Add_global_test_environment.hxx"
#include "internal/Unit_test_impl.hxx"


namespace testing {


::jmsd::cutf::Environment *AddGlobalTestEnvironment( ::jmsd::cutf::Environment *env );

	namespace internal {


	class ParameterizedTestSuiteRegistry;
	class AssertHelper;
	class StreamingListenerTest;
	class UnitTestRecordPropertyTestHelper;


	// Convenience function for accessing the global UnitTest implementation object.
	void ReportFailureInUnknownLocation(TestPartResult::Type result_type, const std::string& message);


	} // namespace internal
} // namespace testing


namespace jmsd {
namespace cutf {


// A UnitTest consists of a vector of TestSuites.
//
// This is a singleton class.  The only instance of UnitTest is
// created when UnitTest::GetInstance() is first called.  This
// instance is never deleted.
//
// UnitTest is not copyable.
//
// This class is thread-safe as long as the methods are called
// according to their specification.
class JMSD_DEPRECATED_GTEST_API_ UnitTest {
 public:
  // Gets the singleton UnitTest object.  The first time this method
  // is called, a UnitTest object is constructed and returned.
  // Consecutive calls will return the same object.
  static UnitTest* GetInstance();

  // Runs all tests in this UnitTest object and prints the result.
  // Returns 0 if successful, or 1 otherwise.
  //
  // This method can only be called from the main thread.
  //
  // INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
  int Run() GTEST_MUST_USE_RESULT_;

  // Returns the working directory when the first TEST() or TEST_F()
  // was executed.  The UnitTest object owns the string.
  const char* original_working_dir() const;

  // Returns the TestSuite object for the test that's currently running,
  // or NULL if no test is running.
  const TestSuite* current_test_suite() const GTEST_LOCK_EXCLUDED_(mutex_);

// Legacy API is still available but deprecated
#ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_
  const TestCase* current_test_case() const GTEST_LOCK_EXCLUDED_(mutex_);
#endif

  // Returns the TestInfo object for the test that's currently running,
  // or NULL if no test is running.
  const TestInfo* current_test_info() const
	  GTEST_LOCK_EXCLUDED_(mutex_);

  // Returns the random seed used at the start of the current test run.
  int random_seed() const;

  // Returns the ParameterizedTestSuiteRegistry object used to keep track of
  // value-parameterized tests and instantiate and register them.
  //
  // INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
  ::testing::internal::ParameterizedTestSuiteRegistry& parameterized_test_registry()
	  GTEST_LOCK_EXCLUDED_(mutex_);

  // Gets the number of successful test suites.
  int successful_test_suite_count() const;

  // Gets the number of failed test suites.
  int failed_test_suite_count() const;

  // Gets the number of all test suites.
  int total_test_suite_count() const;

  // Gets the number of all test suites that contain at least one test
  // that should run.
  int test_suite_to_run_count() const;

  //  Legacy API is deprecated but still available
#ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_
  int successful_test_case_count() const;
  int failed_test_case_count() const;
  int total_test_case_count() const;
  int test_case_to_run_count() const;
#endif  //  GTEST_REMOVE_LEGACY_TEST_CASEAPI_

  // Gets the number of successful tests.
  int successful_test_count() const;

  // Gets the number of skipped tests.
  int skipped_test_count() const;

  // Gets the number of failed tests.
  int failed_test_count() const;

  // Gets the number of disabled tests that will be reported in the XML report.
  int reportable_disabled_test_count() const;

  // Gets the number of disabled tests.
  int disabled_test_count() const;

  // Gets the number of tests to be printed in the XML report.
  int reportable_test_count() const;

  // Gets the number of all tests.
  int total_test_count() const;

  // Gets the number of tests that should run.
  int test_to_run_count() const;

  // Gets the time of the test program start, in ms from the start of the
  // UNIX epoch.
  ::testing::internal::TimeInMillis start_timestamp() const;

  // Gets the elapsed time, in milliseconds.
  ::testing::internal::TimeInMillis elapsed_time() const;

  // Returns true if and only if the unit test passed (i.e. all test suites
  // passed).
  bool Passed() const;

  // Returns true if and only if the unit test failed (i.e. some test suite
  // failed or something outside of all tests failed).
  bool Failed() const;

  // Gets the i-th test suite among all the test suites. i can range from 0 to
  // total_test_suite_count() - 1. If i is not in that range, returns NULL.
  const TestSuite* GetTestSuite(int i) const;

//  Legacy API is deprecated but still available
#ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_
  const TestCase* GetTestCase(int i) const;
#endif  //  GTEST_REMOVE_LEGACY_TEST_CASEAPI_

  // Returns the TestResult containing information on test failures and
  // properties logged outside of individual test suites.
  const TestResult& ad_hoc_test_result() const;

  // Returns the list of event listeners that can be used to track events
  // inside Google Test.
  ::jmsd::cutf::TestEventListeners& listeners();

 private:
  // Registers and returns a global test environment.  When a test
  // program is run, all global test environments will be set-up in
  // the order they were registered.  After all tests in the program
  // have finished, all global test environments will be torn-down in
  // the *reverse* order they were registered.
  //
  // The UnitTest object takes ownership of the given environment.
  //
  // This method can only be called from the main thread.
  Environment *AddEnvironment( Environment *env );

  // Adds a TestPartResult to the current TestResult object.  All
  // Google Test assertion macros (e.g. ASSERT_TRUE, EXPECT_EQ, etc)
  // eventually call this to report their results.  The user code
  // should use the assertion macros instead of calling this directly.
  void AddTestPartResult(::testing::TestPartResult::Type result_type,
						 const char* file_name,
						 int line_number,
						 const std::string& message,
						 const std::string& os_stack_trace)
	  GTEST_LOCK_EXCLUDED_(mutex_);

  // Adds a TestProperty to the current TestResult object when invoked from
  // inside a test, to current TestSuite's ad_hoc_test_result_ when invoked
  // from SetUpTestSuite or TearDownTestSuite, or to the global property set
  // when invoked elsewhere.  If the result already contains a property with
  // the same key, the value will be updated.
  void RecordProperty(const std::string& key, const std::string& value);

  // Gets the i-th test suite among all the test suites. i can range from 0 to
  // total_test_suite_count() - 1. If i is not in that range, returns NULL.
  TestSuite* GetMutableTestSuite(int i);

  // Accessors for the implementation object.
  internal::UnitTestImpl* impl();
  const internal::UnitTestImpl* impl() const;

  // These classes and functions are friends as they need to access private members of UnitTest.
  friend ::jmsd::cutf::ScopedTrace;
  friend Test;
  friend ::testing::internal::AssertHelper;
  friend ::testing::internal::StreamingListenerTest;
  friend ::testing::internal::UnitTestRecordPropertyTestHelper;
  friend function_Add_global_test_environment;
  friend internal::UnitTestImpl *internal::GetUnitTestImpl();
  friend void ::testing::internal::ReportFailureInUnknownLocation( TestPartResult::Type result_type, const std::string& message);

  UnitTest();
  virtual ~UnitTest();

  // Pushes a trace defined by SCOPED_TRACE() on to the per-thread
  // Google Test trace stack.
  void PushGTestTrace(const ::testing::internal::TraceInfo& trace)
	  GTEST_LOCK_EXCLUDED_(mutex_);

  // Pops a trace from the per-thread Google Test trace stack.
  void PopGTestTrace()
	  GTEST_LOCK_EXCLUDED_(mutex_);

  // Protects mutable state in *impl_.  This is mutable as some const
  // methods need to lock it too.
  mutable ::testing::internal::Mutex mutex_;

  // Opaque implementation object.  This field is never changed once
  // the object is constructed.  We don't mark it as const here, as
  // doing so will cause a warning in the constructor of UnitTest.
  // Mutable state in *impl_ is protected by mutex_.
 internal::UnitTestImpl *impl_;

  // We disallow copying UnitTest.
  GTEST_DISALLOW_COPY_AND_ASSIGN_(UnitTest);
};


} // namespace cutf
} // namespace jmsd


namespace testing {



} // namespace testing
