#include "Text_output_attributes.h"


namespace jmsd {
namespace cutf {


// The list of reserved attributes used in the <testsuites> element of XML output.
//char const *const kReservedTestSuitesAttributes[] = {
::std::vector< ::std::string > const kReservedTestSuitesAttributes = {
	"disabled",
	"errors",
	"failures",
	"name",
	"random_seed",
	"tests",
	"time",
	"timestamp"
};

// The list of reserved attributes used in the <testsuite> element of XML output.
//const char *const kReservedTestSuiteAttributes[] = {
::std::vector< ::std::string > const kReservedTestSuiteAttributes = {
	"disabled",
	"errors",
	"failures",
	"name",
	"tests",
	"time",
	"timestamp"
};

// The list of reserved attributes used in the <testcase> element of XML output.
//char const *const kReservedTestCaseAttributes[] = {
::std::vector< ::std::string > const kReservedTestCaseAttributes = {
	"classname",
	"name",
	"status",
	"time",
	"type_param",
	"value_param",
	"file",
	"line"
};

// Use a slightly different set for allowed output to ensure existing tests can still RecordProperty("result") or "RecordProperty(timestamp")
//const char *const kReservedOutputTestCaseAttributes[] = {
::std::vector< ::std::string > const kReservedOutputTestCaseAttributes = {
	"classname",
	"name",
	"status",
	"time",
	"type_param",
	"value_param",
	"file",
	"line",
	"result",
	"timestamp"
};


} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
