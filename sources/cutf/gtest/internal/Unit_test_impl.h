#pragma once

#include "Unit_test_impl.hxx"


#include "gtest/Test_suite.h"

#include "gtest/gtest-test-part.h"

#include "gtest-port.h"


namespace jmsd {
namespace cutf {
namespace internal {


// The private implementation of the UnitTest class.  We don't protect
// the methods under a mutex, as this class is not accessible by a
// user and the UnitTest class that delegates work to this class does
// proper locking.
class GTEST_API_ UnitTestImpl {
 public:
  explicit UnitTestImpl(::jmsd::cutf::UnitTest* parent);
  virtual ~UnitTestImpl();

  // There are two different ways to register your own TestPartResultReporter.
  // You can register your own repoter to listen either only for test results
  // from the current thread or for results from all threads.
  // By default, each per-thread test result repoter just passes a new
  // TestPartResult to the global test result reporter, which registers the
  // test part result for the currently running test.

  // Returns the global test part result reporter.
  ::testing::TestPartResultReporterInterface* GetGlobalTestPartResultReporter();

  // Sets the global test part result reporter.
  void SetGlobalTestPartResultReporter(
	  ::testing::TestPartResultReporterInterface* reporter);

  // Returns the test part result reporter for the current thread.
  ::testing::TestPartResultReporterInterface* GetTestPartResultReporterForCurrentThread();

  // Sets the test part result reporter for the current thread.
  void SetTestPartResultReporterForCurrentThread(
	  ::testing::TestPartResultReporterInterface* reporter);

  // Gets the number of successful test suites.
  int successful_test_suite_count() const;

  // Gets the number of failed test suites.
  int failed_test_suite_count() const;

  // Gets the number of all test suites.
  int total_test_suite_count() const;

  // Gets the number of all test suites that contain at least one test
  // that should run.
  int test_suite_to_run_count() const;

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
  ::testing::internal::TimeInMillis start_timestamp() const { return start_timestamp_; }

  // Gets the elapsed time, in milliseconds.
  ::testing::internal::TimeInMillis elapsed_time() const { return elapsed_time_; }

  // Returns true if and only if the unit test passed (i.e. all test suites
  // passed).
  bool Passed() const { return !Failed(); }

  // Returns true if and only if the unit test failed (i.e. some test suite
  // failed or something outside of all tests failed).
  bool Failed() const {
	return failed_test_suite_count() > 0 || ad_hoc_test_result()->Failed();
  }

  // Gets the i-th test suite among all the test suites. i can range from 0 to
  // total_test_suite_count() - 1. If i is not in that range, returns NULL.
  const TestSuite* GetTestSuite(int i) const {
	const int index = GetElementOr(test_suite_indices_, i, -1);
	return index < 0 ? nullptr : test_suites_[static_cast<size_t>(i)];
  }

  //  Legacy API is deprecated but still available
#ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_
  const TestCase* GetTestCase(int i) const { return GetTestSuite(i); }
#endif  //  GTEST_REMOVE_LEGACY_TEST_CASEAPI_

  // Gets the i-th test suite among all the test suites. i can range from 0 to
  // total_test_suite_count() - 1. If i is not in that range, returns NULL.
  TestSuite* GetMutableSuiteCase(int i) {
	const int index = GetElementOr(test_suite_indices_, i, -1);
	return index < 0 ? nullptr : test_suites_[static_cast<size_t>(index)];
  }

  // Provides access to the event listener list.
  TestEventListeners* listeners() { return &listeners_; }

  // Returns the TestResult for the test that's currently running, or
  // the TestResult for the ad hoc test if no test is running.
  TestResult* current_test_result();

  // Returns the TestResult for the ad hoc test.
  const TestResult* ad_hoc_test_result() const { return &ad_hoc_test_result_; }

  // Sets the OS stack trace getter.
  //
  // Does nothing if the input and the current OS stack trace getter
  // are the same; otherwise, deletes the old getter and makes the
  // input the current getter.
  void set_os_stack_trace_getter(OsStackTraceGetterInterface* getter);

  // Returns the current OS stack trace getter if it is not NULL;
  // otherwise, creates an OsStackTraceGetter, makes it the current
  // getter, and returns it.
  OsStackTraceGetterInterface* os_stack_trace_getter();

