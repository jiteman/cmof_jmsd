#include "Pretty_unit_test_result_printer.h"


namespace jmsd {
namespace cutf {


// Fired before each iteration of tests starts.
void PrettyUnitTestResultPrinter::OnTestIterationStart(
	const ::jmsd::cutf::UnitTest& unit_test, int iteration) {
  if (GTEST_FLAG(repeat) != 1)
	printf("\nRepeating all tests (iteration %d) . . .\n\n", iteration + 1);

  const char* const filter = GTEST_FLAG(filter).c_str();

  // Prints the filter if it's not *.  This reminds the user that some
  // tests may be skipped.
  if (!String::CStringEquals(filter, ::jmsd::cutf::constants::kUniversalFilter)) {
	ColoredPrintf(COLOR_YELLOW,
				  "Note: %s filter = %s\n", GTEST_NAME_, filter);
  }

  if (internal::ShouldShard(::jmsd::cutf::constants::kTestTotalShards, ::jmsd::cutf::constants::kTestShardIndex, false)) {
	const int32_t shard_index = Int32FromEnvOrDie(::jmsd::cutf::constants::kTestShardIndex, -1);
	ColoredPrintf(COLOR_YELLOW,
				  "Note: This is test shard %d of %s.\n",
				  static_cast<int>(shard_index) + 1,
				  internal::posix::GetEnv(::jmsd::cutf::constants::kTestTotalShards));
  }

  if (GTEST_FLAG(shuffle)) {
	ColoredPrintf(COLOR_YELLOW,
				  "Note: Randomizing tests' orders with a seed of %d .\n",
				  unit_test.random_seed());
  }

  ColoredPrintf(COLOR_GREEN,  "[==========] ");
  printf("Running %s from %s.\n",
		 FormatTestCount(unit_test.test_to_run_count()).c_str(),
		 FormatTestSuiteCount(unit_test.test_suite_to_run_count()).c_str());
  fflush(stdout);
}

void PrettyUnitTestResultPrinter::OnEnvironmentsSetUpStart(
	const ::jmsd::cutf::UnitTest& /*unit_test*/) {
  ColoredPrintf(COLOR_GREEN,  "[----------] ");
  printf("Global test environment set-up.\n");
  fflush(stdout);
}

#ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_
void PrettyUnitTestResultPrinter::OnTestCaseStart(const TestCase& test_case) {
  const std::string counts =
	  FormatCountableNoun(test_case.test_to_run_count(), "test", "tests");
  ColoredPrintf(COLOR_GREEN, "[----------] ");
  printf("%s from %s", counts.c_str(), test_case.name());
  if (test_case.type_param() == nullptr) {
	printf("\n");
  } else {
	printf(", where %s = %s\n", kTypeParamLabel, test_case.type_param());
  }
  fflush(stdout);
}
#else
void PrettyUnitTestResultPrinter::OnTestSuiteStart(
	const ::jmsd::cutf::TestSuite& test_suite) {
  const std::string counts =
	  FormatCountableNoun(test_suite.test_to_run_count(), "test", "tests");
  ColoredPrintf(COLOR_GREEN, "[----------] ");
  printf("%s from %s", counts.c_str(), test_suite.name());
  if (test_suite.type_param() == nullptr) {
	printf("\n");
  } else {
	printf(", where %s = %s\n", kTypeParamLabel, test_suite.type_param());
  }
  fflush(stdout);
}
#endif  // GTEST_REMOVE_LEGACY_TEST_CASEAPI_

void PrettyUnitTestResultPrinter::OnTestStart(const ::jmsd::cutf::TestInfo& test_info) {
  ColoredPrintf(COLOR_GREEN,  "[ RUN      ] ");
  PrintTestName(test_info.test_suite_name(), test_info.name());
  printf("\n");
  fflush(stdout);
}

// Called after an assertion failure.
void PrettyUnitTestResultPrinter::OnTestPartResult(
	const TestPartResult& result) {
  switch (result.type()) {
	// If the test part succeeded, we don't need to do anything.
	case TestPartResult::kSuccess:
	  return;
	default:
	  // Print failure message from the assertion
	  // (e.g. expected this and got that).
	  PrintTestPartResult(result);
	  fflush(stdout);
  }
}

void PrettyUnitTestResultPrinter::OnTestEnd(const ::jmsd::cutf::TestInfo& test_info) {
  if (test_info.result()->Passed()) {
	ColoredPrintf(COLOR_GREEN, "[       OK ] ");
  } else if (test_info.result()->Skipped()) {
	ColoredPrintf(COLOR_GREEN, "[  SKIPPED ] ");
  } else {
	ColoredPrintf(COLOR_RED, "[  FAILED  ] ");
  }
  PrintTestName(test_info.test_suite_name(), test_info.name());
  if (test_info.result()->Failed())
	PrintFullTestCommentIfPresent(test_info);

  if (GTEST_FLAG(print_time)) {
	printf(" (%s ms)\n", internal::StreamableToString(
		   test_info.result()->elapsed_time()).c_str());
  } else {
	printf("\n");
  }
  fflush(stdout);
}

#ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_
void PrettyUnitTestResultPrinter::OnTestCaseEnd(const TestCase& test_case) {
  if (!GTEST_FLAG(print_time)) return;

  const std::string counts =
	  FormatCountableNoun(test_case.test_to_run_count(), "test", "tests");
  ColoredPrintf(COLOR_GREEN, "[----------] ");
  printf("%s from %s (%s ms total)\n\n", counts.c_str(), test_case.name(),
		 internal::StreamableToString(test_case.elapsed_time()).c_str());
  fflush(stdout);
}
#else
void PrettyUnitTestResultPrinter::OnTestSuiteEnd(const ::jmsd::cutf::TestSuite& test_suite) {
  if (!GTEST_FLAG(print_time)) return;

  const std::string counts =
	  FormatCountableNoun(test_suite.test_to_run_count(), "test", "tests");
  ColoredPrintf(COLOR_GREEN, "[----------] ");
  printf("%s from %s (%s ms total)\n\n", counts.c_str(), test_suite.name(),
		 internal::StreamableToString(test_suite.elapsed_time()).c_str());
  fflush(stdout);
}
#endif  // GTEST_REMOVE_LEGACY_TEST_CASEAPI_

void PrettyUnitTestResultPrinter::OnEnvironmentsTearDownStart(
	const ::jmsd::cutf::UnitTest& /*unit_test*/) {
  ColoredPrintf(COLOR_GREEN,  "[----------] ");
  printf("Global test environment tear-down\n");
  fflush(stdout);
}

// Internal helper for printing the list of failed tests.
void PrettyUnitTestResultPrinter::PrintFailedTests(const ::jmsd::cutf::UnitTest& unit_test) {
  const int failed_test_count = unit_test.failed_test_count();
  ColoredPrintf(COLOR_RED,  "[  FAILED  ] ");
  printf("%s, listed below:\n", FormatTestCount(failed_test_count).c_str());

  for (int i = 0; i < unit_test.total_test_suite_count(); ++i) {
	const ::jmsd::cutf::TestSuite& test_suite = *unit_test.GetTestSuite(i);
	if (!test_suite.should_run() || (test_suite.failed_test_count() == 0)) {
	  continue;
	}
	for (int j = 0; j < test_suite.total_test_count(); ++j) {
	  const ::jmsd::cutf::TestInfo& test_info = *test_suite.GetTestInfo(j);
	  if (!test_info.should_run() || !test_info.result()->Failed()) {
		continue;
	  }
	  ColoredPrintf(COLOR_RED, "[  FAILED  ] ");
	  printf("%s.%s", test_suite.name(), test_info.name());
	  PrintFullTestCommentIfPresent(test_info);
	  printf("\n");
	}
  }
  printf("\n%2d FAILED %s\n", failed_test_count,
		 failed_test_count == 1 ? "TEST" : "TESTS");
}

// Internal helper for printing the list of test suite failures not covered by
// PrintFailedTests.
void PrettyUnitTestResultPrinter::PrintFailedTestSuites(
	const ::jmsd::cutf::UnitTest& unit_test) {
  int suite_failure_count = 0;
  for (int i = 0; i < unit_test.total_test_suite_count(); ++i) {
	const ::jmsd::cutf::TestSuite& test_suite = *unit_test.GetTestSuite(i);
	if (!test_suite.should_run()) {
	  continue;
	}
	if (test_suite.ad_hoc_test_result().Failed()) {
	  ColoredPrintf(COLOR_RED, "[  FAILED  ] ");
	  printf("%s: SetUpTestSuite or TearDownTestSuite\n", test_suite.name());
	  ++suite_failure_count;
	}
  }
  if (suite_failure_count > 0) {
	printf("\n%2d FAILED TEST %s\n", suite_failure_count,
		   suite_failure_count == 1 ? "SUITE" : "SUITES");
  }
}

// Internal helper for printing the list of skipped tests.
void PrettyUnitTestResultPrinter::PrintSkippedTests(const ::jmsd::cutf::UnitTest& unit_test) {
  const int skipped_test_count = unit_test.skipped_test_count();
  if (skipped_test_count == 0) {
	return;
  }

  for (int i = 0; i < unit_test.total_test_suite_count(); ++i) {
	const ::jmsd::cutf::TestSuite& test_suite = *unit_test.GetTestSuite(i);
	if (!test_suite.should_run() || (test_suite.skipped_test_count() == 0)) {
	  continue;
	}
	for (int j = 0; j < test_suite.total_test_count(); ++j) {
	  const ::jmsd::cutf::TestInfo& test_info = *test_suite.GetTestInfo(j);
	  if (!test_info.should_run() || !test_info.result()->Skipped()) {
		continue;
	  }
	  ColoredPrintf(COLOR_GREEN, "[  SKIPPED ] ");
	  printf("%s.%s", test_suite.name(), test_info.name());
	  printf("\n");
	}
  }
}

void PrettyUnitTestResultPrinter::OnTestIterationEnd(const ::jmsd::cutf::UnitTest& unit_test,
													 int /*iteration*/) {
  ColoredPrintf(COLOR_GREEN,  "[==========] ");
  printf("%s from %s ran.",
		 FormatTestCount(unit_test.test_to_run_count()).c_str(),
		 FormatTestSuiteCount(unit_test.test_suite_to_run_count()).c_str());
  if (GTEST_FLAG(print_time)) {
	printf(" (%s ms total)",
		   internal::StreamableToString(unit_test.elapsed_time()).c_str());
  }
  printf("\n");
  ColoredPrintf(COLOR_GREEN,  "[  PASSED  ] ");
  printf("%s.\n", FormatTestCount(unit_test.successful_test_count()).c_str());

  const int skipped_test_count = unit_test.skipped_test_count();
  if (skipped_test_count > 0) {
	ColoredPrintf(COLOR_GREEN, "[  SKIPPED ] ");
	printf("%s, listed below:\n", FormatTestCount(skipped_test_count).c_str());
	PrintSkippedTests(unit_test);
  }

  if (!unit_test.Passed()) {
	PrintFailedTests(unit_test);
	PrintFailedTestSuites(unit_test);
  }

  int num_disabled = unit_test.reportable_disabled_test_count();
  if (num_disabled && !GTEST_FLAG(also_run_disabled_tests)) {
	if (unit_test.Passed()) {
	  printf("\n");  // Add a spacer if no FAILURE banner is displayed.
	}
	ColoredPrintf(COLOR_YELLOW,
				  "  YOU HAVE %d DISABLED %s\n\n",
				  num_disabled,
				  num_disabled == 1 ? "TEST" : "TESTS");
  }
  // Ensure that Google Test output is printed before, e.g., heapchecker output.
  fflush(stdout);
}

// End PrettyUnitTestResultPrinter


} // namespace cutf
} // namespace jmsd
