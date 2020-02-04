#include "Default_per_thread_test_part_result_reporter.h"


#include "Unit_test_impl.h"


namespace jmsd {
namespace cutf {
namespace internal {


DefaultPerThreadTestPartResultReporter::DefaultPerThreadTestPartResultReporter( UnitTestImpl *unit_test )
	:
		unit_test_( unit_test )
{}

void DefaultPerThreadTestPartResultReporter::ReportTestPartResult( ::testing::TestPartResult const &result ) {
	unit_test_->GetGlobalTestPartResultReporter()->ReportTestPartResult( result );
}



} // namespace internal
} // namespace cutf
} // namespace jmsd
