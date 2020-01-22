#include "Floating_point_comparator.h"


#include "Assertion_result.h"

#include "Floating_point_less_or_near_equal.hin"


namespace jmsd {
namespace cutf {


// Asserts that val1 is less than, or almost equal to, val2.
// Fails otherwise.
// In particular, it fails if either val1 or val2 is NaN.
AssertionResult Floating_point_comparator::float_less_or_near_equal( char const *const expr1, char const *const expr2, float const val1, float const val2 ) {
	return Floating_point_less_or_near_equal< float >::compare( expr1, expr2, val1, val2 );
}

// Asserts that val1 is less than, or almost equal to, val2.
// Fails otherwise.
// In particular, it fails if either val1 or val2 is NaN.
AssertionResult Floating_point_comparator::double_less_or_near_equal( char const *const expr1, char const *const expr2, double const val1, double const val2 ) {
	return Floating_point_less_or_near_equal< double >::compare( expr1, expr2, val1, val2 );
}


} // namespace cutf
} // namespace jmsd
