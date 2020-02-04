#pragma once

#include "Default_global_test_part_result_reporter.hxx"


#include "gtest/gtest-test-part.h"
//#include "Test_part_result_reporter_interface.h"

#include "Unit_test_impl.hxx"


namespace jmsd {
namespace cutf {
namespace internal {


// This is the default global test part result reporter used in UnitTestImpl.
// This class should only be used by UnitTestImpl.
class DefaultGlobalTestPartResultReporter :
	public ::testing::TestPartResultReporterInterface
{
public:
	explicit DefaultGlobalTestPartResultReporter(UnitTestImpl* unit_test);

	// Implements the TestPartResultReporterInterface. Reports the test part
	// result in the current test.
	void ReportTestPartResult(const ::testing::TestPartResult& result) override;

private:
	UnitTestImpl* const unit_test_;

	GTEST_DISALLOW_COPY_AND_ASSIGN_(DefaultGlobalTestPartResultReporter);
};


} // namespace internal
} // namespace cutf
} // namespace jmsd
