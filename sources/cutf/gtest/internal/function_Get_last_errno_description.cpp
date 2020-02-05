#include "function_Get_last_errno_description.h"


#include "gtest-port.h"


namespace jmsd {
namespace cutf {
namespace internal {


// Returns the message describing the last system error in errno.
::std::string function_Get_last_errno_description::GetLastErrnoDescription() {
	return errno == 0 ? "" : ::testing::internal::posix::StrError( errno );
}


} // namespace internal
} // namespace cutf
} // namespace jmsd
