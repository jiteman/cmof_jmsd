#include "Scoped_premature_exit_file.h"


#include "gtest-port.h"


namespace jmsd {
namespace cutf {
namespace internal {


ScopedPrematureExitFile::ScopedPrematureExitFile( char const *premature_exit_filepath )
	:
		premature_exit_filepath_( premature_exit_filepath ? premature_exit_filepath : "" )
{
	// If a path to the premature-exit file is specified...
	if ( !premature_exit_filepath_.empty() ) {
		// create the file with a single "0" character in it.  I/O
		// errors are ignored as there's nothing better we can do and we
		// don't want to fail the test because of this.
		FILE* pfile = ::testing::internal::posix::FOpen(premature_exit_filepath, "w");
		fwrite( "0", 1, 1, pfile );
		fclose( pfile );
	}
}

ScopedPrematureExitFile::~ScopedPrematureExitFile() {
#if !defined GTEST_OS_ESP8266
	if ( !premature_exit_filepath_.empty() ) {
		int const retval = remove( premature_exit_filepath_.c_str() );

		if ( retval ) {
			GTEST_LOG_( ERROR ) << "Failed to remove premature exit filepath \"" << premature_exit_filepath_ << "\" with error " << retval;
		}
	}
#endif
}



} // namespace internal
} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
