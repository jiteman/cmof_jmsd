#pragma once

#include "Configurable_event_listener.hxx"

#include "gmock/gmock.h"


namespace jmsd {
namespace tpa {
namespace gtm {
namespace modification {


class Configurable_event_listener :
	public ::testing::TestEventListener
{

private:
	::testing::TestEventListener* _event_listener;

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
	void OnTestProgramStart( ::testing::UnitTest const &unit_test ) override final;
	void OnTestIterationStart( ::testing::UnitTest const &unit_test, int iteration ) override final;
	void OnEnvironmentsSetUpStart( ::testing::UnitTest const &unit_test ) override final;
	void OnEnvironmentsSetUpEnd( ::testing::UnitTest const &unit_test ) override final;
	void OnTestCaseStart( ::testing::TestCase const &test_case ) override final;
	void OnTestStart( ::testing::TestInfo const &test_info ) override final;
	void OnTestPartResult( ::testing::TestPartResult const &result ) override final;
	void OnTestEnd( ::testing::TestInfo const &test_info ) override final;
	void OnTestCaseEnd( ::testing::TestCase const &test_case ) override final;
	void OnEnvironmentsTearDownStart( ::testing::UnitTest const &unit_test ) override final;
	void OnEnvironmentsTearDownEnd( ::testing::UnitTest const &unit_test ) override final;
	void OnTestIterationEnd( ::testing::UnitTest const &unit_test, int iteration ) override final;
	void OnTestProgramEnd( ::testing::UnitTest const &unit_test ) override final;
	//~virtuals TestEventListener

};


} // namespace modification
} // namespace gtm
} // namespace tpa
} // namespace jmsd
