#include "Test_result.h"


namespace jmsd {
namespace cutf {


// class TestResult

// Creates an empty TestResult.
TestResult::TestResult()
	: death_test_count_(0), start_timestamp_(0), elapsed_time_(0) {}

// D'tor.
TestResult::~TestResult() {
}

// Returns the i-th test part result among all the results. i can
// range from 0 to total_part_count() - 1. If i is not in that range,
// aborts the program.
const TestPartResult& TestResult::GetTestPartResult(int i) const {
  if (i < 0 || i >= total_part_count())
	internal::posix::Abort();
  return test_part_results_.at(static_cast<size_t>(i));
}

// Returns the i-th test property. i can range from 0 to
// test_property_count() - 1. If i is not in that range, aborts the
// program.
const TestProperty& TestResult::GetTestProperty(int i) const {
  if (i < 0 || i >= test_property_count())
	internal::posix::Abort();
  return test_properties_.at(static_cast<size_t>(i));
}

// Clears the test part results.
void TestResult::ClearTestPartResults() {
  test_part_results_.clear();
}

// Adds a test part result to the list.
void TestResult::AddTestPartResult(const TestPartResult& test_part_result) {
  test_part_results_.push_back(test_part_result);
}

// Adds a test property to the list. If a property with the same key as the
// supplied property is already represented, the value of this test_property
// replaces the old value for that key.
void TestResult::RecordProperty(const std::string& xml_element,
								const TestProperty& test_property) {
  if (!ValidateTestProperty(xml_element, test_property)) {
	return;
  }
  internal::MutexLock lock(&test_properites_mutex_);
  const std::vector<TestProperty>::iterator property_with_matching_key =
	  std::find_if(test_properties_.begin(), test_properties_.end(),
				   internal::TestPropertyKeyIs(test_property.key()));
  if (property_with_matching_key == test_properties_.end()) {
	test_properties_.push_back(test_property);
	return;
  }
  property_with_matching_key->SetValue(test_property.value());
}

// The list of reserved attributes used in the <testsuites> element of XML
// output.
static const char* const kReservedTestSuitesAttributes[] = {
  "disabled",
  "errors",
  "failures",
  "name",
  "random_seed",
  "tests",
  "time",
  "timestamp"
};

// The list of reserved attributes used in the <testsuite> element of XML
// output.
static const char* const kReservedTestSuiteAttributes[] = {
	"disabled", "errors", "failures", "name", "tests", "time", "timestamp"};

// The list of reserved attributes used in the <testcase> element of XML output.
static const char* const kReservedTestCaseAttributes[] = {
	"classname",   "name", "status", "time",  "type_param",
	"value_param", "file", "line"};

// Use a slightly different set for allowed output to ensure existing tests can
// still RecordProperty("result") or "RecordProperty(timestamp")
static const char* const kReservedOutputTestCaseAttributes[] = {
	"classname",   "name", "status", "time",   "type_param",
	"value_param", "file", "line",   "result", "timestamp"};

template <int kSize>
std::vector<std::string> ArrayAsVector(const char* const (&array)[kSize]) {
  return std::vector<std::string>(array, array + kSize);
}

static std::vector<std::string> GetReservedAttributesForElement(
	const std::string& xml_element) {
  if (xml_element == "testsuites") {
	return ArrayAsVector(kReservedTestSuitesAttributes);
  } else if (xml_element == "testsuite") {
	return ArrayAsVector(kReservedTestSuiteAttributes);
  } else if (xml_element == "testcase") {
	return ArrayAsVector(kReservedTestCaseAttributes);
  } else {
	GTEST_CHECK_(false) << "Unrecognized xml_element provided: " << xml_element;
  }
  // This code is unreachable but some compilers may not realizes that.
  return std::vector<std::string>();
}

// TODO(jdesprez): Merge the two getReserved attributes once skip is improved
static std::vector<std::string> GetReservedOutputAttributesForElement(
	const std::string& xml_element) {
  if (xml_element == "testsuites") {
	return ArrayAsVector(kReservedTestSuitesAttributes);
  } else if (xml_element == "testsuite") {
	return ArrayAsVector(kReservedTestSuiteAttributes);
  } else if (xml_element == "testcase") {
	return ArrayAsVector(kReservedOutputTestCaseAttributes);
  } else {
	GTEST_CHECK_(false) << "Unrecognized xml_element provided: " << xml_element;
  }
  // This code is unreachable but some compilers may not realizes that.
  return std::vector<std::string>();
}

static std::string FormatWordList(const std::vector<std::string>& words) {
  Message word_list;
  for (size_t i = 0; i < words.size(); ++i) {
	if (i > 0 && words.size() > 2) {
	  word_list << ", ";
	}
	if (i == words.size() - 1) {
	  word_list << "and ";
	}
	word_list << "'" << words[i] << "'";
  }
  return word_list.GetString();
}

static bool ValidateTestPropertyName(
	const std::string& property_name,
	const std::vector<std::string>& reserved_names) {
  if (std::find(reserved_names.begin(), reserved_names.end(), property_name) !=
		  reserved_names.end()) {
	ADD_FAILURE() << "Reserved key used in RecordProperty(): " << property_name
				  << " (" << FormatWordList(reserved_names)
				  << " are reserved by " << GTEST_NAME_ << ")";
	return false;
  }
  return true;
}

// Adds a failure if the key is a reserved attribute of the element named
// xml_element.  Returns true if the property is valid.
bool TestResult::ValidateTestProperty(const std::string& xml_element,
									  const TestProperty& test_property) {
  return ValidateTestPropertyName(test_property.key(),
								  GetReservedAttributesForElement(xml_element));
}

// Clears the object.
void TestResult::Clear() {
  test_part_results_.clear();
  test_properties_.clear();
  death_test_count_ = 0;
  elapsed_time_ = 0;
}

// Returns true off the test part was skipped.
static bool TestPartSkipped(const TestPartResult& result) {
  return result.skipped();
}

// Returns true if and only if the test was skipped.
bool TestResult::Skipped() const {
  return !Failed() && CountIf(test_part_results_, TestPartSkipped) > 0;
}

// Returns true if and only if the test failed.
bool TestResult::Failed() const {
  for (int i = 0; i < total_part_count(); ++i) {
	if (GetTestPartResult(i).failed())
	  return true;
  }
  return false;
}

// Returns true if and only if the test part fatally failed.
static bool TestPartFatallyFailed(const TestPartResult& result) {
  return result.fatally_failed();
}

// Returns true if and only if the test fatally failed.
bool TestResult::HasFatalFailure() const {
  return CountIf(test_part_results_, TestPartFatallyFailed) > 0;
}

// Returns true if and only if the test part non-fatally failed.
static bool TestPartNonfatallyFailed(const TestPartResult& result) {
  return result.nonfatally_failed();
}

// Returns true if and only if the test has a non-fatal failure.
bool TestResult::HasNonfatalFailure() const {
  return CountIf(test_part_results_, TestPartNonfatallyFailed) > 0;
}

// Gets the number of all test parts.  This is the sum of the number
// of successful test parts and the number of failed test parts.
int TestResult::total_part_count() const {
  return static_cast<int>(test_part_results_.size());
}

// Returns the number of the test properties.
int TestResult::test_property_count() const {
  return static_cast<int>(test_properties_.size());
}


} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
