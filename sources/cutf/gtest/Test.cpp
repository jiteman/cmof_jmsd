#include "Test.h"


#include "Unit_test.h"

#include "internal/Unit_test_impl.h"

#include "Message.hin"

#include "internal/Exception_handling.hin"

#include "internal/gtest-internal.h"


namespace jmsd {
namespace cutf {


// class Test

// Creates a Test object.

// The c'tor saves the states of all flags.
Test::Test()
	: gtest_flag_saver_( new GTEST_FLAG_SAVER_ ) {
}

// The d'tor restores the states of all flags.  The actual work is
// done by the d'tor of the gtest_flag_saver_ field, and thus not
// visible here.
Test::~Test()
{}

// static
void Test::SetUpTestSuite()
{}

// static
void Test::TearDownTestSuite()
{}


#ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_
// Legacy API is deprecated but still available
// static
void Test::TearDownTestCase()
{}

// static
void Test::SetUpTestCase()
{}
#endif // #ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_

// Sets up the test fixture.
//
// A sub-class may override this.
void Test::SetUp()
{}

// Tears down the test fixture.
//
// A sub-class may override this.
void Test::TearDown()
{}

// Allows user supplied key value pairs to be recorded for later output.
void Test::RecordProperty(const std::string& key, const std::string& value) {
	UnitTest::GetInstance()->RecordProperty(key, value);
}

// Allows user supplied key value pairs to be recorded for later output.
void Test::RecordProperty(const std::string& key, int value) {
  Message value_message;
  value_message << value;
  RecordProperty(key, value_message.GetString().c_str());
}

// Google Test requires all tests in the same test suite to use the same test
// fixture class.  This function checks if the current test has the
// same fixture class as the first test in the current test suite.  If
// yes, it returns true; otherwise it generates a Google Test failure and
// returns false.
bool Test::HasSameFixtureClass() {
  internal::UnitTestImpl* const impl = internal::GetUnitTestImpl();
  const TestSuite* const test_suite = impl->current_test_suite();

  // Info about the first test in the current test suite.
  const TestInfo* const first_test_info = test_suite->test_info_list()[0];
  const ::testing::internal::TypeId first_fixture_id = first_test_info->fixture_class_id_;
  const char* const first_test_name = first_test_info->name();

  // Info about the current test.
  const TestInfo* const this_test_info = impl->current_test_info();
  const ::testing::internal::TypeId this_fixture_id = this_test_info->fixture_class_id_;
  const char* const this_test_name = this_test_info->name();

  if (this_fixture_id != first_fixture_id) {
	// Is the first test defined using TEST?
	const bool first_is_TEST = first_fixture_id == ::testing::internal::GetTestTypeId();
	// Is this test defined using TEST?
	const bool this_is_TEST = this_fixture_id == ::testing::internal::GetTestTypeId();

	if (first_is_TEST || this_is_TEST) {
	  // Both TEST and TEST_F appear in same test suite, which is incorrect.
	  // Tell the user how to fix this.

	  // Gets the name of the TEST and the name of the TEST_F.  Note
	  // that first_is_TEST and this_is_TEST cannot both be true, as
	  // the fixture IDs are different for the two tests.
	  const char* const TEST_name =
		  first_is_TEST ? first_test_name : this_test_name;
	  const char* const TEST_F_name =
		  first_is_TEST ? this_test_name : first_test_name;

	  ADD_FAILURE()
		  << "All tests in the same test suite must use the same test fixture\n"
		  << "class, so mixing TEST_F and TEST in the same test suite is\n"
		  << "illegal.  In test suite " << this_test_info->test_suite_name()
		  << ",\n"
		  << "test " << TEST_F_name << " is defined using TEST_F but\n"
		  << "test " << TEST_name << " is defined using TEST.  You probably\n"
		  << "want to change the TEST to TEST_F or move it to another test\n"
		  << "case.";
	} else {
	  // Two fixture classes with the same name appear in two different
	  // namespaces, which is not allowed. Tell the user how to fix this.
	  ADD_FAILURE()
		  << "All tests in the same test suite must use the same test fixture\n"
		  << "class.  However, in test suite "
		  << this_test_info->test_suite_name() << ",\n"
		  << "you defined test " << first_test_name << " and test "
		  << this_test_name << "\n"
		  << "using two different test fixture classes.  This can happen if\n"
		  << "the two classes are from different namespaces or translation\n"
		  << "units and have the same name.  You should probably rename one\n"
		  << "of the classes to put the tests into different test suites.";
	}
	return false;
  }

  return true;
}

// Runs the test and updates the test result.
void Test::Run() {
	if (!HasSameFixtureClass()) return;

	internal::UnitTestImpl* const impl = internal::GetUnitTestImpl();
	impl->os_stack_trace_getter()->UponLeavingGTest();
	internal::HandleExceptionsInMethodIfSupported(this, &Test::SetUp, "SetUp()");

	// We will run the test only if SetUp() was successful and didn't call GTEST_SKIP().
	if (!HasFatalFailure() && !IsSkipped()) {
		impl->os_stack_trace_getter()->UponLeavingGTest();
		internal::HandleExceptionsInMethodIfSupported(
		this, &Test::TestBody, "the test body");
	}

	// However, we want to clean up as much as possible.
	// Hence we will always call TearDown(), even if SetUp() or the test body has failed.
	impl->os_stack_trace_getter()->UponLeavingGTest();
	internal::HandleExceptionsInMethodIfSupported( this, &Test::TearDown, "TearDown()");
}

void Test::DeleteSelf_() {
	delete this;
}

// Returns true if and only if the current test has a fatal failure.
// static
bool Test::HasFatalFailure() {
	return internal::GetUnitTestImpl()->current_test_result()->HasFatalFailure();
}

// Returns true if and only if the current test has a non-fatal failure.
// static
bool Test::HasNonfatalFailure() {
	return internal::GetUnitTestImpl()->current_test_result()->HasNonfatalFailure();
}

// Returns true if and only if the current test was skipped.
// static
bool Test::IsSkipped() {
	return internal::GetUnitTestImpl()->current_test_result()->Skipped();
}

// Returns true if and only if the current test has a (either fatal or non-fatal) failure.
// static
bool Test::HasFailure() {
	return THIS_STATIC::HasFatalFailure() || THIS_STATIC::HasNonfatalFailure();
}

struct Test::Setup_should_be_spelled_SetUp
{};

Test::Setup_should_be_spelled_SetUp *Test::Setup() {
	return nullptr;
}


} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