  // Returns the current OS stack trace as an std::string.
  //
  // The maximum number of stack frames to be included is specified by
  // the gtest_stack_trace_depth flag.  The skip_count parameter
  // specifies the number of top frames to be skipped, which doesn't
  // count against the number of frames to be included.
  //
  // For example, if Foo() calls Bar(), which in turn calls
  // CurrentOsStackTraceExceptTop(1), Foo() will be included in the
  // trace but Bar() and CurrentOsStackTraceExceptTop() won't.
  std::string CurrentOsStackTraceExceptTop(int skip_count) GTEST_NO_INLINE_;

  // Finds and returns a TestSuite with the given name.  If one doesn't
  // exist, creates one and returns it.
  //
  // Arguments:
  //
  //   test_suite_name: name of the test suite
  //   type_param:     the name of the test's type parameter, or NULL if
  //                   this is not a typed or a type-parameterized test.
  //   set_up_tc:      pointer to the function that sets up the test suite
  //   tear_down_tc:   pointer to the function that tears down the test suite
  TestSuite* GetTestSuite(const char* test_suite_name, const char* type_param,
						  internal::SetUpTestSuiteFunc set_up_tc,
						  internal::TearDownTestSuiteFunc tear_down_tc);

//  Legacy API is deprecated but still available
#ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_
  TestCase* GetTestCase(const char* test_case_name, const char* type_param,
						internal::SetUpTestSuiteFunc set_up_tc,
						internal::TearDownTestSuiteFunc tear_down_tc) {
	return GetTestSuite(test_case_name, type_param, set_up_tc, tear_down_tc);
  }
#endif  //  GTEST_REMOVE_LEGACY_TEST_CASEAPI_

  // Adds a TestInfo to the unit test.
  //
  // Arguments:
  //
  //   set_up_tc:    pointer to the function that sets up the test suite
  //   tear_down_tc: pointer to the function that tears down the test suite
  //   test_info:    the TestInfo object
  void AddTestInfo(internal::SetUpTestSuiteFunc set_up_tc,
				   internal::TearDownTestSuiteFunc tear_down_tc,
				   TestInfo* test_info) {
	// In order to support thread-safe death tests, we need to
	// remember the original working directory when the test program
	// was first invoked.  We cannot do this in RUN_ALL_TESTS(), as
	// the user may have changed the current directory before calling
	// RUN_ALL_TESTS().  Therefore we capture the current directory in
	// AddTestInfo(), which is called to register a TEST or TEST_F
	// before main() is reached.
	if (original_working_dir_.IsEmpty()) {
	  original_working_dir_.Set(FilePath::GetCurrentDir());
	  GTEST_CHECK_(!original_working_dir_.IsEmpty())
		  << "Failed to get the current working directory.";
	}

	GetTestSuite(test_info->test_suite_name(), test_info->type_param(),
				 set_up_tc, tear_down_tc)
		->AddTestInfo(test_info);
  }

  // Returns ParameterizedTestSuiteRegistry object used to keep track of
  // value-parameterized tests and instantiate and register them.
  internal::ParameterizedTestSuiteRegistry& parameterized_test_registry() {
	return parameterized_test_registry_;
  }

  // Sets the TestSuite object for the test that's currently running.
  void set_current_test_suite(TestSuite* a_current_test_suite) {
	current_test_suite_ = a_current_test_suite;
  }

  // Sets the TestInfo object for the test that's currently running.  If
  // current_test_info is NULL, the assertion results will be stored in
  // ad_hoc_test_result_.
  void set_current_test_info(TestInfo* a_current_test_info) {
	current_test_info_ = a_current_test_info;
  }

  // Registers all parameterized tests defined using TEST_P and
  // INSTANTIATE_TEST_SUITE_P, creating regular tests for each test/parameter
  // combination. This method can be called more then once; it has guards
  // protecting from registering the tests more then once.  If
  // value-parameterized tests are disabled, RegisterParameterizedTests is
  // present but does nothing.
  void RegisterParameterizedTests();

  // Runs all tests in this UnitTest object, prints the result, and
  // returns true if all tests are successful.  If any exception is
  // thrown during a test, this test is considered to be failed, but
  // the rest of the tests will still be run.
  bool RunAllTests();

