#pragma once

#include "Test_event_repeater.hxx"


#include "gtest/Test_event_listener.h"


namespace jmsd {
namespace cutf {
namespace internal {


// This class forwards events to other event listeners.
class TestEventRepeater :
	public TestEventListener
{
public:
	TestEventRepeater() : forwarding_enabled_(true) {}
	~TestEventRepeater() override;
	void Append(TestEventListener *listener);
	TestEventListener* Release(TestEventListener* listener);

	// Controls whether events will be forwarded to listeners_. Set to false
	// in death test child processes.
	bool forwarding_enabled() const { return forwarding_enabled_; }
	void set_forwarding_enabled(bool enable) { forwarding_enabled_ = enable; }

	void OnTestProgramStart(const UnitTest& unit_test) override;
	void OnTestIterationStart(const UnitTest& unit_test, int iteration) override;
	void OnEnvironmentsSetUpStart(const UnitTest& unit_test) override;
	void OnEnvironmentsSetUpEnd(const UnitTest& unit_test) override;

	//  Legacy API is deprecated but still available
#ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_
	void OnTestCaseStart(const TestSuite& parameter) override;
	void OnTestCaseEnd(const TestCase& parameter) override;
#endif // #ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_

	void OnTestSuiteStart(const TestSuite& parameter) override;
	void OnTestStart(const TestInfo& test_info) override;
	void OnTestPartResult(const ::testing::TestPartResult& result) override;
	void OnTestEnd(const TestInfo& test_info) override;

	void OnTestSuiteEnd(const TestSuite& parameter) override;
	void OnEnvironmentsTearDownStart(const UnitTest& unit_test) override;
	void OnEnvironmentsTearDownEnd(const UnitTest& unit_test) override;
	void OnTestIterationEnd(const UnitTest& unit_test, int iteration) override;
	void OnTestProgramEnd(const UnitTest& unit_test) override;

private:
	// Controls whether events will be forwarded to listeners_. Set to false in death test child processes.
	bool forwarding_enabled_;

	// The list of listeners that receive events.
	std::vector<TestEventListener*> listeners_;

	GTEST_DISALLOW_COPY_AND_ASSIGN_(TestEventRepeater);
};


} // namespace internal
} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
