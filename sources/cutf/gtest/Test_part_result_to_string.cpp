#include "Test_part_result_to_string.h"


namespace jmsd {
namespace cutf {


// Converts a TestPartResult::Type enum to human-friendly string
// representation.  Both kNonFatalFailure and kFatalFailure are translated
// to "Failure", as the user usually doesn't care about the difference
// between the two when viewing the test result.
char const *TestPartResultTypeToString( ::testing::TestPartResult::Type const type ) {
	switch ( type ) {
		case ::testing::TestPartResult::Type::kSkip:
			return "Skipped";
		case ::testing::TestPartResult::Type::kSuccess:
			return "Success";

		case ::testing::TestPartResult::Type::kNonFatalFailure:
		case ::testing::TestPartResult::Type::kFatalFailure:
#ifdef _MSC_VER
			return "error: ";
#else
			return "Failure\n";
#endif
		default:
			return "Unknown result type";
	}
}


} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
