#pragma once

#include "Empty_test_event_listener.hxx"


#include "Test_event_listener.h"


#include "cutf.h"


namespace jmsd {
namespace cutf {


// The convenience class for users who need to override just one or two
// methods and are not concerned that a possible change to a signature of
// the methods they override will not be caught during the build.  For
// comments about each method please see the definition of TestEventListener
// above.
class JMSD_CUTF_SHARED_INTERFACE EmptyTestEventListener :
	public TestEventListener
{
public:
	void OnTestProgramStart( UnitTest const &unit_test ) override;
	void OnTestIterationStart( UnitTest const &unit_test , int /*iteration*/) override;
	void OnEnvironmentsSetUpStart( UnitTest const &unit_test ) override;
	void OnEnvironmentsSetUpEnd( UnitTest const &unit_test ) override;
	void OnTestSuiteStart( TestSuite const &test_suite ) override;

	//  Legacy API is deprecated but still available
#ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_
	void OnTestCaseStart(const TestCase& /*test_case*/) override;
	void OnTestCaseEnd(const TestCase& /*test_case*/) override;
#endif // #ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_

	void OnTestStart( TestInfo const &test_info ) override;
	void OnTestPartResult(const ::testing::TestPartResult& /*test_part_result*/) override;
	void OnTestEnd( TestInfo const &test_info ) override;
	void OnTestSuiteEnd( TestSuite const &test_suite ) override;

	void OnEnvironmentsTearDownStart( UnitTest const &unit_test ) override;
	void OnEnvironmentsTearDownEnd( UnitTest const &unit_test ) override;
	void OnTestIterationEnd( UnitTest const &unit_test , int /*iteration*/) override;
	void OnTestProgramEnd( UnitTest const &unit_test ) override;

};


} // namespace cutf
} // namespace jmsd
