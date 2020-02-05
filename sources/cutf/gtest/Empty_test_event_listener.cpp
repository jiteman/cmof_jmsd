#include "Empty_test_event_listener.h"


namespace jmsd {
namespace cutf {


void EmptyTestEventListener::OnTestProgramStart(const ::jmsd::cutf::UnitTest& /*unit_test*/)
{}

void EmptyTestEventListener::OnTestIterationStart(const ::jmsd::cutf::UnitTest& /*unit_test*/, int /*iteration*/)
{}

void EmptyTestEventListener::OnEnvironmentsSetUpStart(const ::jmsd::cutf::UnitTest& /*unit_test*/)
{}

void EmptyTestEventListener::OnEnvironmentsSetUpEnd(const::jmsd::cutf::UnitTest& /*unit_test*/)
{}

void EmptyTestEventListener::OnTestSuiteStart(const ::jmsd::cutf::TestSuite& /*test_suite*/)
{}

//  Legacy API is deprecated but still available
#ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_
void EmptyTestEventListener::OnTestCaseStart(const TestCase& /*test_case*/)
{}

void EmptyTestEventListener::OnTestCaseEnd(const TestCase& /*test_case*/)
{}
#endif  //  GTEST_REMOVE_LEGACY_TEST_CASEAPI_

void EmptyTestEventListener::OnTestStart(const ::jmsd::cutf::TestInfo& /*test_info*/)
{}

void EmptyTestEventListener::OnTestPartResult(const ::testing::TestPartResult& /*test_part_result*/)
{}

void EmptyTestEventListener::OnTestEnd(const ::jmsd::cutf::TestInfo& /*test_info*/)
{}

void EmptyTestEventListener::OnTestSuiteEnd(const ::jmsd::cutf::TestSuite& /*test_suite*/)
{}

void EmptyTestEventListener::OnEnvironmentsTearDownStart(const ::jmsd::cutf::UnitTest& /*unit_test*/)
{}

void EmptyTestEventListener::OnEnvironmentsTearDownEnd(const ::jmsd::cutf::UnitTest& /*unit_test*/)
{}

void EmptyTestEventListener::OnTestIterationEnd(const ::jmsd::cutf::UnitTest& /*unit_test*/, int /*iteration*/)
{}

void EmptyTestEventListener::OnTestProgramEnd(const ::jmsd::cutf::UnitTest& /*unit_test*/)
{}


} // namespace cutf
} // namespace jmsd
