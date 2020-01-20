#pragma once

#include "Pretty_unit_test_result_printer.hxx"


#include "Test_event_listener.h"


#include <cstdio>


namespace jmsd {
namespace cutf {


// This class implements the TestEventListener interface.
//
// Class PrettyUnitTestResultPrinter is copyable.
class PrettyUnitTestResultPrinter : public TestEventListener {
public:
	PrettyUnitTestResultPrinter() {}
	static void PrintTestName(const char* test_suite, const char* test) {
		::printf("%s.%s", test_suite, test);
	}

	// The following methods override what's in the TestEventListener class.
	void OnTestProgramStart(const ::jmsd::cutf::UnitTest& /*unit_test*/) override {}
	void OnTestIterationStart(const ::jmsd::cutf::UnitTest& unit_test, int iteration) override;
	void OnEnvironmentsSetUpStart(const ::jmsd::cutf::UnitTest& unit_test) override;
	void OnEnvironmentsSetUpEnd(const ::jmsd::cutf::UnitTest& /*unit_test*/) override {}
	void OnTestSuiteStart(const ::jmsd::cutf::TestSuite& test_suite) override;
	void OnTestSuiteEnd(const ::jmsd::cutf::TestSuite& test_suite) override;

	#ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_
	void OnTestCaseStart(const ::jmsd::cutf::TestCase& test_case) override;
	void OnTestCaseEnd(const TestCase& test_case) override;
	#endif // #ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_

	void OnTestStart(const ::jmsd::cutf::TestInfo& test_info) override;

	void OnTestPartResult(const ::testing::TestPartResult& result) override;
	void OnTestEnd(const ::jmsd::cutf::TestInfo& test_info) override;

	void OnEnvironmentsTearDownStart(const ::jmsd::cutf::UnitTest& unit_test) override;
	void OnEnvironmentsTearDownEnd(const ::jmsd::cutf::UnitTest& /*unit_test*/) override {}
	void OnTestIterationEnd(const ::jmsd::cutf::UnitTest& unit_test, int iteration) override;
	void OnTestProgramEnd(const ::jmsd::cutf::UnitTest& /*unit_test*/) override {}

private:
	static void PrintFailedTests(const ::jmsd::cutf::UnitTest& unit_test);
	static void PrintFailedTestSuites(const ::jmsd::cutf::UnitTest& unit_test);
	static void PrintSkippedTests(const ::jmsd::cutf::UnitTest& unit_test);
};


} // namespace cutf
} // namespace jmsd

