#include "Unit_test_options.h"


#include "gtest/gtest-constants.h"
#include "gtest/gtest-flags.h"


namespace jmsd {
namespace cutf {
namespace internal {


// Returns the output format, or "" for normal printed output.
std::string UnitTestOptions::GetOutputFormat() {
	char const *const gtest_output_flag = ::testing:: GTEST_FLAG( output ).c_str();
	char const *const colon = ::strchr( gtest_output_flag, ':' );

	return
		( colon == nullptr ) ?
			::std::string( gtest_output_flag ) :
			::std::string( gtest_output_flag, static_cast< size_t >( colon - gtest_output_flag ) );
}

// Returns the name of the requested output file, or the default if none was explicitly specified.
::std::string UnitTestOptions::GetAbsolutePathToOutputFile() {
	char const *const gtest_output_flag = ::testing:: GTEST_FLAG(output).c_str();

	::std::string format = GetOutputFormat();

	if ( format.empty() ) {
	format = ::std::string( constants::kDefaultOutputFormat );
	}

	char const *const colon = ::strchr( gtest_output_flag, ':' );

	if ( colon == nullptr ) {
	return
	internal::FilePath::MakeFileName(
	internal::FilePath(
	::jmsd::cutf::UnitTest::GetInstance()->original_working_dir() ),
	internal::FilePath( ::jmsd::cutf::constants::kDefaultOutputFile ),
	0,
	format.c_str() ).string();
	}

	internal::FilePath output_name( colon + 1 );

	if ( !output_name.IsAbsolutePath() ) {
	output_name =
	internal::FilePath::ConcatPaths(
	internal::FilePath( ::jmsd::cutf::UnitTest::GetInstance()->original_working_dir() ),
	internal::FilePath( colon + 1 ) );
	}

	if (!output_name.IsDirectory())
	return output_name.string();

	internal::FilePath result(internal::FilePath::GenerateUniqueFileName(
	output_name, internal::GetCurrentExecutableName(),
	GetOutputFormat().c_str()));
	return result.string();
}

} // namespace internal
} // namespace cutf
} // namespace jmsd
