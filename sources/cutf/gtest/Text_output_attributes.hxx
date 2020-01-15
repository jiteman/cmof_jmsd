#pragma once


#include <string>
#include <vector>


namespace jmsd {
namespace cutf {


// The list of reserved attributes used in the <testsuites> element of XML output.
//extern char const *const kReservedTestSuitesAttributes[];
extern ::std::vector< ::std::string > const kReservedTestSuitesAttributes;

// The list of reserved attributes used in the <testsuite> element of XML output.
//extern const char *const kReservedTestSuiteAttributes[];
extern ::std::vector< ::std::string > const kReservedTestSuiteAttributes;

// The list of reserved attributes used in the <testcase> element of XML output.
//extern char const *const kReservedTestCaseAttributes[];
extern ::std::vector< ::std::string > const kReservedTestCaseAttributes;

// Use a slightly different set for allowed output to ensure existing tests can still RecordProperty("result") or "RecordProperty(timestamp")
//extern const char *const kReservedOutputTestCaseAttributes[];
extern ::std::vector< ::std::string > const kReservedOutputTestCaseAttributes;


} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
