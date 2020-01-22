#pragma once

#include "Floating_point_comparator.hxx"


#include "Assertion_result.hxx"


#include "cutf.h"


namespace jmsd {
namespace cutf {


class JMSD_CUTF_SHARED_INTERFACE Floating_point_comparator {

public:
	static AssertionResult float_less_or_near_equal( char const *expr1, char const *expr2, float val1, float val2 );
	static AssertionResult double_less_or_near_equal( char const *expr1, char const *expr2, double val1, double val2 );
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
public:

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
public:
	virtual ~Floating_point_comparator() noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
protected:
	Floating_point_comparator() noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	Floating_point_comparator( Floating_point_comparator const &another ) noexcept = delete;
	Floating_point_comparator &operator =( Floating_point_comparator const &another ) noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	Floating_point_comparator( Floating_point_comparator &&another ) noexcept = delete;
	Floating_point_comparator &operator =( Floating_point_comparator &&another ) noexcept = delete;

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
private:

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:

};


} // namespace cutf
} // namespace jmsd
