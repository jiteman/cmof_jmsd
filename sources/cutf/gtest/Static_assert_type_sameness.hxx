#pragma once


namespace jmsd {
namespace cutf {


// Compile-time assertion for type equality.
// StaticAssertTypeEq<type1, type2>() compiles if and only if type1 and type2
// are the same type.  The value it returns is not interesting.
//
// Instead of making StaticAssertTypeEq a class template, we make it a
// function template that invokes a helper class template.  This
// prevents a user from misusing StaticAssertTypeEq<T1, T2> by
// defining objects of that type.
template< typename, typename >
constexpr bool Static_assert_type_sameness() noexcept;


} // namespace cutf
} // namespace jmsd
