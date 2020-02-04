#include "gtest/gtest.h"

class SetupFailTest : public ::jmsd::cutf::Test {
 protected:
  static void SetUpTestSuite() {
    ASSERT_EQ("", "SET_UP_FAIL");
  }
};

TEST_F(SetupFailTest, NoopPassingTest) {}

class TearDownFailTest : public ::jmsd::cutf::Test {
 protected:
  static void TearDownTestSuite() {
    ASSERT_EQ("", "TEAR_DOWN_FAIL");
  }
};

TEST_F(TearDownFailTest, NoopPassingTest) {}
