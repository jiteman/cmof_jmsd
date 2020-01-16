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


// Since most methods are very similar, use macros to reduce boilerplate.
// This defines a member that forwards the call to all listeners.
#define GTEST_REPEATER_METHOD_(Name, Type) \
void TestEventRepeater::Name(const Type& parameter) { \
  if (forwarding_enabled_) { \
	for (size_t i = 0; i < listeners_.size(); i++) { \
	  listeners_[i]->Name(parameter); \
	} \
  } \
}
// This defines a member that forwards the call to all listeners in reverse
// order.
#define GTEST_REVERSE_REPEATER_METHOD_(Name, Type)      \
  void TestEventRepeater::Name(const Type& parameter) { \
	if (forwarding_enabled_) {                          \
	  for (size_t i = listeners_.size(); i != 0; i--) { \
		listeners_[i - 1]->Name(parameter);             \
	  }                                                 \
	}                                                   \
  }

GTEST_REPEATER_METHOD_(OnTestProgramStart, UnitTest)
GTEST_REPEATER_METHOD_(OnEnvironmentsSetUpStart, UnitTest)
//  Legacy API is deprecated but still available
#ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_
GTEST_REPEATER_METHOD_(OnTestCaseStart, TestSuite)
#endif  //  GTEST_REMOVE_LEGACY_TEST_CASEAPI_
GTEST_REPEATER_METHOD_(OnTestSuiteStart, TestSuite)
GTEST_REPEATER_METHOD_(OnTestStart, TestInfo)
GTEST_REPEATER_METHOD_(OnTestPartResult, ::testing::TestPartResult)
GTEST_REPEATER_METHOD_(OnEnvironmentsTearDownStart, UnitTest)
GTEST_REVERSE_REPEATER_METHOD_(OnEnvironmentsSetUpEnd, UnitTest)
GTEST_REVERSE_REPEATER_METHOD_(OnEnvironmentsTearDownEnd, UnitTest)
GTEST_REVERSE_REPEATER_METHOD_(OnTestEnd, TestInfo)
//  Legacy API is deprecated but still available
#ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_
GTEST_REVERSE_REPEATER_METHOD_(OnTestCaseEnd, TestSuite)
#endif  //  GTEST_REMOVE_LEGACY_TEST_CASEAPI_
GTEST_REVERSE_REPEATER_METHOD_(OnTestSuiteEnd, TestSuite)
GTEST_REVERSE_REPEATER_METHOD_(OnTestProgramEnd, UnitTest)

#undef GTEST_REPEATER_METHOD_
#undef GTEST_REVERSE_REPEATER_METHOD_

void TestEventRepeater::OnTestIterationStart(const UnitTest& unit_test,
											 int iteration) {
  if (forwarding_enabled_) {
	for (size_t i = 0; i < listeners_.size(); i++) {
	  listeners_[i]->OnTestIterationStart(unit_test, iteration);
	}
  }
}

void TestEventRepeater::OnTestIterationEnd(const UnitTest& unit_test,
										   int iteration) {
  if (forwarding_enabled_) {
	for (size_t i = listeners_.size(); i > 0; i--) {
	  listeners_[i - 1]->OnTestIterationEnd(unit_test, iteration);
	}
  }
}

// End TestEventRepeater


} // namespace internal
} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
