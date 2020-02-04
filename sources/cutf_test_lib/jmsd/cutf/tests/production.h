#pragma once

// This is part of the unit test for gtest_prod.h.

#include "gtest/gtest_prod.h"

class PrivateCode {
 public:
  // Declares a friend test that does not use a fixture.
  FRIEND_TEST(PrivateCodeTest, CanAccessPrivateMembers);

  // Declares a friend test that uses a fixture.
  FRIEND_TEST(PrivateCodeFixtureTest, CanAccessPrivateMembers);

  PrivateCode();

  int x() const { return x_; }
 private:
  void set_x(int an_x) { x_ = an_x; }
  int x_;
};