  // Clears the results of all tests, except the ad hoc tests.
  void ClearNonAdHocTestResult() {
	ForEach(test_suites_, TestSuite::ClearTestSuiteResult);
  }

  // Clears the results of ad-hoc test assertions.
  void ClearAdHocTestResult() {
	ad_hoc_test_result_.Clear();
  }

  // Adds a TestProperty to the current TestResult object when invoked in a
  // context of a test or a test suite, or to the global property set. If the
  // result already contains a property with the same key, the value will be
  // updated.
  void RecordProperty(const ::jmsd::cutf::TestProperty& test_property);

  enum ReactionToSharding {
	HONOR_SHARDING_PROTOCOL,
	IGNORE_SHARDING_PROTOCOL
  };

  // Matches the full name of each test against the user-specified
  // filter to decide whether the test should run, then records the
  // result in each TestSuite and TestInfo object.
  // If shard_tests == HONOR_SHARDING_PROTOCOL, further filters tests
  // based on sharding variables in the environment.
  // Returns the number of tests that should run.
  int FilterTests(ReactionToSharding shard_tests);

  // Prints the names of the tests matching the user-specified filter flag.
  void ListTestsMatchingFilter();

  const TestSuite* current_test_suite() const { return current_test_suite_; }
  TestInfo* current_test_info() { return current_test_info_; }
  const TestInfo* current_test_info() const { return current_test_info_; }

  // Returns the vector of environments that need to be set-up/torn-down
  // before/after the tests are run.
  std::vector<Environment*>& environments() { return environments_; }

  // Getters for the per-thread Google Test trace stack.
  std::vector<TraceInfo>& gtest_trace_stack() {
	return *(gtest_trace_stack_.pointer());
  }
  const std::vector<TraceInfo>& gtest_trace_stack() const {
	return gtest_trace_stack_.get();
  }

#if GTEST_HAS_DEATH_TEST
  void InitDeathTestSubprocessControlInfo() {
	internal_run_death_test_flag_.reset(ParseInternalRunDeathTestFlag());
  }
  // Returns a pointer to the parsed --gtest_internal_run_death_test
  // flag, or NULL if that flag was not specified.
  // This information is useful only in a death test child process.
  // Must not be called before a call to InitGoogleTest.
  const InternalRunDeathTestFlag* internal_run_death_test_flag() const {
	return internal_run_death_test_flag_.get();
  }

  // Returns a pointer to the current death test factory.
  internal::DeathTestFactory* death_test_factory() {
	return death_test_factory_.get();
  }

  void SuppressTestEventsIfInSubprocess();

  friend class ReplaceDeathTestFactory;
#endif  // GTEST_HAS_DEATH_TEST

  // Initializes the event listener performing XML output as specified by
  // UnitTestOptions. Must not be called before InitGoogleTest.
  void ConfigureXmlOutput();

#if GTEST_CAN_STREAM_RESULTS_
  // Initializes the event listener for streaming test results to a socket.
  // Must not be called before InitGoogleTest.
  void ConfigureStreamingOutput();
#endif

  // Performs initialization dependent upon flag values obtained in
  // ParseGoogleTestFlagsOnly.  Is called from InitGoogleTest after the call to
  // ParseGoogleTestFlagsOnly.  In case a user neglects to call InitGoogleTest
  // this function is also called from RunAllTests.  Since this function can be
  // called more than once, it has to be idempotent.
  void PostFlagParsingInit();

  // Gets the random seed used at the start of the current test iteration.
  int random_seed() const { return random_seed_; }

  // Gets the random number generator.
  internal::Random* random() { return &random_; }

  // Shuffles all test suites, and the tests within each test suite,
  // making sure that death tests are still run first.
  void ShuffleTests();

  // Restores the test suites and tests to their order before the first shuffle.
  void UnshuffleTests();

  // Returns the value of GTEST_FLAG(catch_exceptions) at the moment
  // UnitTest::Run() starts.
  bool catch_exceptions() const { return catch_exceptions_; }

 private:
  friend ::jmsd::cutf::UnitTest;

  // Used by UnitTest::Run() to capture the state of
  // GTEST_FLAG(catch_exceptions) at the moment it starts.
  void set_catch_exceptions(bool value) { catch_exceptions_ = value; }

  // The UnitTest object that owns this implementation object.
  ::jmsd::cutf::UnitTest* const parent_;

