#include "Death_test_check.h"


#include "gtest-death-test-internal.h"

#include "Unit_test_impl.h"


namespace jmsd {
namespace cutf {
namespace internal {


// Routine for aborting the program which is safe to call from an
// exec-style death test child process, in which case the error
// message is propagated back to the parent process.  Otherwise, the
// message is simply printed to stderr.  In either case, the program
// then exits with status 1.
void DeathTestAbort(const std::string& message) {
  // On a POSIX system, this function may be called from a threadsafe-style
  // death test child process, which operates on a very small stack.  Use
  // the heap for any additional non-minuscule memory requirements.
  const ::testing::internal::InternalRunDeathTestFlag* const flag =
	  GetUnitTestImpl()->internal_run_death_test_flag();
  if (flag != nullptr) {
	FILE* parent = ::testing::internal::posix::FDOpen(flag->write_fd(), "w");
	fputc( ::testing::internal::kDeathTestInternalError, parent);
	fprintf(parent, "%s", message.c_str());
	fflush(parent);
	_exit(1);
  } else {
	fprintf(stderr, "%s", message.c_str());
	fflush(stderr);
	::testing::internal::posix::Abort();
  }
}


} // namespace internal
} // namespace cutf
} // namespace jmsd
