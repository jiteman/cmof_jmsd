#pragma once

#include "function_Should_shard.hxx"


#include "cutf.h"


namespace jmsd {
namespace cutf {
namespace internal {


class JMSD_CUTF_SHARED_INTERFACE function_Should_shard {

public:
	// Checks whether sharding is enabled by examining the relevant environment variable values.
	// If the variables are present, but inconsistent (e.g., shard_index >= total_shards), prints an error and exits.
	// If in_subprocess_for_death_test, sharding is disabled because it must only be applied to the original test process.
	// Otherwise, we could filter out death tests we intended to execute.
	static bool ShouldShard( char const *total_shards_env, char const *shard_index_env, bool in_subprocess_for_death_test);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
public:

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
private:
	virtual ~function_Should_shard() noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	function_Should_shard() noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	function_Should_shard( function_Should_shard const &another ) noexcept = delete;
	function_Should_shard &operator =( function_Should_shard const &another ) noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	function_Should_shard( function_Should_shard &&another ) noexcept = delete;
	function_Should_shard &operator =( function_Should_shard &&another ) noexcept = delete;

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
