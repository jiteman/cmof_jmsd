#pragma once


#include <string>


namespace jmsd {
namespace cutf {
namespace internal {


// Routine for aborting the program which is safe to call from an
// exec-style death test child process, in which case the error
// message is propagated back to the parent process.  Otherwise, the
// message is simply printed to stderr.  In either case, the program
// then exits with status 1.
void DeathTestAbort(const std::string& message);


} // namespace internal
} // namespace cutf
} // namespace jmsd
