#include "gtest-typed-test_test.h"

#include "gtest/gtest.h"

#include "gtest/Message.hin"

#include <vector>

#if GTEST_HAS_TYPED_TEST_P

// Tests that the same type-parameterized test case can be
// instantiated in different translation units linked together.
// (ContainerTest is also instantiated in gtest-typed-test_test.cc.)
INSTANTIATE_TYPED_TEST_SUITE_P(Vector, ContainerTest,
                               testing::Types<std::vector<int> >);

#endif  // GTEST_HAS_TYPED_TEST_P
