// This test verifies that it's possible to use Google Test by including
// the gtest.h header file alone.

#include "gtest/gtest.h"

namespace {

void Subroutine() {
  EXPECT_EQ(42, 42);
}

TEST(SoleHeader_NoFatalFailureTest, ExpectNoFatalFailure) {
  EXPECT_NO_FATAL_FAILURE(;);
  EXPECT_NO_FATAL_FAILURE(SUCCEED());
  EXPECT_NO_FATAL_FAILURE(Subroutine());
  EXPECT_NO_FATAL_FAILURE({ SUCCEED(); });
}

TEST(SoleHeader_NoFatalFailureTest, AssertNoFatalFailure) {
  ASSERT_NO_FATAL_FAILURE(;);
  ASSERT_NO_FATAL_FAILURE(SUCCEED());
  ASSERT_NO_FATAL_FAILURE(Subroutine());
  ASSERT_NO_FATAL_FAILURE({ SUCCEED(); });
}

}  // namespace
