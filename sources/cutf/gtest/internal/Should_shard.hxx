#pragma once


#include "gtest-port.h"


namespace jmsd {
namespace cutf {
namespace internal {


// Checks whether sharding is enabled by examining the relevant
// environment variable values. If the variables are present,
// but inconsistent (e.g., shard_index >= total_shards), prints
// an error and exits. If in_subprocess_for_death_test, sharding is
// disabled because it must only be applied to the original test
// process. Otherwise, we could filter out death tests we intended to execute.
GTEST_API_ bool ShouldShard( char const *total_shards_env, char const *shard_index_env, bool in_subprocess_for_death_test);


} // namespace internal
} // namespace cutf
} // namespace jmsd
