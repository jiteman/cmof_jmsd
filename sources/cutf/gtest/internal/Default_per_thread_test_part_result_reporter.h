#pragma once

#include "Default_per_thread_test_part_result_reporter.hxx"


#include "gtest/gtest-test-part.h"
//#include "Test_part_result_reporter_interface.h"

#include "Unit_test_impl.hxx"


namespace jmsd {
namespace cutf {
namespace internal {


// This is the default per thread test part result reporter used in UnitTestImpl. This class should only be used by UnitTestImpl.
class DefaultPerThreadTestPartResultReporter :
	public ::testing::TestPartResultReporterInterface
{
public:
	explicit DefaultPerThreadTestPartResultReporter( UnitTestImpl *unit_test );

	// Implements the TestPartResultReporterInterface. The implementation just delegates to the current global test part result reporter of *unit_test_.
	void ReportTestPartResult( ::testing::TestPartResult const &result ) override;

private:
	::jmsd::cutf::internal::UnitTestImpl *const unit_test_;

	GTEST_DISALLOW_COPY_AND_ASSIGN_( DefaultPerThreadTestPartResultReporter );
};


} // namespace internal
} // namespace cutf
} // namespace jmsd
