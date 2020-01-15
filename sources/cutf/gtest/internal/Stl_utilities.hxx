#pragma once


#include "Random_number_generator.hxx"

#include <vector>


namespace jmsd {
namespace cutf {
namespace internal {


// STL container utilities.

// Returns the number of elements in the given container that satisfy the given predicate.
template< class Container, typename Predicate >
int CountIf( Container const &c, Predicate predicate );

// Applies a function/functor to each element in the container.
template< class Container, typename Functor >
void ForEach( Container const &c, Functor functor );

// Returns the i-th element of the vector, or default_value if i is not in range [0, v.size()).
template< typename E >
E GetElementOr( ::std::vector< E > const &v, int i, E default_value );

// Performs an in-place shuffle of a range of the vector's elements.
// 'begin' and 'end' are element indices as an STL-style range;
// i.e. [begin, end) are shuffled, where 'end' == size() means to
// shuffle to the end of the vector.
template< typename E >
void ShuffleRange( Random *random, int begin, int end, ::std::vector< E > *v );

// Performs an in-place shuffle of the vector's elements.
template< typename E >
void Shuffle( Random *random, ::std::vector< E > *v );

// A function for deleting an object.  Handy for being used as a functor.
template< typename T >
void Delete( T *x );


} // namespace internal
} // namespace cutf
} // namespace jmsd


namespace testing {
namespace internal {


} // namespace internal
} // namespace testing
