#pragma once

#include "function_Should_run_test_on_shard.hxx"


#include "cutf.h"


namespace jmsd {
namespace cutf {
namespace internal {


class JMSD_CUTF_SHARED_INTERFACE function_Should_run_test_on_shard {

public:
	// Given the total number of shards, the shard index, and the test id,
	// returns true if and only if the test should be run on this shard. The test id
	// is some arbitrary but unique non-negative integer assigned to each test
	// method. Assumes that 0 <= shard_index < total_shards.
	static bool ShouldRunTestOnShard( int total_shards, int shard_index, int test_id );

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
public:

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
private:
	virtual ~function_Should_run_test_on_shard() noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	function_Should_run_test_on_shard() noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	function_Should_run_test_on_shard( function_Should_run_test_on_shard const &another ) noexcept = delete;
	function_Should_run_test_on_shard &operator =( function_Should_run_test_on_shard const &another ) noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	function_Should_run_test_on_shard( function_Should_run_test_on_shard &&another ) noexcept = delete;
	function_Should_run_test_on_shard &operator =( function_Should_run_test_on_shard &&another ) noexcept = delete;

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
private:

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:

};


} // namespace internal
} // namespace cutf
} // namespace jmsd
