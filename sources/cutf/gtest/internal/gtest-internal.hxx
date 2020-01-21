#pragma once


namespace testing {
namespace internal {


struct CodeLocation;

// In order to catch the mistake of putting tests that use different
// test fixture classes in the same test suite, we need to assign
// unique IDs to fixture classes and compare them.  The TypeId type is
// used to hold such IDs.  The user should treat TypeId as an opaque
// type: the only operation allowed on TypeId values is to compare
// them for equality using the == operator.
typedef const void* TypeId;

// Types of SetUpTestSuite() and TearDownTestSuite() functions.
using SetUpTestSuiteFunc = void (*)();
using TearDownTestSuiteFunc = void (*)();

class TestFactoryBase;


} // namespace internal
} // namespace testing
