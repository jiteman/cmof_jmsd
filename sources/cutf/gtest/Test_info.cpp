#include "Test_info.h"


namespace jmsd {
namespace cutf {


// class TestInfo

// Constructs a TestInfo object. It assumes ownership of the test factory
// object.
TestInfo::TestInfo(const std::string& a_test_suite_name,
				   const std::string& a_name, const char* a_type_param,
				   const char* a_value_param,
				   internal::CodeLocation a_code_location,
				   internal::TypeId fixture_class_id,
				   internal::TestFactoryBase* factory)
	: test_suite_name_(a_test_suite_name),
	  name_(a_name),
	  type_param_(a_type_param ? new std::string(a_type_param) : nullptr),
	  value_param_(a_value_param ? new std::string(a_value_param) : nullptr),
	  location_(a_code_location),
	  fixture_class_id_(fixture_class_id),
	  should_run_(false),
	  is_disabled_(false),
	  matches_filter_(false),
	  factory_(factory),
	  result_() {}

// Destructs a TestInfo object.
TestInfo::~TestInfo() { delete factory_; }

namespace internal {

// Creates a new TestInfo object and registers it with Google Test;
// returns the created object.
//
// Arguments:
//
//   test_suite_name:   name of the test suite
//   name:             name of the test
//   type_param:       the name of the test's type parameter, or NULL if
//                     this is not a typed or a type-parameterized test.
//   value_param:      text representation of the test's value parameter,
//                     or NULL if this is not a value-parameterized test.
//   code_location:    code location where the test is defined
//   fixture_class_id: ID of the test fixture class
//   set_up_tc:        pointer to the function that sets up the test suite
//   tear_down_tc:     pointer to the function that tears down the test suite
//   factory:          pointer to the factory that creates a test object.
//                     The newly created TestInfo instance will assume
//                     ownership of the factory object.
TestInfo* MakeAndRegisterTestInfo(
	const char* test_suite_name, const char* name, const char* type_param,
	const char* value_param, CodeLocation code_location,
	TypeId fixture_class_id, SetUpTestSuiteFunc set_up_tc,
	TearDownTestSuiteFunc tear_down_tc, TestFactoryBase* factory) {
  TestInfo* const test_info =
	  new TestInfo(test_suite_name, name, type_param, value_param,
				   code_location, fixture_class_id, factory);
  GetUnitTestImpl()->AddTestInfo(set_up_tc, tear_down_tc, test_info);
  return test_info;
}

void ReportInvalidTestSuiteType(const char* test_suite_name,
								CodeLocation code_location) {
  Message errors;
  errors
	  << "Attempted redefinition of test suite " << test_suite_name << ".\n"
	  << "All tests in the same test suite must use the same test fixture\n"
	  << "class.  However, in test suite " << test_suite_name << ", you tried\n"
	  << "to define a test using a fixture class different from the one\n"
	  << "used earlier. This can happen if the two fixture classes are\n"
	  << "from different namespaces and have the same name. You should\n"
	  << "probably rename one of the classes to put the tests into different\n"
	  << "test suites.";

  GTEST_LOG_(ERROR) << FormatFileLocation(code_location.file.c_str(),
										  code_location.line)
					<< " " << errors.GetString();
}
}  // namespace internal

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

  const TimeInMillis start = internal::GetTimeInMillis();

  impl->os_stack_trace_getter()->UponLeavingGTest();

  // Creates the test object.
  Test* const test = internal::HandleExceptionsInMethodIfSupported(
	  factory_, &internal::TestFactoryBase::CreateTest,
	  "the test fixture's constructor");

  // Runs the test if the constructor didn't generate a fatal failure or invoke
  // GTEST_SKIP().
  // Note that the object will not be null
  if (!Test::HasFatalFailure() && !Test::IsSkipped()) {
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

  result_.set_start_timestamp(start);
  result_.set_elapsed_time(internal::GetTimeInMillis() - start);

  // Notifies the unit test event listener that a test has just finished.
  repeater->OnTestEnd(*this);

  // Tells UnitTest to stop associating assertion results to this
  // test.
  impl->set_current_test_info(nullptr);
}


} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
