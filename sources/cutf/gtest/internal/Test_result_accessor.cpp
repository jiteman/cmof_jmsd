#include "Test_result_accessor.h"


#include "gtest/Test_result.h"
#include "gtest/Test_property.h"


namespace jmsd {
namespace cutf {
namespace internal {


// static
void TestResultAccessor::RecordProperty(
	TestResult* test_result,
	const std::string& xml_element,
	const ::jmsd::cutf::TestProperty& property)
{
	test_result->RecordProperty(xml_element, property);
}

// static
void TestResultAccessor::ClearTestPartResults(TestResult* test_result) {
	test_result->ClearTestPartResults();
}

// static
const std::vector<testing::TestPartResult>& TestResultAccessor::test_part_results(
	const TestResult& test_result)
{
	return test_result.test_part_results();
}


} // namespace internal
} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
