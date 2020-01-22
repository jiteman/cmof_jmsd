#pragma once


#include "cutf.h"


namespace jmsd {
namespace cutf {
namespace internal {


// Given the total number of shards, the shard index, and the test id,
// returns true if and only if the test should be run on this shard. The test id
// is some arbitrary but unique non-negative integer assigned to each test
// method. Assumes that 0 <= shard_index < total_shards.
JMSD_CUTF_SHARED_INTERFACE bool ShouldRunTestOnShard( int total_shards, int shard_index, int test_id);


} // namespace internal
} // namespace cutf
} // namespace jmsd
