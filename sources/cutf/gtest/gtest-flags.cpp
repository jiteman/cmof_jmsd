#include "gtest-flags.h"


#include "function_Get_default_filter.h"
#include "gtest-constants.h"


namespace jmsd {
namespace cutf {





} // namespace cutf
} // namespace jmsd


namespace testing {


GTEST_DEFINE_FLAG_bool_(
	also_run_disabled_tests,
	internal::BoolFromGTestEnv("also_run_disabled_tests", false),
	"Run disabled tests too, in addition to the tests normally being run.");

GTEST_DEFINE_FLAG_bool_(
	break_on_failure, internal::BoolFromGTestEnv("break_on_failure", false),
	"True if and only if a failed assertion should be a debugger "
	"break-point.");

GTEST_DEFINE_FLAG_bool_(catch_exceptions,
				   internal::BoolFromGTestEnv("catch_exceptions", true),
				   "True if and only if " GTEST_NAME_
				   " should catch exceptions and treat them as test failures.");

GTEST_DEFINE_FLAG_string_(
	color,
	internal::StringFromGTestEnv("color", "auto"),
	"Whether to use colors in the output.  Valid values: yes, no, "
	"and auto.  'auto' means to use colors if the output is "
	"being sent to a terminal and the TERM environment variable "
	"is set to a terminal type that supports colors.");

GTEST_DEFINE_FLAG_string_(
	filter,
	internal::StringFromGTestEnv("filter", ::jmsd::cutf::function_Get_default_filter::GetDefaultFilter()),
	"A colon-separated list of glob (not regex) patterns "
	"for filtering the tests to run, optionally followed by a "
	"'-' and a : separated list of negative patterns (tests to "
	"exclude).  A test is run if it matches one of the positive "
	"patterns and does not match any of the negative patterns.");

GTEST_DEFINE_FLAG_bool_(
	install_failure_signal_handler,
	internal::BoolFromGTestEnv("install_failure_signal_handler", false),
	"If true and supported on the current platform, " GTEST_NAME_ " should "
	"install a signal handler that dumps debugging information when fatal "
	"signals are raised.");

GTEST_DEFINE_FLAG_bool_(list_tests, false,
				   "List all tests without running them.");

// The net priority order after flag processing is thus:
//   --gtest_output command line flag
//   GTEST_OUTPUT environment variable
//   XML_OUTPUT_FILE environment variable
//   ''
GTEST_DEFINE_FLAG_string_(
	output,
	internal::StringFromGTestEnv("output",
	  internal::OutputFlagAlsoCheckEnvVar().c_str()),
	"A format (defaults to \"xml\" but can be specified to be \"json\"), "
	"optionally followed by a colon and an output file name or directory. "
	"A directory is indicated by a trailing pathname separator. "
	"Examples: \"xml:filename.xml\", \"xml::directoryname/\". "
	"If a directory is specified, output files will be created "
	"within that directory, with file-names based on the test "
	"executable's name and, if necessary, made unique by adding "
	"digits.");

GTEST_DEFINE_FLAG_bool_(print_time, internal::BoolFromGTestEnv("print_time", true),
				   "True if and only if " GTEST_NAME_
				   " should display elapsed time in text output.");

GTEST_DEFINE_FLAG_bool_(print_utf8, internal::BoolFromGTestEnv("print_utf8", true),
				   "True if and only if " GTEST_NAME_
				   " prints UTF8 characters as text.");

GTEST_DEFINE_FLAG_int32_(
	random_seed,
	internal::Int32FromGTestEnv("random_seed", 0),
	"Random number seed to use when shuffling test orders.  Must be in range "
	"[1, 99999], or 0 to use a seed based on the current time.");

GTEST_DEFINE_FLAG_int32_(
	repeat,
	internal::Int32FromGTestEnv("repeat", 1),
	"How many times to repeat each test.  Specify a negative number "
	"for repeating forever.  Useful for shaking out flaky tests.");

GTEST_DEFINE_FLAG_bool_(show_internal_stack_frames, false,
				   "True if and only if " GTEST_NAME_
				   " should include internal stack frames when "
				   "printing test failure stack traces.");

GTEST_DEFINE_FLAG_bool_(shuffle, internal::BoolFromGTestEnv("shuffle", false),
				   "True if and only if " GTEST_NAME_
				   " should randomize tests' order on every run.");

GTEST_DEFINE_FLAG_int32_(
	stack_trace_depth,
	internal::Int32FromGTestEnv("stack_trace_depth", ::jmsd::cutf::constants::kMaxStackTraceDepth),
	"The maximum number of stack frames to print when an "
	"assertion fails.  The valid range is 0 through 100, inclusive.");

GTEST_DEFINE_FLAG_string_(
	stream_result_to,
	internal::StringFromGTestEnv("stream_result_to", ""),
	"This flag specifies the host name and the port number on which to stream "
	"test results. Example: \"localhost:555\". The flag is effective only on "
	"Linux.");

GTEST_DEFINE_FLAG_bool_(
	throw_on_failure,
	internal::BoolFromGTestEnv("throw_on_failure", false),
	"When this flag is specified, a failed assertion will throw an exception "
	"if exceptions are enabled or exit the program with a non-zero code "
	"otherwise. For use with an external test framework.");

#if GTEST_USE_OWN_FLAGFILE_FLAG_
GTEST_DEFINE_FLAG_string_(
	flagfile,
	internal::StringFromGTestEnv("flagfile", ""),
	"This flag specifies the flagfile to read command-line flags from.");
#endif  // GTEST_USE_OWN_FLAGFILE_FLAG_



} // namespace testing
