#pragma once

#include "Random_number_generator.hxx"


#include "gtest-port.h"


namespace jmsd {
namespace cutf {
namespace internal {


// A simple Linear Congruential Generator for generating random numbers with a uniform distribution.
// Unlike rand() and srand(), it doesn't use global state (and therefore can't interfere with user code).
// Unlike rand_r(), it's portable.
// An LCG isn't very random, but it's good enough for our purposes.
class JMSD_DEPRECATED_GTEST_API_ Random {

public:
	static const uint32_t kMaxRange;

	explicit Random( uint32_t seed );

	void Reseed( uint32_t seed );

	// Generates a random number from [0, range).  Crashes if 'range' is 0 or greater than kMaxRange.
	uint32_t Generate( uint32_t range );

private:
	uint32_t state_;
	GTEST_DISALLOW_COPY_AND_ASSIGN_( Random );
};


} // namespace internal
} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
