#pragma once

#include "Test_event_listener.hxx"


#include "gtest-test-part.h"

#include "Unit_test.hxx"
#include "Test_info.hxx"
#include "Test_suite.hxx"


#include "internal/gtest-port.h"


namespace jmsd {
namespace cutf {


// The interface for tracing execution of tests. The methods are organized in the order the corresponding events are fired.
class JMSD_DEPRECATED_GTEST_API_ TestEventListener {
public:
	virtual ~TestEventListener() = 0;

	// Fired before any test activity starts.
	virtual void OnTestProgramStart( UnitTest const &unit_test ) = 0;

	// Fired before each iteration of tests starts.  There may be more than
	// one iteration if GTEST_FLAG(repeat) is set. iteration is the iteration
	// index, starting from 0.
	virtual void OnTestIterationStart( UnitTest const &unit_test, int iteration ) = 0;

	// Fired before environment set-up for each iteration of tests starts.
	virtual void OnEnvironmentsSetUpStart( UnitTest const &unit_test ) = 0;

	// Fired after environment set-up for each iteration of tests ends.
	virtual void OnEnvironmentsSetUpEnd( UnitTest const &unit_test ) = 0;

	// Fired before the test suite starts.
	virtual void OnTestSuiteStart( TestSuite const &test_suite );

	//  Legacy API is deprecated but still available
#ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_
	virtual void OnTestCaseStart( TestCase const &test_case );
#endif  //  GTEST_REMOVE_LEGACY_TEST_CASEAPI_

	// Fired before the test starts.
	virtual void OnTestStart( TestInfo const &test_info ) = 0;

	// Fired after a failed assertion or a SUCCEED() invocation.
	// If you want to throw an exception from this function to skip to the next TEST, it must be AssertionException defined above, or inherited from it.
	virtual void OnTestPartResult( ::testing::TestPartResult const &test_part_result ) = 0;

	// Fired after the test ends.
	virtual void OnTestEnd( TestInfo const &test_info ) = 0;

	// Fired after the test suite ends.
	virtual void OnTestSuiteEnd( TestSuite const &test_suite );

	//  Legacy API is deprecated but still available
#ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_
	virtual void OnTestCaseEnd( TestCase const &test_case );
#endif  //  GTEST_REMOVE_LEGACY_TEST_CASEAPI_

	// Fired before environment tear-down for each iteration of tests starts.
	virtual void OnEnvironmentsTearDownStart( UnitTest const &unit_test ) = 0;

	// Fired after environment tear-down for each iteration of tests ends.
	virtual void OnEnvironmentsTearDownEnd( UnitTest const &unit_test ) = 0;

	// Fired after each iteration of tests finishes.
	virtual void OnTestIterationEnd( UnitTest const &unit_test, int iteration ) = 0;

	// Fired after all test activities have ended.
	virtual void OnTestProgramEnd( UnitTest const &unit_test ) = 0;
};


} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
