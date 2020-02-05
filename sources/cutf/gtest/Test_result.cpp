#include "Test_result.h"


#include "internal/function_Stl_utilities.hin"

#include "Text_output_utilities.hxx"


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
const ::testing::TestPartResult& TestResult::GetTestPartResult(int i) const {
  if (i < 0 || i >= total_part_count())
	::testing::internal::posix::Abort();
  return test_part_results_.at(static_cast<size_t>(i));
}

// Returns the i-th test property. i can range from 0 to
// test_property_count() - 1. If i is not in that range, aborts the
// program.
const TestProperty& TestResult::GetTestProperty(int i) const {
  if (i < 0 || i >= test_property_count())
	::testing::internal::posix::Abort();
  return test_properties_.at(static_cast<size_t>(i));
}

// Clears the test part results.
void TestResult::ClearTestPartResults() {
  test_part_results_.clear();
}

// Adds a test part result to the list.
void TestResult::AddTestPartResult(const ::testing::TestPartResult& test_part_result) {
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

  ::testing::internal::MutexLock lock(&test_properites_mutex_);
  const std::vector<TestProperty>::iterator property_with_matching_key =
	  std::find_if(test_properties_.begin(), test_properties_.end(),
				   ::testing::internal::TestPropertyKeyIs(test_property.key()));
  if (property_with_matching_key == test_properties_.end()) {
	test_properties_.push_back(test_property);
	return;
  }
  property_with_matching_key->SetValue(test_property.value());
}


// Adds a failure if the key is a reserved attribute of the element named
// xml_element.  Returns true if the property is valid.
bool TestResult::ValidateTestProperty(const std::string& xml_element,
									  const TestProperty& test_property) {
  return ValidateTestPropertyName(test_property.key(),
								  ::jmsd::cutf::GetReservedAttributesForElement(xml_element));
}

// Clears the object.
void TestResult::Clear() {
  test_part_results_.clear();
  test_properties_.clear();
  death_test_count_ = 0;
  elapsed_time_ = 0;
}

// Returns true off the test part was skipped.
static bool TestPartSkipped(const ::testing::TestPartResult& result) {
  return result.skipped();
}

// Returns true if and only if the test was skipped.
bool TestResult::Skipped() const {
  return !Failed() && internal::function_Stl_utilities::CountIf(test_part_results_, TestPartSkipped) > 0;
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
static bool TestPartFatallyFailed(const ::testing::TestPartResult& result) {
  return result.fatally_failed();
}

// Returns true if and only if the test fatally failed.
bool TestResult::HasFatalFailure() const {
  return internal::function_Stl_utilities::CountIf(test_part_results_, TestPartFatallyFailed) > 0;
}

// Returns true if and only if the test part non-fatally failed.
static bool TestPartNonfatallyFailed(const ::testing::TestPartResult& result) {
  return result.nonfatally_failed();
}

// Returns true if and only if the test has a non-fatal failure.
bool TestResult::HasNonfatalFailure() const {
  return internal::function_Stl_utilities::CountIf(test_part_results_, TestPartNonfatallyFailed) > 0;
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
