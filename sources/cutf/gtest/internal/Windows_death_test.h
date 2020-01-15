#pragma once

#include "Windows_death_test.hxx"


#include "Death_test_impl.h"

#include "gtest-port.h"


# if GTEST_OS_WINDOWS

namespace jmsd {
namespace cutf {
namespace internal {


// WindowsDeathTest implements death tests on Windows. Due to the
// specifics of starting new processes on Windows, death tests there are
// always threadsafe, and Google Test considers the
// --gtest_death_test_style=fast setting to be equivalent to
// --gtest_death_test_style=threadsafe there.
//
// A few implementation notes:  Like the Linux version, the Windows
// implementation uses pipes for child-to-parent communication. But due to
// the specifics of pipes on Windows, some extra steps are required:
//
// 1. The parent creates a communication pipe and stores handles to both
//    ends of it.
// 2. The parent starts the child and provides it with the information
//    necessary to acquire the handle to the write end of the pipe.
// 3. The child acquires the write end of the pipe and signals the parent
//    using a Windows event.
// 4. Now the parent can release the write end of the pipe on its side. If
//    this is done before step 3, the object's reference count goes down to
//    0 and it is destroyed, preventing the child from acquiring it. The
//    parent now has to release it, or read operations on the read end of
//    the pipe will not return when the child terminates.
// 5. The parent reads child's output through the pipe (outcome code and
//    any possible error messages) from the pipe, and its stderr and then
//    determines whether to fail the test.
//
// Note: to distinguish Win32 API calls from the local method and function
// calls, the former are explicitly resolved in the global namespace.
//
class WindowsDeathTest : public DeathTestImpl {
 public:
  WindowsDeathTest(const char* a_statement, ::testing::Matcher<const std::string&> matcher,
				   const char* file, int line)
	  : DeathTestImpl(a_statement, std::move(matcher)),
		file_(file),
		line_(line) {}

  // All of these virtual functions are inherited from DeathTest.
  virtual int Wait();
  virtual TestRole AssumeRole();

 private:
  // The name of the file in which the death test is located.
  const char* const file_;
  // The line number on which the death test is located.
  const int line_;
  // Handle to the write end of the pipe to the child process.
  ::testing::internal::AutoHandle write_handle_;
  // Child process handle.
  ::testing::internal::AutoHandle child_handle_;
  // Event the child process uses to signal the parent that it has
  // acquired the handle to the write end of the pipe. After seeing this
  // event the parent can release its own handles to make sure its
  // ReadFile() calls return when the child terminates.
  ::testing::internal::AutoHandle event_handle_;
};


} // namespace internal
} // namespace cutf
} // namespace jmsd


#endif // # if GTEST_OS_WINDOWS


namespace testing {


} // namespace testing
