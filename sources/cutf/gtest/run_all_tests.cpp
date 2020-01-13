#include "run_all_tests.h"


#include "gtest.h" // for UnitTest


namespace jmsd {
namespace cutf {


int RUN_ALL_TESTS() {
	return ::testing::UnitTest::GetInstance()->Run();
}


} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
