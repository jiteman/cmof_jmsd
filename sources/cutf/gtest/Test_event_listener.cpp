#include "Test_event_listener.h"


namespace jmsd {
namespace cutf {


TestEventListener::~TestEventListener()
{}


void TestEventListener::OnTestSuiteStart( TestSuite const & /*test_suite*/ )
{}


void TestEventListener::OnTestSuiteEnd( TestSuite const & /*test_suite*/ )
{}


#ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_

void TestEventListener::OnTestCaseStart( TestCase const & /*test_case*/ )
{}


void TestEventListener::OnTestCaseEnd( TestCase const & /*test_case*/ )
{}

#endif // #ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_


} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