  // The working directory when the first TEST() or TEST_F() was
  // executed.
  internal::FilePath original_working_dir_;

  // The default test part result reporters.
  DefaultGlobalTestPartResultReporter default_global_test_part_result_reporter_;
  DefaultPerThreadTestPartResultReporter
	  default_per_thread_test_part_result_reporter_;

  // Points to (but doesn't own) the global test part result reporter.
  TestPartResultReporterInterface* global_test_part_result_repoter_;

  // Protects read and write access to global_test_part_result_reporter_.
  internal::Mutex global_test_part_result_reporter_mutex_;

  // Points to (but doesn't own) the per-thread test part result reporter.
  internal::ThreadLocal<TestPartResultReporterInterface*>
	  per_thread_test_part_result_reporter_;

  // The vector of environments that need to be set-up/torn-down
  // before/after the tests are run.
  std::vector<Environment*> environments_;

  // The vector of TestSuites in their original order.  It owns the
  // elements in the vector.
  std::vector<TestSuite*> test_suites_;

  // Provides a level of indirection for the test suite list to allow
  // easy shuffling and restoring the test suite order.  The i-th
  // element of this vector is the index of the i-th test suite in the
  // shuffled order.
  std::vector<int> test_suite_indices_;

  // ParameterizedTestRegistry object used to register value-parameterized
  // tests.
  internal::ParameterizedTestSuiteRegistry parameterized_test_registry_;

  // Indicates whether RegisterParameterizedTests() has been called already.
  bool parameterized_tests_registered_;

  // Index of the last death test suite registered.  Initially -1.
  int last_death_test_suite_;

  // This points to the TestSuite for the currently running test.  It
  // changes as Google Test goes through one test suite after another.
  // When no test is running, this is set to NULL and Google Test
  // stores assertion results in ad_hoc_test_result_.  Initially NULL.
  TestSuite* current_test_suite_;

  // This points to the TestInfo for the currently running test.  It
  // changes as Google Test goes through one test after another.  When
  // no test is running, this is set to NULL and Google Test stores
  // assertion results in ad_hoc_test_result_.  Initially NULL.
  TestInfo* current_test_info_;

  // Normally, a user only writes assertions inside a TEST or TEST_F,
  // or inside a function called by a TEST or TEST_F.  Since Google
  // Test keeps track of which test is current running, it can
  // associate such an assertion with the test it belongs to.
  //
  // If an assertion is encountered when no TEST or TEST_F is running,
  // Google Test attributes the assertion result to an imaginary "ad hoc"
  // test, and records the result in ad_hoc_test_result_.
  TestResult ad_hoc_test_result_;

  // The list of event listeners that can be used to track events inside
  // Google Test.
  TestEventListeners listeners_;

  // The OS stack trace getter.  Will be deleted when the UnitTest
  // object is destructed.  By default, an OsStackTraceGetter is used,
  // but the user can set this field to use a custom getter if that is
  // desired.
  OsStackTraceGetterInterface* os_stack_trace_getter_;

  // True if and only if PostFlagParsingInit() has been called.
  bool post_flag_parse_init_performed_;

  // The random number seed used at the beginning of the test run.
  int random_seed_;

  // Our random number generator.
  internal::Random random_;

  // The time of the test program start, in ms from the start of the
  // UNIX epoch.
  TimeInMillis start_timestamp_;

  // How long the test took to run, in milliseconds.
  TimeInMillis elapsed_time_;

#if GTEST_HAS_DEATH_TEST
  // The decomposed components of the gtest_internal_run_death_test flag,
  // parsed when RUN_ALL_TESTS is called.
  std::unique_ptr<InternalRunDeathTestFlag> internal_run_death_test_flag_;
  std::unique_ptr<internal::DeathTestFactory> death_test_factory_;
#endif  // GTEST_HAS_DEATH_TEST

  // A per-thread stack of traces created by the SCOPED_TRACE() macro.
  internal::ThreadLocal<std::vector<TraceInfo> > gtest_trace_stack_;

  // The value of GTEST_FLAG(catch_exceptions) at the moment RunAllTests()
  // starts.
  bool catch_exceptions_;

  GTEST_DISALLOW_COPY_AND_ASSIGN_(UnitTestImpl);
};  // class UnitTestImpl


} // namespace internal
} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
