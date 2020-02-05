#include "function_Should_run_test_on_shard.h"


namespace jmsd {
namespace cutf {
namespace internal {


// Given the total number of shards, the shard index, and the test id,
// returns true if and only if the test should be run on this shard. The test id
// is some arbitrary but unique non-negative integer assigned to each test
// method. Assumes that 0 <= shard_index < total_shards.
// static
bool function_Should_run_test_on_shard::ShouldRunTestOnShard( int const total_shards, int const shard_index, int const test_id ) {
	return ( test_id % total_shards ) == shard_index;
}


} // namespace internal
} // namespace cutf
} // namespace jmsd
