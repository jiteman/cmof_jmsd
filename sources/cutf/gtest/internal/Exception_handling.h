#pragma once

#include "Exception_handling.hxx"


#include "gtest-port.h"


namespace jmsd {
namespace cutf {
namespace internal {


#if GTEST_HAS_EXCEPTIONS

GTEST_DISABLE_MSC_WARNINGS_PUSH_(4275 \
/* an exported class was derived from a class that was not exported */)

// This exception is thrown by (and only by) a failed Google Test
// assertion when GTEST_FLAG(throw_on_failure) is true (if exceptions
// are enabled).  We derive it from std::runtime_error, which is for
// errors presumably detectable only at run time.  Since
// std::runtime_error inherits from std::exception, many testing
// frameworks know how to extract and print the message inside it.
class JMSD_DEPRECATED_GTEST_API_ GoogleTestFailureException :
	public ::std::runtime_error
{
public:
	explicit GoogleTestFailureException( ::testing::TestPartResult const &failure );

};

GTEST_DISABLE_MSC_WARNINGS_POP_()  //  4275

#endif  // GTEST_HAS_EXCEPTIONS


} // namespace internal
} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
