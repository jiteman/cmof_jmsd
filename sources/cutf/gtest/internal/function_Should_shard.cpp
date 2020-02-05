#include "function_Should_shard.h"


#include "gtest/Message.h"
#include "gtest/gtest-constants.h"

#include "Colored_print.h"
#include "gtest-color.h"
#include "function_Int32_from_environment_or_die.h"

#include "gtest/Message.hin"


namespace jmsd {
namespace cutf {
namespace internal {


// Checks whether sharding is enabled by examining the relevant
// environment variable values. If the variables are present,
// but inconsistent (i.e., shard_index >= total_shards), prints
// an error and exits. If in_subprocess_for_death_test, sharding is
// disabled because it must only be applied to the original test
// process. Otherwise, we could filter out death tests we intended to execute.
// static
bool function_Should_shard::ShouldShard( char const *const total_shards_env, char const *const shard_index_env, bool const in_subprocess_for_death_test) {
  if ( in_subprocess_for_death_test ) {
	return false;
  }

  const int32_t total_shards = internal::function_Int32_from_environment_or_die::Int32FromEnvOrDie(total_shards_env, -1);
  const int32_t shard_index = internal::function_Int32_from_environment_or_die::Int32FromEnvOrDie(shard_index_env, -1);

  if (total_shards == -1 && shard_index == -1) {
	return false;
  } else if (total_shards == -1 && shard_index != -1) {
	const Message msg = Message()
	  << "Invalid environment variables: you have "
	  << constants::kTestShardIndex << " = " << shard_index
	  << ", but have left " << constants::kTestTotalShards << " unset.\n";
	Colored_print::ColoredPrintf( GTestColor::COLOR_RED, "%s", msg.GetString().c_str());
	fflush(stdout);
	exit(EXIT_FAILURE);
  } else if (total_shards != -1 && shard_index == -1) {
	const Message msg = Message()
	  << "Invalid environment variables: you have "
	  << constants::kTestTotalShards << " = " << total_shards
	  << ", but have left " << constants::kTestShardIndex << " unset.\n";
	Colored_print::ColoredPrintf( GTestColor::COLOR_RED, "%s", msg.GetString().c_str());
	fflush(stdout);
	exit(EXIT_FAILURE);
  } else if (shard_index < 0 || shard_index >= total_shards) {
	const Message msg = Message()
	  << "Invalid environment variables: we require 0 <= "
	  << constants::kTestShardIndex << " < " << constants::kTestTotalShards
	  << ", but you have " << constants::kTestShardIndex << "=" << shard_index
	  << ", " << constants::kTestTotalShards << "=" << total_shards << ".\n";
	Colored_print::ColoredPrintf( GTestColor::COLOR_RED, "%s", msg.GetString().c_str());
	fflush(stdout);
	exit(EXIT_FAILURE);
  }

  return total_shards > 1;
}



} // namespace internal
} // namespace cutf
} // namespace jmsd
