#pragma once


namespace jmsd {
namespace cutf {


class TestSuite;


// Old API is still available but deprecated
#ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_
using TestCase = TestSuite;
#endif



} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
