#pragma once

#include "Death_test_check.hxx"


#include "gtest/gtest-message.h"

#include "gtest/internal/Streamable_to_string.hin"

#include "gtest-internal.h"


namespace jmsd {
namespace cutf {
namespace internal {


// A replacement for CHECK that calls DeathTestAbort if the assertion
// fails.
# define GTEST_DEATH_TEST_CHECK_(expression) \
  do { \
	if (!::testing::internal::IsTrue(expression)) { \
	  ::jmsd::cutf::internal::DeathTestAbort( \
		  ::std::string("CHECK failed: File ") + __FILE__ +  ", line " \
		  + StreamableToString(__LINE__) + ": " \
		  + #expression); \
	} \
  } while (::testing::internal::AlwaysFalse())

// This macro is similar to GTEST_DEATH_TEST_CHECK_, but it is meant for
// evaluating any system call that fulfills two conditions: it must return
// -1 on failure, and set errno to EINTR when it is interrupted and
// should be tried again.  The macro expands to a loop that repeatedly
// evaluates the expression as long as it evaluates to -1 and sets
// errno to EINTR.  If the expression evaluates to -1 but errno is
// something other than EINTR, DeathTestAbort is called.
# define GTEST_DEATH_TEST_CHECK_SYSCALL_(expression) \
  do { \
	int gtest_retval; \
	do { \
	  gtest_retval = (expression); \
	} while (gtest_retval == -1 && errno == EINTR); \
	if (gtest_retval == -1) { \
	  ::jmsd::cutf::internal::DeathTestAbort( \
		  ::std::string("CHECK failed: File ") + __FILE__ + ", line " \
		  + StreamableToString(__LINE__) + ": " \
		  + #expression + " != -1"); \
	} \
  } while (::testing::internal::AlwaysFalse())


} // namespace internal
} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
