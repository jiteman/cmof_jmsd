#pragma once

#include "Test.hxx"


#include "internal/gtest-port.h"


#include "Test_info.hxx"


namespace jmsd {
namespace cutf {


// The abstract class that all tests inherit from.
//
// In Google Test, a unit test program contains one or many TestSuites, and
// each TestSuite contains one or many Tests.
//
// When you define a test using the TEST macro, you don't need to
// explicitly derive from Test - the TEST macro automatically does
// this for you.
//
// The only time you derive from Test is when defining a test fixture
// to be used in a TEST_F.  For example:
//
//   class FooTest : public testing::Test {
//    protected:
//     void SetUp() override { ... }
//     void TearDown() override { ... }
//     ...
//   };
//
//   TEST_F(FooTest, Bar) { ... }
//   TEST_F(FooTest, Baz) { ... }
//
// Test is not copyable.
class JMSD_DEPRECATED_GTEST_API_ Test {

public:
	typedef Test THIS_STATIC;

	friend TestInfo;

	// The d'tor is virtual as we intend to inherit from Test.
	virtual ~Test() = 0;

	// Sets up the stuff shared by all tests in this test case.
	//
	// Google Test will call Foo::SetUpTestSuite() before running the first
	// test in test case Foo.  Hence a sub-class can define its own
	// SetUpTestSuite() method to shadow the one defined in the super
	// class.
	static void SetUpTestSuite();

	// Tears down the stuff shared by all tests in this test suite.
	//
	// Google Test will call Foo::TearDownTestSuite() after running the last
	// test in test case Foo.  Hence a sub-class can define its own
	// TearDownTestSuite() method to shadow the one defined in the super
	// class.
	static void TearDownTestSuite();

	// Legacy API is deprecated but still available
#ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_
	static void TearDownTestCase() {}
	static void SetUpTestCase() {}
#endif  // GTEST_REMOVE_LEGACY_TEST_CASEAPI_

	// Returns true if and only if the current test has a fatal failure.
	static bool HasFatalFailure();

	// Returns true if and only if the current test has a non-fatal failure.
	static bool HasNonfatalFailure();

	// Returns true if and only if the current test was skipped.
	static bool IsSkipped();

	// Returns true if and only if the current test has a (either fatal or non-fatal) failure.
	static bool HasFailure();

	// Logs a property for the current test, test suite, or for the entire
	// invocation of the test program when used outside of the context of a
	// test suite.  Only the last value for a given key is remembered.  These
	// are public static so they can be called from utility functions that are
	// not members of the test fixture.  Calls to RecordProperty made during
	// lifespan of the test (from the moment its constructor starts to the
	// moment its destructor finishes) will be output in XML as attributes of
	// the <testcase> element.  Properties recorded from fixture's
	// SetUpTestSuite or TearDownTestSuite are logged as attributes of the
	// corresponding <testsuite> element.  Calls to RecordProperty made in the
	// global context (before or after invocation of RUN_ALL_TESTS and from
	// SetUp/TearDown method of Environment objects registered with Google
	// Test) will be output as attributes of the <testsuites> element.
	static void RecordProperty(const std::string& key, const std::string& value);
	static void RecordProperty(const std::string& key, int value);

protected:
	// Creates a Test object.
	Test();

	// Sets up the test fixture.
	virtual void SetUp();

	// Tears down the test fixture.
	virtual void TearDown();

	private:
	// Returns true if and only if the current test has the same fixture class
	// as the first test in the current test suite.
	static bool HasSameFixtureClass();

	// Runs the test after the test fixture has been set up.
	//
	// A sub-class must implement this to define the test logic.
	//
	// DO NOT OVERRIDE THIS FUNCTION DIRECTLY IN A USER PROGRAM.
	// Instead, use the TEST or TEST_F macro.
	virtual void TestBody() = 0;

	// Sets up, executes, and tears down the test.
	void Run();

	// Deletes self.  We deliberately pick an unusual name for this
	// internal method to avoid clashing with names used in user TESTs.
	void DeleteSelf_();

	const std::unique_ptr< GTEST_FLAG_SAVER_ > gtest_flag_saver_;

	// Often a user misspells SetUp() as Setup() and spends a long time
	// wondering why it is never called by Google Test.  The declaration of
	// the following method is solely for catching such an error at
	// compile time:
	//
	//   - The return type is deliberately chosen to be not void, so it
	//   will be a conflict if void Setup() is declared in the user's
	//   test fixture.
	//
	//   - This method is private, so it will be another compiler error
	//   if the method is called from the user's test fixture.
	//
	// DO NOT OVERRIDE THIS FUNCTION.
	//
	// If you see an error about overriding the following function or
	// about it being private, you have mis-spelled SetUp() as Setup().
	struct Setup_should_be_spelled_SetUp;

	virtual Setup_should_be_spelled_SetUp *Setup() final;

	// We disallow copying Tests.
	GTEST_DISALLOW_COPY_AND_ASSIGN_( Test );
};


} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
