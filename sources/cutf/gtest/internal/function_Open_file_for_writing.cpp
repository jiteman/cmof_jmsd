#include "function_Open_file_for_writing.h"


#include "gtest-filepath.h"

#include "gtest-port.h"


namespace jmsd {
namespace cutf {
namespace internal {


// Utilty function to Open File for Writing
// static
FILE *function_Open_file_for_writing::OpenFileForWriting( ::std::string const &output_file ) {
	FILE* fileout = nullptr;
	::testing::internal::FilePath const output_file_path( output_file );
	::testing::internal::FilePath const output_dir( output_file_path.RemoveFileName() );

	if ( output_dir.CreateDirectoriesRecursively() ) {
		fileout = ::testing::internal::posix::FOpen (output_file.c_str(), "w" );
	}

	if ( fileout == nullptr ) {
		GTEST_LOG_( FATAL ) << "Unable to open file \"" << output_file << "\"";
	}

	return fileout;
}


} // namespace internal
} // namespace cutf
} // namespace jmsd
