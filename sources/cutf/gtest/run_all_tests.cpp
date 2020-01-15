#include "run_all_tests.h"


#include "Unit_test.h"


namespace jmsd {
namespace cutf {


int RUN_ALL_TESTS() {
	return ::jmsd::cutf::UnitTest::GetInstance()->Run();
}


} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
