#include "Random_number_generator.h"


namespace jmsd {
namespace cutf {
namespace internal {


// static
uint32_t const Random::kMaxRange = 1u << 31;

Random::Random( uint32_t const seed)
	:
		state_( seed )
{}

void Random::Reseed( uint32_t const seed ) {
	state_ = seed;
}

// Generates a random number from [0, range), using a Linear Congruential Generator (LCG).
// Crashes if 'range' is 0 or greater than kMaxRange.
uint32_t Random::Generate( uint32_t const range ) {
	// These constants are the same as are used in glibc's rand( 3 ).
	// Use wider types than necessary to prevent unsigned overflow diagnostics.
	state_ = static_cast< uint32_t >(1103515245ULL * state_ + 12345U ) % kMaxRange;

	GTEST_CHECK_( range > 0 ) << "Cannot generate a number in the range [0, 0).";
	GTEST_CHECK_( range <= kMaxRange ) << "Generation of a number in [0, " << range << ") was requested, " << "but this can only generate numbers in [0, " << kMaxRange << ").";

	// Converting via modulus introduces a bit of downward bias, but it's simple, and a linear congruential generator isn't too good to begin with.
	return state_ % range;
}


} // namespace internal
} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
