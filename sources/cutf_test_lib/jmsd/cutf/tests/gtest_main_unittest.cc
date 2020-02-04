#include "gtest/gtest.h"

// Tests that we don't have to define main() when we link to
// gtest_main instead of gtest.

namespace {

TEST(GTestMainTest, ShouldSucceed) {
}

}  // namespace

// We are using the main() function defined in gtest_main.cc, so we
// don't define it here.
