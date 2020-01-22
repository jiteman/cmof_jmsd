#pragma once


#include "gtest/internal/gtest-port.h"


namespace jmsd {
namespace cutf {


// Use this function in main() to run all tests.  It returns 0 if all
// tests are successful, or 1 otherwise.
//
// RUN_ALL_TESTS() should be invoked after the command line has been
// parsed by InitGoogleTest().
//
// This function was formerly a macro; thus, it is in the global
// namespace and has an all-caps name.
int JMSD_DEPRECATED_GTEST_API_ RUN_ALL_TESTS() GTEST_MUST_USE_RESULT_;


} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
