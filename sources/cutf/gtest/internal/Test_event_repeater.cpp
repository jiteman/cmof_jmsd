#include "Test_event_repeater.h"


#include "function_Stl_utilities.hin"
#include "function_Delete.hin"


namespace jmsd {
namespace cutf {
namespace internal {


TestEventRepeater::~TestEventRepeater() {
	function_Stl_utilities::ForEach(listeners_, function_Delete::Delete< TestEventListener >);
}

void TestEventRepeater::Append(TestEventListener *listener) {
  listeners_.push_back(listener);
}

TestEventListener* TestEventRepeater::Release(TestEventListener *listener) {
  for (size_t i = 0; i < listeners_.size(); ++i) {
	if (listeners_[i] == listener) {
	  listeners_.erase(listeners_.begin() + static_cast<int>(i));
	  return listener;
	}
  }

  return nullptr;
}

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


} // namespace internal
} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
