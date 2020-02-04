#include "Default_global_test_part_result_reporter.h"


#include "Unit_test_impl.h"
#include "gtest/Test_event_listener.h"


namespace jmsd {
namespace cutf {
namespace internal {


DefaultGlobalTestPartResultReporter::DefaultGlobalTestPartResultReporter( UnitTestImpl* unit_test)
	:
		unit_test_(unit_test)
{}

void DefaultGlobalTestPartResultReporter::ReportTestPartResult( ::testing::TestPartResult const &result ) {
	unit_test_->current_test_result()->AddTestPartResult(result);
	unit_test_->listeners()->repeater()->OnTestPartResult(result);
}


} // namespace internal
} // namespace cutf
} // namespace jmsd
