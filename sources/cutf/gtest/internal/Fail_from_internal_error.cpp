#include "Fail_from_internal_error.h"


#include "gtest/Message.hin"

#include "gtest-port.h"


namespace jmsd {
namespace cutf {
namespace internal {


// This is called from a death test parent process to read a failure
// message from the death test child process and log it with the FATAL
// severity. On Windows, the message is read from a pipe handle. On other
// platforms, it is read from a file descriptor.
void FailFromInternalError( int const fd ) {
	Message error;
	char buffer[ 256 ];
	int num_read;

	do {
		while ( ( num_read = ::testing::internal::posix::Read( fd, buffer, 255 ) ) > 0 ) {
			buffer[ num_read ] = '\0';
			error << buffer;
		}
	} while ( num_read == -1 && errno == EINTR );

	if (num_read == 0) {
		GTEST_LOG_( FATAL ) << error.GetString();
	} else {
		int const last_error = errno;
		GTEST_LOG_( FATAL ) << "Error while reading death test internal: " << GetLastErrnoDescription() << " [" << last_error << "]";
	}
}


} // namespace internal
} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
