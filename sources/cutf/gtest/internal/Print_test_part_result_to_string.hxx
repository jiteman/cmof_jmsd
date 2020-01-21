#pragma once


#include "gtest/gtest-test-part.h"


#include <string>


namespace jmsd {
namespace cutf {
namespace internal {


::std::string PrintTestPartResultToString( ::testing::TestPartResult const &test_part_result );


} // namespace internal
} // namespace cutf
} // namespace jmsd
