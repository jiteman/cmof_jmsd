#pragma once

#include "function_Stl_utilities.hxx"


#include "Random_number_generator.hxx"

#include <vector>


namespace jmsd {
namespace cutf {
namespace internal {


class function_Stl_utilities {

public:
	// Returns the number of elements in the given container that satisfy the given predicate.
	template< class Container, typename Predicate >
	static int CountIf( Container const &c, Predicate predicate );

	// Applies a function/functor to each element in the container.
	template< class Container, typename Functor >
	static void ForEach( Container const &c, Functor functor );

	// Returns the i-th element of the vector, or default_value if i is not in range [0, v.size()).
	template< typename Element >
	static Element GetElementOr( ::std::vector< Element > const &v, int i, Element default_value );

	// Performs an in-place shuffle of a range of the vector's elements.
	// 'begin' and 'end' are element indices as an STL-style range;
	// i.e. [begin, end) are shuffled, where 'end' == size() means to
	// shuffle to the end of the vector.
	template< typename Element >
	static void ShuffleRange( Random *random, int begin, int end, ::std::vector< Element > *v );

	// Performs an in-place shuffle of the vector's elements.
	template< typename Element >
	static void Shuffle( Random *random, ::std::vector< Element > *v );

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
public:

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
private:
	virtual ~function_Stl_utilities() noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	function_Stl_utilities() noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	function_Stl_utilities( function_Stl_utilities const &another ) noexcept = delete;
	function_Stl_utilities &operator =( function_Stl_utilities const &another ) noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	function_Stl_utilities( function_Stl_utilities &&another ) noexcept = delete;
	function_Stl_utilities &operator =( function_Stl_utilities &&another ) noexcept = delete;

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
