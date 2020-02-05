#include "function_Write_to_shard_status_file_if_needed.h"


#include "gtest/gtest-constants.h"

#include "Colored_print.h"
#include "gtest-color.h"

#include "gtest-port.h"


namespace jmsd {
namespace cutf {
namespace internal {


// Reads the GTEST_SHARD_STATUS_FILE environment variable, and creates the file
// if the variable is present. If a file already exists at this location, this
// function will write over it. If the variable is present, but the file cannot
// be created, prints an error and exits.
// static
void function_Write_to_shard_status_file_if_needed::WriteToShardStatusFileIfNeeded() {
  const char* const test_shard_file = ::testing::internal::posix::GetEnv( constants::kTestShardStatusFile );
  if (test_shard_file != nullptr) {
	FILE* const file = ::testing::internal::posix::FOpen(test_shard_file, "w");
	if (file == nullptr) {
	  Colored_print::ColoredPrintf( GTestColor::COLOR_RED,
					"Could not write to the test shard status file \"%s\" "
					"specified by the %s environment variable.\n",
					test_shard_file, constants::kTestShardStatusFile );
	  fflush(stdout);
	  exit(EXIT_FAILURE);
	}
	fclose(file);
  }
}



} // namespace internal
} // namespace cutf
} // namespace jmsd
