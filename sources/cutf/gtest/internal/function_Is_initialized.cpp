#include "function_Is_initialized.h"


#include "gtest-port.h"


namespace jmsd {
namespace cutf {
namespace internal {


// GTestIsInitialized() returns true if and only if the user has initialized Google Test.
// Useful for catching the user mistake of not initializing Google Test before calling RUN_ALL_TESTS().
// static
bool function_Is_initialized::GTestIsInitialized() {
	return ::testing::internal::GetArgvs().size() > 0;
}


} // namespace internal
} // namespace cutf
} // namespace jmsd
