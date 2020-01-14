#pragma once

//#include "gtest-flags.hxx"

#include "internal/gtest-port.h"


namespace jmsd {
namespace cutf {





} // namespace cutf
} // namespace jmsd


namespace testing {


// Declares the flags.

// This flag temporary enables the disabled tests.
GTEST_DECLARE_FLAG_bool_(also_run_disabled_tests);

// This flag brings the debugger on an assertion failure.
GTEST_DECLARE_FLAG_bool_(break_on_failure);

// This flag controls whether Google Test catches all test-thrown exceptions
// and logs them as failures.
GTEST_DECLARE_FLAG_bool_(catch_exceptions);

// This flag enables using colors in terminal output. Available values are
// "yes" to enable colors, "no" (disable colors), or "auto" (the default)
// to let Google Test decide.
GTEST_DECLARE_FLAG_string_(color);

// This flag sets up the filter to select by name using a glob pattern
// the tests to run. If the filter is not given all tests are executed.
GTEST_DECLARE_FLAG_string_(filter);

// This flag controls whether Google Test installs a signal handler that dumps
// debugging information when fatal signals are raised.
GTEST_DECLARE_FLAG_bool_(install_failure_signal_handler);

// This flag causes the Google Test to list tests. None of the tests listed
// are actually run if the flag is provided.
GTEST_DECLARE_FLAG_bool_(list_tests);

// This flag controls whether Google Test emits a detailed XML report to a file
// in addition to its normal textual output.
GTEST_DECLARE_FLAG_string_(output);

// This flags control whether Google Test prints the elapsed time for each
// test.
GTEST_DECLARE_FLAG_bool_(print_time);

// This flags control whether Google Test prints UTF8 characters as text.
GTEST_DECLARE_FLAG_bool_(print_utf8);

// This flag specifies the random number seed.
GTEST_DECLARE_FLAG_int32_(random_seed);

// This flag sets how many times the tests are repeated. The default value
// is 1. If the value is -1 the tests are repeating forever.
GTEST_DECLARE_FLAG_int32_(repeat);

// This flag controls whether Google Test includes Google Test internal
// stack frames in failure stack traces.
GTEST_DECLARE_FLAG_bool_(show_internal_stack_frames);

// When this flag is specified, tests' order is randomized on every iteration.
GTEST_DECLARE_FLAG_bool_(shuffle);

// This flag specifies the maximum number of stack frames to be
// printed in a failure message.
GTEST_DECLARE_FLAG_int32_(stack_trace_depth);

// When this flag is specified, a failed assertion will throw an
// exception if exceptions are enabled, or exit the program with a
// non-zero code otherwise. For use with an external test framework.
GTEST_DECLARE_FLAG_bool_(throw_on_failure);

// When this flag is set with a "host:port" string, on supported
// platforms test results are streamed to the specified port on
// the specified host machine.
GTEST_DECLARE_FLAG_string_(stream_result_to);

#if GTEST_USE_OWN_FLAGFILE_FLAG_
GTEST_DECLARE_FLAG_string_(flagfile);
#endif  // GTEST_USE_OWN_FLAGFILE_FLAG_


} // namespace testing
