#pragma once

// This file implements some actions that depend on gmock-generated-actions.h.

#include "gmock/gmock-generated-actions.h"

#include <algorithm>
#include <type_traits>


namespace testing {
namespace internal {

// An internal replacement for std::copy which mimics its behavior. This is
// necessary because Visual Studio deprecates ::std::copy, issuing warning 4996.
// However Visual Studio 2010 and later do not honor #pragmas which disable that
// warning.
template<typename InputIterator, typename OutputIterator>
inline OutputIterator CopyElements(InputIterator first,
                                   InputIterator last,
                                   OutputIterator output) {
  for (; first != last; ++first, ++output) {
    *output = *first;
  }
  return output;
}

}  // namespace internal

// Various overloads for Invoke().

// Action ReturnArg<k>() returns the k-th argument of the mock function.
ACTION_TEMPLATE(ReturnArg,
                HAS_1_TEMPLATE_PARAMS(int, k),
                AND_0_VALUE_PARAMS()) {
  return ::std::get<k>(args);
}

// Action SaveArg<k>(pointer) saves the k-th (0-based) argument of the
// mock function to *pointer.
ACTION_TEMPLATE(SaveArg,
                HAS_1_TEMPLATE_PARAMS(int, k),
                AND_1_VALUE_PARAMS(pointer)) {
  *pointer = ::std::get<k>(args);
}

// Action SaveArgPointee<k>(pointer) saves the value pointed to
// by the k-th (0-based) argument of the mock function to *pointer.
ACTION_TEMPLATE(SaveArgPointee,
                HAS_1_TEMPLATE_PARAMS(int, k),
                AND_1_VALUE_PARAMS(pointer)) {
  *pointer = *::std::get<k>(args);
}

// Action SetArgReferee<k>(value) assigns 'value' to the variable
// referenced by the k-th (0-based) argument of the mock function.
ACTION_TEMPLATE(SetArgReferee,
                HAS_1_TEMPLATE_PARAMS(int, k),
                AND_1_VALUE_PARAMS(value)) {
  typedef typename ::std::tuple_element<k, args_type>::type argk_type;
  // Ensures that argument #k is a reference.  If you get a compiler
  // error on the next line, you are using SetArgReferee<k>(value) in
  // a mock function whose k-th (0-based) argument is not a reference.
  GTEST_COMPILE_ASSERT_(std::is_reference<argk_type>::value,
                        SetArgReferee_must_be_used_with_a_reference_argument);
  ::std::get<k>(args) = value;
}

// Action SetArrayArgument<k>(first, last) copies the elements in
// source range [first, last) to the array pointed to by the k-th
// (0-based) argument, which can be either a pointer or an
// iterator. The action does not take ownership of the elements in the
// source range.
ACTION_TEMPLATE(SetArrayArgument,
                HAS_1_TEMPLATE_PARAMS(int, k),
                AND_2_VALUE_PARAMS(first, last)) {
  // Visual Studio deprecates ::std::copy, so we use our own copy in that case.
#ifdef _MSC_VER
  internal::CopyElements(first, last, ::std::get<k>(args));
#else
  ::std::copy(first, last, ::std::get<k>(args));
#endif
}

// Action DeleteArg<k>() deletes the k-th (0-based) argument of the mock
// function.
ACTION_TEMPLATE(DeleteArg,
                HAS_1_TEMPLATE_PARAMS(int, k),
                AND_0_VALUE_PARAMS()) {
  delete ::std::get<k>(args);
}

// This action returns the value pointed to by 'pointer'.
ACTION_P(ReturnPointee, pointer) { return *pointer; }

// Action Throw(exception) can be used in a mock function of any type
// to throw the given exception.  Any copyable value can be thrown.
#if GTEST_HAS_EXCEPTIONS

ACTION_P(Throw, exception) { throw exception; }

#endif  // GTEST_HAS_EXCEPTIONS


}  // namespace testing
