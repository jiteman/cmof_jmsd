#pragma once

// #include "gtti.hxx"


#include "cutf.h"


namespace jmsd {
namespace cutf {
namespace constants {
namespace internal {



// Text printed in Google Test's text output and --gtest_list_tests output to label the type parameter and value parameter for a test.
extern const char kTypeParamLabel[];
extern const char kValueParamLabel[];

// The text used in failure messages to indicate the start of the stack trace.
JMSD_CUTF_SHARED_INTERFACE extern const char kStackTraceMarker[];


} // namespace internal
} // namespace constants
} // namespace cutf
} // namespace jmsd

