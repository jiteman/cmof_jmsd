#include "gtest/Message.hin"

#include "gtest/gtest.h"


GTEST_DISABLE_MSC_WARNINGS_PUSH_( 4702 )

TEST( SkipTest, DISABLED_DoesSkip ) {
	GTEST_SKIP() << "skipping single test";

	EXPECT_EQ(0, 1);
GTEST_DISABLE_MSC_WARNINGS_POP_()
}

class Fixture : public ::jmsd::cutf::Test {
 protected:
  void SetUp() override {
    GTEST_SKIP() << "skipping all tests for this fixture";
  }
};

TEST_F(Fixture, DISABLED_SkipsOneTest) {
  EXPECT_EQ(5, 7);
}

TEST_F(Fixture, DISABLED_SkipsAnotherTest) {
  EXPECT_EQ(99, 100);
}
