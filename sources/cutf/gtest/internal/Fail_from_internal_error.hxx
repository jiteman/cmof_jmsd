#pragma once


#include <string>


namespace jmsd {
namespace cutf {
namespace internal {


// Returns the message describing the last system error in errno.
::std::string GetLastErrnoDescription();

// This is called from a death test parent process to read a failure
// message from the death test child process and log it with the FATAL
// severity. On Windows, the message is read from a pipe handle. On other
// platforms, it is read from a file descriptor.
void FailFromInternalError( int fd );


} // namespace internal
} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
