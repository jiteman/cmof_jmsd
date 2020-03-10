// Tests Google Mock's functionality that depends on exceptions.

#include "gmock/gmock.h"
//#include "gtest/gtest.h"

#include "gtest/Assertion_result.hin"
#include "gtest/Message.hin"

#if GTEST_HAS_EXCEPTIONS
namespace {

using testing::HasSubstr;


// A type that cannot be default constructed.
class NonDefaultConstructible {
 public:
  explicit NonDefaultConstructible(int /* dummy */) {}
};

class MockFoo {
 public:
  // A mock method that returns a user-defined type.  Google Mock
  // doesn't know what the default value for this type is.
  MOCK_METHOD0(GetNonDefaultConstructible, NonDefaultConstructible());
};

TEST(DefaultValueTest, ThrowsRuntimeErrorWhenNoDefaultValue) {
  MockFoo mock;
  try {
    // No expectation is set on this method, so Google Mock must
    // return the default value.  However, since Google Mock knows
    // nothing about the return type, it doesn't know what to return,
    // and has to throw (when exceptions are enabled) or abort
    // (otherwise).
    mock.GetNonDefaultConstructible();
    FAIL() << "GetNonDefaultConstructible()'s return type has no default "
           << "value, so Google Mock should have thrown.";
  } catch (const ::jmsd::cutf::internal::GoogleTestFailureException& /* unused */) {
    FAIL() << "Google Test does not try to catch an exception of type "
           << "GoogleTestFailureException, which is used for reporting "
           << "a failure to other testing frameworks.  Google Mock should "
           << "not throw a GoogleTestFailureException as it will kill the "
           << "entire test program instead of just the current TEST.";
  } catch (const std::exception& ex) {
    EXPECT_THAT(ex.what(), HasSubstr("has no default value"));
  }
}


}  // unnamed namespace
#endif
