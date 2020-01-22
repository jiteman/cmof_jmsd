#pragma once


#include "gtest-internal.hxx"


#include "cutf.h"


namespace jmsd {
namespace cutf {
namespace internal {


// Outputs a message explaining invalid registration of different fixture class for the same test suite.
// This may happen when TEST_P macro is used to define two tests with the same name but in different namespaces.
JMSD_CUTF_SHARED_INTERFACE void ReportInvalidTestSuiteType( char const *test_suite_name, ::testing::internal::CodeLocation const &code_location );


} // namespace internal
} // namespace cutf
} // namespace jmsd
