#pragma once


#include "gtest-port.h"

#if GTEST_HAS_SEH
#include "custom/temporary_windows_include.h"
#endif // #if GTEST_HAS_SEH

#include <string>


namespace testing {


class TestPartResult;


} // namespace testing


namespace jmsd {
namespace cutf {

#if GTEST_HAS_SEH

// Adds an "exception thrown" fatal failure to the current test.  This
// function returns its result via an output parameter pointer because VC++
// prohibits creation of objects with destructors on stack in functions
// using __try (see error C2712).
::std::string *FormatSehExceptionMessage( DWORD exception_code, const char *location );

#endif  // GTEST_HAS_SEH

namespace internal {


#if GTEST_HAS_EXCEPTIONS

::std::string FormatCxxExceptionMessage( char const *description, char const *location );

// Runs the given method and handles SEH exceptions it throws, when
// SEH is supported; returns the 0-value for type Result in case of an
// SEH exception.  (Microsoft compilers cannot handle SEH and C++
// exceptions in the same function.  Therefore, we provide a separate
// wrapper function for handling SEH exceptions.)
template< class T, typename Result >
Result HandleSehExceptionsInMethodIfSupported( T *object, Result ( T::*method )(), char const *location );

// Runs the given method and catches and reports C++ and/or SEH-style
// exceptions, if they are supported; returns the 0-value for type
// Result in case of an SEH exception.
template< class T, typename Result >
Result HandleExceptionsInMethodIfSupported( T *object, Result ( T::*method )(), char const *location );


class GoogleTestFailureException;

#endif // #if GTEST_HAS_EXCEPTIONS


} // namespace internal
} // namespace cutf
} // namespace jmsd
