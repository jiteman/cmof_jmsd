#pragma once

#include "Configurable_event_listener.hxx"


#include "gtest/Test_event_listener.h"



namespace jmsd {
namespace ctf {
namespace modification {


class Configurable_event_listener :
	public cutf::TestEventListener
{

private:
	cutf::TestEventListener* _event_listener;

private:
	bool _showTestCases; // Show the names of each test case.
	bool _showTestNames; // Show the names of each test.
	bool _showSuccesses; // Show each success.
	bool _showInlineFailures; // Show each failure as it occurs. You will also see it at the bottom after the full suite is run.
	bool _showTestPartFailures; // Show test part failures (FAIL() reaction)
	bool _showEnvironment; // Show the setup of the global environment.

public:
	explicit Configurable_event_listener( TestEventListener *an_event_listener );

	// virtual TestEventListener
	~Configurable_event_listener() override final;

	// virtuals TestEventListener
	void OnTestProgramStart( ::jmsd::cutf::UnitTest const &unit_test ) override final;
	void OnTestIterationStart( ::jmsd::cutf::UnitTest const &unit_test, int iteration ) override final;
	void OnEnvironmentsSetUpStart( ::jmsd::cutf::UnitTest const &unit_test ) override final;
	void OnEnvironmentsSetUpEnd( ::jmsd::cutf::UnitTest const &unit_test ) override final;

#ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI
	void OnTestCaseStart( ::testing::TestCase const &test_case ) override final;
	void OnTestCaseEnd( ::testing::TestCase const &test_case ) override final;
#endif // #ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI

	void OnTestStart( ::jmsd::cutf::TestInfo const &test_info ) override final;
	void OnTestPartResult( ::testing::TestPartResult const &result ) override final;
	void OnTestEnd( ::jmsd::cutf::TestInfo const &test_info ) override final;
		void OnEnvironmentsTearDownStart( ::jmsd::cutf::UnitTest const &unit_test ) override final;
	void OnEnvironmentsTearDownEnd( ::jmsd::cutf::UnitTest const &unit_test ) override final;
	void OnTestIterationEnd( ::jmsd::cutf::UnitTest const &unit_test, int iteration ) override final;
	void OnTestProgramEnd( ::jmsd::cutf::UnitTest const &unit_test ) override final;
	//~virtuals TestEventListener

};


} // namespace modification
} // namespace ctf
} // namespace jmsd
