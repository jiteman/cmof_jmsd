#pragma once


#include "gtest-port.h"


namespace jmsd {
namespace cutf {
namespace internal {


// Parses the environment variable var as a 32-bit integer.
// If it is unset, returns default_val.
// If it is not a 32-bit integer, prints an error and aborts.
GTEST_API_ int32_t Int32FromEnvOrDie(const char* env_var, int32_t default_val);


} // namespace internal
} // namespace cutf
} // namespace jmsd
