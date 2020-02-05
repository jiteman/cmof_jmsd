#include "Unit_test_impl.h"


#include "gtest/Unit_test.h"
#include "gtest/Pretty_unit_test_result_printer.h"
#include "gtest/Environment.h"
#include "gtest/gtest-constants.h"

#include "Xml_unit_test_result_printer.h"
#include "Json_test_result_printer.h"
#include "Unit_test_options.h"
#include "Colored_print.h"
#include "function_Open_file_for_writing.h"
#include "function_Is_initialized.h"
#include "function_Int32_from_environment_or_die.h"
#include "function_Write_to_shard_status_file_if_needed.h"
#include "function_Should_run_test_on_shard.h"
#include "function_String_stream_to_string.h"
#include "Streaming_listener.h"

#include "gtest-flags-internal.h"
#include "gtest-constants-internal.h"

#include "function_Stl_utilities.hin"
#include "function_Delete.hin"

#include "gtest-death-test-internal.h"


namespace jmsd {
namespace cutf {
namespace internal {


// Convenience function for accessing the global UnitTest implementation object.
UnitTestImpl *GetUnitTestImpl() {
  return UnitTest::GetInstance()->impl();
}


// Returns the global test part result reporter.
::testing::TestPartResultReporterInterface *UnitTestImpl::GetGlobalTestPartResultReporter() {
  ::testing::internal::MutexLock lock(&global_test_part_result_reporter_mutex_);
  return global_test_part_result_repoter_;
}

// Sets the global test part result reporter.
void UnitTestImpl::SetGlobalTestPartResultReporter(
	::testing::TestPartResultReporterInterface* reporter )
{
  ::testing::internal::MutexLock lock(&global_test_part_result_reporter_mutex_);
  global_test_part_result_repoter_ = reporter;
}

// Returns the test part result reporter for the current thread.
::testing::TestPartResultReporterInterface *UnitTestImpl::GetTestPartResultReporterForCurrentThread() {
  return per_thread_test_part_result_reporter_.get();
}

// Sets the test part result reporter for the current thread.
void UnitTestImpl::SetTestPartResultReporterForCurrentThread(
	::testing::TestPartResultReporterInterface* reporter)
{
  per_thread_test_part_result_reporter_.set(reporter);
}

// Gets the number of successful test suites.
int UnitTestImpl::successful_test_suite_count() const {
  return function_Stl_utilities::CountIf(test_suites_, TestSuite::TestSuitePassed);
}

// Gets the number of failed test suites.
int UnitTestImpl::failed_test_suite_count() const {
  return function_Stl_utilities::CountIf(test_suites_, TestSuite::TestSuiteFailed);
}

// Gets the number of all test suites.
int UnitTestImpl::total_test_suite_count() const {
  return static_cast<int>(test_suites_.size());
}

// Gets the number of all test suites that contain at least one test
// that should run.
int UnitTestImpl::test_suite_to_run_count() const {
  return function_Stl_utilities::CountIf(test_suites_, TestSuite::ShouldRunTestSuite);
}

// Gets the number of successful tests.
int UnitTestImpl::successful_test_count() const {
  return TestSuite::SumOverTestSuiteList(test_suites_, &TestSuite::successful_test_count);
}

// Gets the number of skipped tests.
int UnitTestImpl::skipped_test_count() const {
  return TestSuite::SumOverTestSuiteList(test_suites_, &TestSuite::skipped_test_count);
}

// Gets the number of failed tests.
int UnitTestImpl::failed_test_count() const {
  return TestSuite::SumOverTestSuiteList(test_suites_, &TestSuite::failed_test_count);
}

// Gets the number of disabled tests that will be reported in the XML report.
int UnitTestImpl::reportable_disabled_test_count() const {
  return TestSuite::SumOverTestSuiteList(test_suites_,
							  &TestSuite::reportable_disabled_test_count);
}

// Gets the number of disabled tests.
int UnitTestImpl::disabled_test_count() const {
  return TestSuite::SumOverTestSuiteList(test_suites_, &TestSuite::disabled_test_count);
}

// Gets the number of tests to be printed in the XML report.
int UnitTestImpl::reportable_test_count() const {
  return TestSuite::SumOverTestSuiteList(test_suites_, &TestSuite::reportable_test_count);
}

// Gets the number of all tests.
int UnitTestImpl::total_test_count() const {
  return TestSuite::SumOverTestSuiteList(test_suites_, &TestSuite::total_test_count);
}

// Gets the number of tests that should run.
int UnitTestImpl::test_to_run_count() const {
  return TestSuite::SumOverTestSuiteList(test_suites_, &TestSuite::test_to_run_count);
}

// Gets the time of the test program start, in ms from the start of the UNIX epoch.
::testing::internal::TimeInMillis UnitTestImpl::start_timestamp() const {
	return start_timestamp_;
}

// Gets the elapsed time, in milliseconds.
::testing::internal::TimeInMillis UnitTestImpl::elapsed_time() const {
	return elapsed_time_;
}

// Returns true if and only if the unit test passed (i.e. all test suites passed).
bool UnitTestImpl::Passed() const {
	return !Failed();
}

// Returns true if and only if the unit test failed (i.e. some test suite
// failed or something outside of all tests failed).
bool UnitTestImpl::Failed() const {
	return failed_test_suite_count() > 0 || ad_hoc_test_result()->Failed();
}

// Gets the i-th test suite among all the test suites. i can range from 0 to
// total_test_suite_count() - 1. If i is not in that range, returns NULL.
const TestSuite* UnitTestImpl::GetTestSuite(int i) const {
	const int index = function_Stl_utilities::GetElementOr(test_suite_indices_, i, -1);

	return index < 0 ? nullptr : test_suites_[static_cast<size_t>(i)];
}

//  Legacy API is deprecated but still available
#ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_
const TestCase* UnitTestImpl::GetTestCase(int i) const {
	return GetTestSuite(i);
}

TestCase* UnitTestImpl::GetTestCase(
	const char* test_case_name,
	const char* type_param,
	::testing::internal::SetUpTestSuiteFunc set_up_tc,
	::testing::internal::TearDownTestSuiteFunc tear_down_tc)
{
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
void UnitTestImpl::AddTestInfo(
	::testing::internal::SetUpTestSuiteFunc set_up_tc,
	::testing::internal::TearDownTestSuiteFunc tear_down_tc,
	TestInfo* test_info)
{
	// In order to support thread-safe death tests, we need to
	// remember the original working directory when the test program
	// was first invoked.  We cannot do this in RUN_ALL_TESTS(), as
	// the user may have changed the current directory before calling
	// RUN_ALL_TESTS().  Therefore we capture the current directory in
	// AddTestInfo(), which is called to register a TEST or TEST_F
	// before main() is reached.
	if ( original_working_dir_.IsEmpty() ) {
		original_working_dir_.Set( ::testing::internal::FilePath::GetCurrentDir() );
		GTEST_CHECK_( !original_working_dir_.IsEmpty() ) << "Failed to get the current working directory.";
	}

	GetTestSuite( test_info->test_suite_name(), test_info->type_param(), set_up_tc, tear_down_tc )->AddTestInfo( test_info );
}

// Gets the i-th test suite among all the test suites. i can range from 0 to
// total_test_suite_count() - 1. If i is not in that range, returns NULL.
TestSuite *UnitTestImpl::GetMutableSuiteCase(int i) {
	const int index = function_Stl_utilities::GetElementOr(test_suite_indices_, i, -1);

	return index < 0 ? nullptr : test_suites_[static_cast<size_t>(index)];
}

// Returns ParameterizedTestSuiteRegistry object used to keep track of
// value-parameterized tests and instantiate and register them.
::testing::internal::ParameterizedTestSuiteRegistry &UnitTestImpl::parameterized_test_registry() {
	return parameterized_test_registry_;
}

// Sets the TestSuite object for the test that's currently running.
void UnitTestImpl::set_current_test_suite( TestSuite *a_current_test_suite ) {
	current_test_suite_ = a_current_test_suite;
}

// Sets the TestInfo object for the test that's currently running.  If
// current_test_info is NULL, the assertion results will be stored in
// ad_hoc_test_result_.
void UnitTestImpl::set_current_test_info( TestInfo *a_current_test_info ) {
	current_test_info_ = a_current_test_info;
}

// Provides access to the event listener list.
TestEventListeners* UnitTestImpl::listeners() {
	return &listeners_;
}

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
std::string UnitTestImpl::CurrentOsStackTraceExceptTop(int skip_count) {
  return os_stack_trace_getter()->CurrentStackTrace(
	  static_cast<int>( ::testing:: GTEST_FLAG(stack_trace_depth)),
	  skip_count + 1
	  // Skips the user-specified number of frames plus this function
	  // itself.
	  );  // NOLINT
}

UnitTestImpl::UnitTestImpl(UnitTest* parent)
	: parent_(parent),
	  GTEST_DISABLE_MSC_WARNINGS_PUSH_(4355 /* using this in initializer */)
		  default_global_test_part_result_reporter_(this),
	  default_per_thread_test_part_result_reporter_(this),
	  GTEST_DISABLE_MSC_WARNINGS_POP_() global_test_part_result_repoter_(
		  &default_global_test_part_result_reporter_),
	  per_thread_test_part_result_reporter_(
		  &default_per_thread_test_part_result_reporter_),
	  parameterized_test_registry_(),
	  parameterized_tests_registered_(false),
	  last_death_test_suite_(-1),
	  current_test_suite_(nullptr),
	  current_test_info_(nullptr),
	  ad_hoc_test_result_(),
	  os_stack_trace_getter_(nullptr),
	  post_flag_parse_init_performed_(false),
	  random_seed_(0),  // Will be overridden by the flag before first use.
	  random_(0),       // Will be reseeded before first use.
	  start_timestamp_(0),
	  elapsed_time_(0),
#if GTEST_HAS_DEATH_TEST
	  death_test_factory_(new ::testing::internal::DefaultDeathTestFactory),
#endif
	  // Will be overridden by the flag before first use.
	  catch_exceptions_(false)
{
	listeners()->SetDefaultResultPrinter( new PrettyUnitTestResultPrinter );
}

UnitTestImpl::~UnitTestImpl() {
	function_Stl_utilities::ForEach( test_suites_, internal::function_Delete::Delete< TestSuite > ); // Deletes every TestSuite.
	function_Stl_utilities::ForEach( environments_, internal::function_Delete::Delete< Environment > ); // Deletes every Environment.

	delete os_stack_trace_getter_;
}

// Adds a TestProperty to the current TestResult object when invoked in a
// context of a test, to current test suite's ad_hoc_test_result when invoke
// from SetUpTestSuite/TearDownTestSuite, or to the global property set
// otherwise.  If the result already contains a property with the same key,
// the value will be updated.
void UnitTestImpl::RecordProperty(const TestProperty& test_property) {
  std::string xml_element;
  TestResult* test_result;  // TestResult appropriate for property recording.

  if (current_test_info_ != nullptr) {
	xml_element = "testcase";
	test_result = current_test_info_->result_.get();
  } else if (current_test_suite_ != nullptr) {
	xml_element = "testsuite";
	test_result = current_test_suite_->ad_hoc_test_result_.get();
  } else {
	xml_element = "testsuites";
	test_result = &ad_hoc_test_result_;
  }
  test_result->RecordProperty(xml_element, test_property);
}

#if GTEST_HAS_DEATH_TEST

void UnitTestImpl::InitDeathTestSubprocessControlInfo() {
	internal_run_death_test_flag_.reset( ::testing::internal::ParseInternalRunDeathTestFlag() );
}
// Returns a pointer to the parsed --gtest_internal_run_death_test
// flag, or NULL if that flag was not specified.
// This information is useful only in a death test child process.
// Must not be called before a call to InitGoogleTest.
const ::testing::internal::InternalRunDeathTestFlag *UnitTestImpl::internal_run_death_test_flag() const {
	return internal_run_death_test_flag_.get();
}

// Returns a pointer to the current death test factory.
::testing::internal::DeathTestFactory *UnitTestImpl::death_test_factory() {
	return death_test_factory_.get();
}

// Disables event forwarding if the control is currently in a death test
// subprocess. Must not be called before InitGoogleTest.
void UnitTestImpl::SuppressTestEventsIfInSubprocess() {
  if (internal_run_death_test_flag_.get() != nullptr)
	listeners()->SuppressEventForwarding();
}
#endif  // GTEST_HAS_DEATH_TEST

// Initializes event listeners performing XML output as specified by
// UnitTestOptions. Must not be called before InitGoogleTest.
void UnitTestImpl::ConfigureXmlOutput() {
  const std::string& output_format = UnitTestOptions::GetOutputFormat();
  if (output_format == "xml") {
	listeners()->SetDefaultXmlGenerator( new XmlUnitTestResultPrinter( UnitTestOptions::GetAbsolutePathToOutputFile().c_str() ) );
  } else if (output_format == "json") {
	listeners()->SetDefaultXmlGenerator( new JsonUnitTestResultPrinter( UnitTestOptions::GetAbsolutePathToOutputFile().c_str() ) );
  } else if (output_format != "") {
	GTEST_LOG_(WARNING) << "WARNING: unrecognized output format \""
						<< output_format << "\" ignored.";
  }
}

#if GTEST_CAN_STREAM_RESULTS_
// Initializes event listeners for streaming test results in string form.
// Must not be called before InitGoogleTest.
void UnitTestImpl::ConfigureStreamingOutput() {
  const std::string& target = ::testing:: GTEST_FLAG(stream_result_to);
  if (!target.empty()) {
	const size_t pos = target.find(':');
	if (pos != std::string::npos) {
	  listeners()->Append(new ::jmsd::cutf::internal::StreamingListener(target.substr(0, pos),
												target.substr(pos+1)));
	} else {
	  GTEST_LOG_(WARNING) << "unrecognized streaming target \"" << target
						  << "\" ignored.";
	}
  }
}
#endif  // GTEST_CAN_STREAM_RESULTS_

// Performs initialization dependent upon flag values obtained in
// ParseGoogleTestFlagsOnly.  Is called from InitGoogleTest after the call to
// ParseGoogleTestFlagsOnly.  In case a user neglects to call InitGoogleTest
// this function is also called from RunAllTests.  Since this function can be
// called more than once, it has to be idempotent.
void UnitTestImpl::PostFlagParsingInit() {
  // Ensures that this function does not execute more than once.
  if (!post_flag_parse_init_performed_) {
	post_flag_parse_init_performed_ = true;

#if defined(GTEST_CUSTOM_TEST_EVENT_LISTENER_)
	// Register to send notifications about key process state changes.
	listeners()->Append(new GTEST_CUSTOM_TEST_EVENT_LISTENER_());
#endif  // defined(GTEST_CUSTOM_TEST_EVENT_LISTENER_)

#if GTEST_HAS_DEATH_TEST
	InitDeathTestSubprocessControlInfo();
	SuppressTestEventsIfInSubprocess();
#endif  // GTEST_HAS_DEATH_TEST

	// Registers parameterized tests. This makes parameterized tests
	// available to the UnitTest reflection API without running
	// RUN_ALL_TESTS.
	RegisterParameterizedTests();

	// Configures listeners for XML output. This makes it possible for users
	// to shut down the default XML output before invoking RUN_ALL_TESTS.
	ConfigureXmlOutput();

#if GTEST_CAN_STREAM_RESULTS_
	// Configures listeners for streaming test results to the specified server.
	ConfigureStreamingOutput();
#endif  // GTEST_CAN_STREAM_RESULTS_

#if GTEST_HAS_ABSL
	if (GTEST_FLAG(install_failure_signal_handler)) {
	  absl::FailureSignalHandlerOptions options;
	  absl::InstallFailureSignalHandler(options);
	}
#endif  // GTEST_HAS_ABSL
  }
}

// Gets the random seed used at the start of the current test iteration.
int UnitTestImpl::random_seed() const {
	return random_seed_;
}

// Gets the random number generator.
internal::Random *UnitTestImpl::random() {
	return &random_;
}

// A predicate that checks the name of a TestSuite against a known
// value.
//
// This is used for implementation of the UnitTest class only.  We put
// it in the anonymous namespace to prevent polluting the outer
// namespace.
//
// TestSuiteNameIs is copyable.
class TestSuiteNameIs {
 public:
  // Constructor.
  explicit TestSuiteNameIs(const std::string& name) : name_(name) {}

  // Returns true if and only if the name of test_suite matches name_.
  bool operator()(const TestSuite* test_suite) const {
	return test_suite != nullptr &&
		   strcmp(test_suite->name(), name_.c_str()) == 0;
  }

 private:
  std::string name_;
};

// Finds and returns a TestSuite with the given name.  If one doesn't
// exist, creates one and returns it.  It's the CALLER'S
// RESPONSIBILITY to ensure that this function is only called WHEN THE
// TESTS ARE NOT SHUFFLED.
//
// Arguments:
//
//   test_suite_name: name of the test suite
//   type_param:     the name of the test suite's type parameter, or NULL if
//                   this is not a typed or a type-parameterized test suite.
//   set_up_tc:      pointer to the function that sets up the test suite
//   tear_down_tc:   pointer to the function that tears down the test suite
TestSuite* UnitTestImpl::GetTestSuite(
	const char* test_suite_name, const char* type_param,
	::testing::internal::SetUpTestSuiteFunc set_up_tc,
	::testing::internal::TearDownTestSuiteFunc tear_down_tc) {
  // Can we find a TestSuite with the given name?
  const auto test_suite =
	  std::find_if(test_suites_.rbegin(), test_suites_.rend(),
				   TestSuiteNameIs(test_suite_name));

  if (test_suite != test_suites_.rend()) return *test_suite;

  // No.  Let's create one.
  auto* const new_test_suite =
	  new TestSuite(test_suite_name, type_param, set_up_tc, tear_down_tc);

  // Is this a death test suite?
  if (internal::UnitTestOptions::MatchesFilter(test_suite_name,
											   constants::kDeathTestSuiteFilter)) {
	// Yes.  Inserts the test suite after the last death test suite
	// defined so far.  This only works when the test suites haven't
	// been shuffled.  Otherwise we may end up running a death test
	// after a non-death test.
	++last_death_test_suite_;
	test_suites_.insert(test_suites_.begin() + last_death_test_suite_,
						new_test_suite);
  } else {
	// No.  Appends to the end of the list.
	test_suites_.push_back(new_test_suite);
  }

  test_suite_indices_.push_back(static_cast<int>(test_suite_indices_.size()));
  return new_test_suite;
}

// Helpers for setting up / tearing down the given environment.  They
// are for use in the ForEach() function.
static void SetUpEnvironment(Environment* env) { env->SetUp(); }
static void TearDownEnvironment(Environment* env) { env->TearDown(); }

// Runs all tests in this UnitTest object, prints the result, and
// returns true if all tests are successful.  If any exception is
// thrown during a test, the test is considered to be failed, but the
// rest of the tests will still be run.
//
// When parameterized tests are enabled, it expands and registers
// parameterized tests first in RegisterParameterizedTests().
// All other functions called from RunAllTests() may safely assume that
// parameterized tests are ready to be counted and run.
bool UnitTestImpl::RunAllTests() {
  // True if and only if Google Test is initialized before RUN_ALL_TESTS() is
  // called.
  const bool gtest_is_initialized_before_run_all_tests = function_Is_initialized::GTestIsInitialized();

  // Do not run any test if the --help flag was specified.
  if (g_help_flag)
	return true;

  // Repeats the call to the post-flag parsing initialization in case the
  // user didn't call InitGoogleTest.
  PostFlagParsingInit();

  // Even if sharding is not on, test runners may want to use the
  // GTEST_SHARD_STATUS_FILE to query whether the test supports the sharding
  // protocol.
  function_Write_to_shard_status_file_if_needed::WriteToShardStatusFileIfNeeded();

  // True if and only if we are in a subprocess for running a thread-safe-style
  // death test.
  bool in_subprocess_for_death_test = false;

#if GTEST_HAS_DEATH_TEST
  in_subprocess_for_death_test =
	  (internal_run_death_test_flag_.get() != nullptr);
# if defined(GTEST_EXTRA_DEATH_TEST_CHILD_SETUP_)
  if (in_subprocess_for_death_test) {
	GTEST_EXTRA_DEATH_TEST_CHILD_SETUP_();
  }
# endif  // defined(GTEST_EXTRA_DEATH_TEST_CHILD_SETUP_)
#endif  // GTEST_HAS_DEATH_TEST
  /*/
  const bool should_shard = ShouldShard(kTestTotalShards, kTestShardIndex,
										in_subprocess_for_death_test);
  /*/
  const bool should_shard = false;
  //*/

  // Compares the full test names with the filter to decide which
  // tests to run.
  const bool has_tests_to_run = FilterTests(should_shard
											  ? HONOR_SHARDING_PROTOCOL
											  : IGNORE_SHARDING_PROTOCOL) > 0;

  // Lists the tests and exits if the --gtest_list_tests flag was specified.
  if (::testing::GTEST_FLAG(list_tests)) {
	// This must be called *after* FilterTests() has been called.
	ListTestsMatchingFilter();
	return true;
  }

  random_seed_ = ::testing::GTEST_FLAG(shuffle) ?
	  ::testing::internal::GetRandomSeedFromFlag( ::testing::GTEST_FLAG(random_seed)) : 0;

  // True if and only if at least one test has failed.
  bool failed = false;

  TestEventListener* repeater = listeners()->repeater();

  start_timestamp_ = ::testing::internal::GetTimeInMillis();
  repeater->OnTestProgramStart(*parent_);

  // How many times to repeat the tests?  We don't want to repeat them
  // when we are inside the subprocess of a death test.
  const int repeat = in_subprocess_for_death_test ? 1 : ::testing::GTEST_FLAG(repeat);
  // Repeats forever if the repeat count is negative.
  const bool gtest_repeat_forever = repeat < 0;
  for (int i = 0; gtest_repeat_forever || i != repeat; i++) {
	// We want to preserve failures generated by ad-hoc test
	// assertions executed before RUN_ALL_TESTS().
	ClearNonAdHocTestResult();

	const ::testing::internal::TimeInMillis start = ::testing::internal::GetTimeInMillis();

	// Shuffles test suites and tests if requested.
	if (has_tests_to_run && ::testing::GTEST_FLAG(shuffle)) {
	  random()->Reseed(static_cast<uint32_t>(random_seed_));
	  // This should be done before calling OnTestIterationStart(),
	  // such that a test event listener can see the actual test order
	  // in the event.
	  ShuffleTests();
	}

	// Tells the unit test event listeners that the tests are about to start.
	repeater->OnTestIterationStart(*parent_, i);

	// Runs each test suite if there is at least one test to run.
	if (has_tests_to_run) {
	  // Sets up all environments beforehand.
	  repeater->OnEnvironmentsSetUpStart(*parent_);
	  function_Stl_utilities::ForEach(environments_, SetUpEnvironment);
	  repeater->OnEnvironmentsSetUpEnd(*parent_);

	  // Runs the tests only if there was no fatal failure or skip triggered
	  // during global set-up.
	  if (Test::IsSkipped()) {
		// Emit diagnostics when global set-up calls skip, as it will not be
		// emitted by default.
		TestResult& test_result =
			*GetUnitTestImpl()->current_test_result();
		for (int j = 0; j < test_result.total_part_count(); ++j) {
		  const ::testing::TestPartResult& test_part_result =
			  test_result.GetTestPartResult(j);
		  if (test_part_result.type() == ::testing::TestPartResult::kSkip) {
			const std::string& result = test_part_result.message();
			printf("%s\n", result.c_str());
		  }
		}
		fflush(stdout);
	  } else if (!Test::HasFatalFailure()) {
		for (int test_index = 0; test_index < total_test_suite_count();
			 test_index++) {
		  GetMutableSuiteCase(test_index)->Run();
		}
	  }

	  // Tears down all environments in reverse order afterwards.
	  repeater->OnEnvironmentsTearDownStart(*parent_);
	  std::for_each(environments_.rbegin(), environments_.rend(),
					TearDownEnvironment);
	  repeater->OnEnvironmentsTearDownEnd(*parent_);
	}

	elapsed_time_ = ::testing::internal::GetTimeInMillis() - start;

	// Tells the unit test event listener that the tests have just finished.
	repeater->OnTestIterationEnd(*parent_, i);

	// Gets the result and clears it.
	if (!Passed()) {
	  failed = true;
	}

	// Restores the original test order after the iteration.  This
	// allows the user to quickly repro a failure that happens in the
	// N-th iteration without repeating the first (N - 1) iterations.
	// This is not enclosed in "if (GTEST_FLAG(shuffle)) { ... }", in
	// case the user somehow changes the value of the flag somewhere
	// (it's always safe to unshuffle the tests).
	UnshuffleTests();

	if ( ::testing::GTEST_FLAG(shuffle)) {
	  // Picks a new random seed for each iteration.
	  random_seed_ = ::testing::internal::GetNextRandomSeed(random_seed_);
	}
  }

  repeater->OnTestProgramEnd(*parent_);

  if (!gtest_is_initialized_before_run_all_tests) {
	Colored_print::ColoredPrintf(
		COLOR_RED,
		"\nIMPORTANT NOTICE - DO NOT IGNORE:\n"
		"This test program did NOT call " GTEST_INIT_GOOGLE_TEST_NAME_
		"() before calling RUN_ALL_TESTS(). This is INVALID. Soon " GTEST_NAME_
		" will start to enforce the valid usage. "
		"Please fix it ASAP, or IT WILL START TO FAIL.\n");  // NOLINT
#if GTEST_FOR_GOOGLE_
	Colored_print::ColoredPrintf(COLOR_RED,
				  "For more details, see http://wiki/Main/ValidGUnitMain.\n");
#endif  // GTEST_FOR_GOOGLE_
  }

  return !failed;
}

// Clears the results of all tests, except the ad hoc tests.
void UnitTestImpl::ClearNonAdHocTestResult() {
	function_Stl_utilities::ForEach(test_suites_, TestSuite::ClearTestSuiteResult);
}

// Clears the results of ad-hoc test assertions.
void UnitTestImpl::ClearAdHocTestResult() {
	ad_hoc_test_result_.Clear();
}

// Compares the name of each test with the user-specified filter to
// decide whether the test should be run, then records the result in
// each TestSuite and TestInfo object.
// If shard_tests == true, further filters tests based on sharding
// variables in the environment - see
// https://github.com/google/googletest/blob/master/googletest/docs/advanced.md
// . Returns the number of tests that should run.
int UnitTestImpl::FilterTests(ReactionToSharding shard_tests) {
  const int32_t total_shards = shard_tests == HONOR_SHARDING_PROTOCOL ? function_Int32_from_environment_or_die::Int32FromEnvOrDie( constants::kTestTotalShards, -1) : -1;
  const int32_t shard_index = shard_tests == HONOR_SHARDING_PROTOCOL ? function_Int32_from_environment_or_die::Int32FromEnvOrDie( constants::kTestShardIndex, -1) : -1;

  // num_runnable_tests are the number of tests that will
  // run across all shards (i.e., match filter and are not disabled).
  // num_selected_tests are the number of tests to be run on
  // this shard.
  int num_runnable_tests = 0;
  int num_selected_tests = 0;
  for (auto* test_suite : test_suites_) {
	const std::string& test_suite_name = test_suite->name();
	test_suite->set_should_run(false);

	for (size_t j = 0; j < test_suite->test_info_list().size(); j++) {
	  TestInfo* const test_info = test_suite->test_info_list()[j];
	  const std::string test_name(test_info->name());
	  // A test is disabled if test suite name or test name matches
	  // kDisableTestFilter.
	  const bool is_disabled = internal::UnitTestOptions::MatchesFilter(
								   test_suite_name, constants::kDisableTestFilter) ||
							   internal::UnitTestOptions::MatchesFilter(
								   test_name, constants::kDisableTestFilter);
	  test_info->is_disabled_ = is_disabled;

	  const bool matches_filter = internal::UnitTestOptions::FilterMatchesTest(
		  test_suite_name, test_name);
	  test_info->matches_filter_ = matches_filter;

	  const bool is_runnable =
		  ( ::testing::GTEST_FLAG(also_run_disabled_tests) || !is_disabled) &&
		  matches_filter;

	  const bool is_in_another_shard =
		  shard_tests != IGNORE_SHARDING_PROTOCOL &&
		  !function_Should_run_test_on_shard::ShouldRunTestOnShard(total_shards, shard_index, num_runnable_tests);
	  test_info->is_in_another_shard_ = is_in_another_shard;
	  const bool is_selected = is_runnable && !is_in_another_shard;

	  num_runnable_tests += is_runnable;
	  num_selected_tests += is_selected;

	  test_info->should_run_ = is_selected;
	  test_suite->set_should_run(test_suite->should_run() || is_selected);
	}
  }
  return num_selected_tests;
}

// Prints the given C-string on a single line by replacing all '\n'
// characters with string "\\n".  If the output takes more than
// max_length characters, only prints the first max_length characters
// and "...".
static void PrintOnOneLine(const char* str, int max_length) {
  if (str != nullptr) {
	for (int i = 0; *str != '\0'; ++str) {
	  if (i >= max_length) {
		printf("...");
		break;
	  }
	  if (*str == '\n') {
		printf("\\n");
		i += 2;
	  } else {
		printf("%c", *str);
		++i;
	  }
	}
  }
}

// Prints the names of the tests matching the user-specified filter flag.
void UnitTestImpl::ListTestsMatchingFilter() {
  // Print at most this many characters for each type/value parameter.
  const int kMaxParamLength = 250;

  for (auto* test_suite : test_suites_) {
	bool printed_test_suite_name = false;

	for (size_t j = 0; j < test_suite->test_info_list().size(); j++) {
	  const TestInfo* const test_info = test_suite->test_info_list()[j];
	  if (test_info->matches_filter_) {
		if (!printed_test_suite_name) {
		  printed_test_suite_name = true;
		  printf("%s.", test_suite->name());
		  if (test_suite->type_param() != nullptr) {
			printf("  # %s = ", constants::internal::kTypeParamLabel);
			// We print the type parameter on a single line to make
			// the output easy to parse by a program.
			PrintOnOneLine(test_suite->type_param(), kMaxParamLength);
		  }
		  printf("\n");
		}
		printf("  %s", test_info->name());
		if (test_info->value_param() != nullptr) {
		  printf("  # %s = ", constants::internal::kValueParamLabel);
		  // We print the value parameter on a single line to make the
		  // output easy to parse by a program.
		  PrintOnOneLine(test_info->value_param(), kMaxParamLength);
		}
		printf("\n");
	  }
	}
  }
  fflush(stdout);
  const std::string& output_format = UnitTestOptions::GetOutputFormat();
  if (output_format == "xml" || output_format == "json") {
	FILE* fileout = function_Open_file_for_writing::OpenFileForWriting(
		UnitTestOptions::GetAbsolutePathToOutputFile().c_str());
	std::stringstream stream;
	if (output_format == "xml") {
	  XmlUnitTestResultPrinter(
		  UnitTestOptions::GetAbsolutePathToOutputFile().c_str())
		  .PrintXmlTestsList(&stream, test_suites_);
	} else if (output_format == "json") {
	  JsonUnitTestResultPrinter(
		  UnitTestOptions::GetAbsolutePathToOutputFile().c_str())
		  .PrintJsonTestList(&stream, test_suites_);
	}
	fprintf(fileout, "%s", ::jmsd::cutf::internal::function_String_stream_to_string::StringStreamToString( stream ).c_str());
	fclose(fileout);
  }
}

const TestSuite *UnitTestImpl::current_test_suite() const { return current_test_suite_; }
TestInfo *UnitTestImpl::current_test_info() { return current_test_info_; }
const TestInfo *UnitTestImpl::current_test_info() const { return current_test_info_; }

// Returns the vector of environments that need to be set-up/torn-down before/after the tests are run.
::std::vector< Environment * > &UnitTestImpl::environments() {
	return environments_;
}

// Getters for the per-thread Google Test trace stack.
::std::vector< ::testing::internal::TraceInfo > &UnitTestImpl::gtest_trace_stack() {
	return *( gtest_trace_stack_.pointer() );
}

const ::std::vector< ::testing::internal::TraceInfo > &UnitTestImpl::gtest_trace_stack() const {
	return gtest_trace_stack_.get();
}

// Sets the OS stack trace getter.
//
// Does nothing if the input and the current OS stack trace getter are
// the same; otherwise, deletes the old getter and makes the input the
// current getter.
void UnitTestImpl::set_os_stack_trace_getter(
	::testing::internal::OsStackTraceGetterInterface* getter) {
  if (os_stack_trace_getter_ != getter) {
	delete os_stack_trace_getter_;
	os_stack_trace_getter_ = getter;
  }
}

// Returns the current OS stack trace getter if it is not NULL;
// otherwise, creates an OsStackTraceGetter, makes it the current
// getter, and returns it.
::testing::internal::OsStackTraceGetterInterface* UnitTestImpl::os_stack_trace_getter() {
  if (os_stack_trace_getter_ == nullptr) {
#ifdef GTEST_OS_STACK_TRACE_GETTER_
	os_stack_trace_getter_ = new GTEST_OS_STACK_TRACE_GETTER_;
#else
	os_stack_trace_getter_ = new ::testing::internal::OsStackTraceGetter;
#endif  // GTEST_OS_STACK_TRACE_GETTER_
  }

  return os_stack_trace_getter_;
}

// Returns the most specific TestResult currently running.
TestResult *UnitTestImpl::current_test_result() {
  if (current_test_info_ != nullptr) {
	return current_test_info_->result_.get();
  }
  if (current_test_suite_ != nullptr) {
	return current_test_suite_->ad_hoc_test_result_.get();
  }
  return &ad_hoc_test_result_;
}

// Returns the TestResult for the ad hoc test.
const TestResult *UnitTestImpl::ad_hoc_test_result() const {
	return &ad_hoc_test_result_;
}

// Shuffles all test suites, and the tests within each test suite,
// making sure that death tests are still run first.
void UnitTestImpl::ShuffleTests() {
  // Shuffles the death test suites.
  function_Stl_utilities::ShuffleRange(random(), 0, last_death_test_suite_ + 1, &test_suite_indices_);

  // Shuffles the non-death test suites.
  function_Stl_utilities::ShuffleRange(random(), last_death_test_suite_ + 1,
			   static_cast<int>(test_suites_.size()), &test_suite_indices_);

  // Shuffles the tests inside each test suite.
  for (auto& test_suite : test_suites_) {
	test_suite->ShuffleTests(random());
  }
}

// Restores the test suites and tests to their order before the first shuffle.
void UnitTestImpl::UnshuffleTests() {
	for ( size_t i = 0; i < test_suites_.size(); i++ ) {
		test_suites_[i]->UnshuffleTests(); // Unshuffles the tests in each test suite.
		test_suite_indices_[i] = static_cast<int>(i); // Resets the index of each test suite.
	}
}

// Returns the value of GTEST_FLAG(catch_exceptions) at the moment UnitTest::Run() starts.
bool UnitTestImpl::catch_exceptions() const {
	return catch_exceptions_;
}

void UnitTestImpl::set_catch_exceptions( bool const value) {
	catch_exceptions_ = value;
}


} // namespace internal
} // namespace cutf
} // namespace jmsd
