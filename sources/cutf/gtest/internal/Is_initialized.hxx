#pragma once


namespace jmsd {
namespace cutf {
namespace internal {


// GTestIsInitialized() returns true if and only if the user has initialized Google Test.
// Useful for catching the user mistake of not initializing Google Test before calling RUN_ALL_TESTS().
bool GTestIsInitialized();


} // namespace internal
} // namespace cutf
} // namespace jmsd
