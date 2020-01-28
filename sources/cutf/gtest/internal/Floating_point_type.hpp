#pragma once

#include "Floating_point_type.hxx"


#include "gtest-port.h"

#include <limits>


namespace jmsd {
namespace cutf {
namespace internal {


// This template class represents an IEEE floating-point number (either single-precision or double-precision, depending on the template parameters).
//
// The purpose of this class is to do more sophisticated number comparison.
// (Due to round-off error, etc, it's very unlikely that two floating-points will be equal exactly.
// Hence a naive comparison by the == operation often doesn't work.)
//
// Format of IEEE floating-point:
//
//   The most-significant bit being the leftmost, an IEEE floating-point looks like
//
//     sign_bit exponent_bits fraction_bits
//
//   Here, sign_bit is a single bit that designates the sign of the number.
//
//   For float, there are 8 exponent bits and 23 fraction bits.
//
//   For double, there are 11 exponent bits and 52 fraction bits.
//
//   More details can be found at http://en.wikipedia.org/wiki/IEEE_floating-point_standard.
//
// Template parameter:
//   RawType: the raw floating-point type (either float or double)

template <typename RawType>
class FloatingPoint {
public:
	// Defines the unsigned integer type that has the same size as the floating point number.
	typedef typename ::testing::internal::TypeWithSize< sizeof( RawType )>::UInt Bits;

public:
	// # of bits in a number.
	static size_t const kBitCount;

	// # of fraction bits in a number.
	static size_t const kFractionBitCount;

	// # of exponent bits in a number.
	static size_t const kExponentBitCount;

	// The mask for the sign bit.
	static Bits const kSignBitMask;

	// The mask for the fraction bits.
	static Bits const kFractionBitMask;

	// The mask for the exponent bits.
	static Bits const kExponentBitMask;

	// How many ULP's (Units in the Last Place) we want to tolerate when
	// comparing two numbers.  The larger the value, the more error we
	// allow.  A 0 value means that two numbers must be exactly the same
	// to be considered equal.
	//
	// The maximum error of a single floating-point operation is 0.5
	// units in the last place.  On Intel CPU's, all floating-point
	// calculations are done with 80-bit precision, while double has 64
	// bits.  Therefore, 4 should be enough for ordinary use.
	//
	// See the following article for more details on ULP:
	// http://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
	static size_t const kMaxUlps;

	// Constructs a FloatingPoint from a raw floating-point number.
	//
	// On an Intel CPU, passing a non-normalized NAN (Not a Number)
	// around may change its bits, although the new value is guaranteed
	// to be also a NAN.  Therefore, don't expect this constructor to
	// preserve the bits in x when x is a NAN.
	explicit FloatingPoint( RawType const &x );

public:
	// Reinterprets a bit pattern as a floating-point number.
	//
	// This function is needed to test the AlmostEquals() method.
	static RawType ReinterpretBits( Bits const bits);

	// Returns the floating-point number that represent positive infinity.
	static RawType Infinity();

	// Returns the maximum representable finite floating-point number.
	static RawType Max();

public:
	// Returns the bits that represents this number.
	Bits const &bits() const;

	// Returns the exponent bits of this number.
	Bits exponent_bits() const;

	// Returns the fraction bits of this number.
	Bits fraction_bits() const;

	// Returns the sign bit of this number.
	Bits sign_bit() const;

	// Returns true if and only if this is NAN (not a number).
	bool is_nan() const;

	// Returns true if and only if this number is at most kMaxUlps ULP's away
	// from rhs.  In particular, this function:
	//
	//   - returns false if either number is (or both are) NAN.
	//   - treats really large numbers as almost equal to infinity.
	//   - thinks +0.0 and -0.0 are 0 DLP's apart.
	bool AlmostEquals( FloatingPoint const &rhs ) const;

private:
	// The data type used to store the actual floating-point number.
	union FloatingPointUnion {
		RawType value_; // The raw floating-point number.
		Bits bits_; // The bits that represent the number.
	};

	// Converts an integer from the sign-and-magnitude representation to
	// the biased representation.  More precisely, let N be 2 to the
	// power of (kBitCount - 1), an integer x is represented by the
	// unsigned number x + N.
	//
	// For instance,
	//
	//   -N + 1 (the most negative number representable using
	//          sign-and-magnitude) is represented by 1;
	//   0      is represented by N; and
	//   N - 1  (the biggest number representable using
	//          sign-and-magnitude) is represented by 2N - 1.
	//
	// Read http://en.wikipedia.org/wiki/Signed_number_representations
	// for more details on signed number representations.
	static Bits SignAndMagnitudeToBiased( Bits const &sam );

	// Given two numbers in the sign-and-magnitude representation, returns the distance between them as an unsigned number.
	static Bits DistanceBetweenSignAndMagnitudeNumbers( Bits const &sam1, Bits const &sam2 );

private:
	FloatingPointUnion u_;

};

// Typedefs the instances of the FloatingPoint template class that we care to use.
typedef FloatingPoint< float > Float;
typedef FloatingPoint< double > Double;


} // namespace internal
} // namespace cutf
} // namespace jmsd
