#pragma once

#include "Test_result_accessor.hxx"


#include "gtest/gtest-test-part.h"

#include <string>
#include <vector>

#include "gtest/Test_result.hxx"
#include "gtest/Test_property.hxx"


#include "cutf.h"


namespace jmsd {
namespace cutf {
namespace internal {


// TestResult contains some private methods that should be hidden from
// Google Test user but are required for testing. This class allow our tests
// to access them.
//
// This class is supplied only for the purpose of testing Google Test's own
// constructs. Do not use it in user tests, either directly or indirectly.
class JMSD_CUTF_SHARED_INTERFACE TestResultAccessor {

public:
	static void RecordProperty( TestResult* test_result, ::std::string const &xml_element, TestProperty const &property );
	static void ClearTestPartResults( TestResult *test_result );
	static ::std::vector< testing::TestPartResult > const &test_part_results( TestResult const &test_result );

};


} // namespace internal
} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
