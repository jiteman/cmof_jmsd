// This file implements death tests.

#include "gtest/gtest-death-test.h"

#include "gtest/internal/custom/gtest.h"

#include "internal/Unit_test_impl.h"
#include "internal/Death_test_impl.h"
#include "internal/Death_test_check.h"
#include "internal/Get_last_errno_description.h"


#include "internal/gtest-port.h"


#if GTEST_OS_WINDOWS

#include "internal/Windows_death_test.h"

#endif // #if GTEST_OS_WINDOWS


#include "internal/Death_test_check.hxx"


#include <utility>


#if GTEST_HAS_DEATH_TEST

# if GTEST_OS_MAC
#  include <crt_externs.h>
# endif  // GTEST_OS_MAC

# include <errno.h>
# include <fcntl.h>
# include <limits.h>

# if GTEST_OS_LINUX
#  include <signal.h>
# endif  // GTEST_OS_LINUX

# include <stdarg.h>

# if GTEST_OS_WINDOWS
#  include "internal/custom/temporary_windows_include.h"
# else
#  include <sys/mman.h>
#  include <sys/wait.h>
# endif  // GTEST_OS_WINDOWS

# if GTEST_OS_QNX
#  include <spawn.h>
# endif  // GTEST_OS_QNX

# if GTEST_OS_FUCHSIA
#  include <lib/fdio/fd.h>
#  include <lib/fdio/io.h>
#  include <lib/fdio/spawn.h>
#  include <lib/zx/channel.h>
#  include <lib/zx/port.h>
#  include <lib/zx/process.h>
#  include <lib/zx/socket.h>
#  include <zircon/processargs.h>
#  include <zircon/syscalls.h>
#  include <zircon/syscalls/policy.h>
#  include <zircon/syscalls/port.h>
# endif  // GTEST_OS_FUCHSIA

#endif  // GTEST_HAS_DEATH_TEST

#include "gtest/internal/gtest-string.h"
#include "gtest/gtest-internal-inl.h"

namespace testing {

// Constants.

// The default death test style.
//
// This is defined in internal/gtest-port.h as "fast", but can be overridden by
// a definition in internal/custom/gtest-port.h. The recommended value, which is
// used internally at Google, is "threadsafe".
static const char kDefaultDeathTestStyle[] = GTEST_DEFAULT_DEATH_TEST_STYLE;

GTEST_DEFINE_FLAG_string_(
	death_test_style,
	internal::StringFromGTestEnv("death_test_style", kDefaultDeathTestStyle),
	"Indicates how to run a death test in a forked child process: "
	"\"threadsafe\" (child process re-executes the test binary "
	"from the beginning, running only the specific death test) or "
	"\"fast\" (child process runs the death test immediately "
	"after forking).");

GTEST_DEFINE_FLAG_bool_(
	death_test_use_fork,
	internal::BoolFromGTestEnv("death_test_use_fork", false),
	"Instructs to use fork()/_exit() instead of clone() in death tests. "
	"Ignored and always uses fork() on POSIX systems where clone() is not "
	"implemented. Useful when running under valgrind or similar tools if "
	"those do not support clone(). Valgrind 3.3.1 will just fail if "
	"it sees an unsupported combination of clone() flags. "
	"It is not recommended to use this flag w/o valgrind though it will "
	"work in 99% of the cases. Once valgrind is fixed, this flag will "
	"most likely be removed.");

namespace internal {
GTEST_DEFINE_FLAG_string_(
	internal_run_death_test, "",
	"Indicates the file, line number, temporal index of "
	"the single death test to run, and a file descriptor to "
	"which a success code may be sent, all separated by "
	"the '|' characters.  This flag is specified if and only if the "
	"current process is a sub-process launched for running a thread-safe "
	"death test.  FOR INTERNAL USE ONLY.");
}  // namespace internal

#if GTEST_HAS_DEATH_TEST

namespace internal {

// Valid only for fast death tests. Indicates the code is running in the
// child process of a fast style death test.
# if !GTEST_OS_WINDOWS && !GTEST_OS_FUCHSIA
static bool g_in_fast_death_test_child = false;
# endif

// Returns a Boolean value indicating whether the caller is currently
// executing in the context of the death test child process.  Tools such as
// Valgrind heap checkers may need this to modify their behavior in death
// tests.  IMPORTANT: This is an internal utility.  Using it may break the
// implementation of death tests.  User code MUST NOT use it.
bool InDeathTestChild() {
# if GTEST_OS_WINDOWS || GTEST_OS_FUCHSIA

  // On Windows and Fuchsia, death tests are thread-safe regardless of the value
  // of the death_test_style flag.
  return !GTEST_FLAG(internal_run_death_test).empty();

# else

  if (GTEST_FLAG(death_test_style) == "threadsafe")
	return !GTEST_FLAG(internal_run_death_test).empty();
  else
	return g_in_fast_death_test_child;
#endif
}

}  // namespace internal

// ExitedWithCode constructor.
ExitedWithCode::ExitedWithCode(int exit_code) : exit_code_(exit_code) {
}

// ExitedWithCode function-call operator.
bool ExitedWithCode::operator()(int exit_status) const {
# if GTEST_OS_WINDOWS || GTEST_OS_FUCHSIA

  return exit_status == exit_code_;

# else

  return WIFEXITED(exit_status) && WEXITSTATUS(exit_status) == exit_code_;

# endif  // GTEST_OS_WINDOWS || GTEST_OS_FUCHSIA
}

# if !GTEST_OS_WINDOWS && !GTEST_OS_FUCHSIA
// KilledBySignal constructor.
KilledBySignal::KilledBySignal(int signum) : signum_(signum) {
}

// KilledBySignal function-call operator.
bool KilledBySignal::operator()(int exit_status) const {
#  if defined(GTEST_KILLED_BY_SIGNAL_OVERRIDE_)
  {
	bool result;
	if (GTEST_KILLED_BY_SIGNAL_OVERRIDE_(signum_, exit_status, &result)) {
	  return result;
	}
  }
#  endif  // defined(GTEST_KILLED_BY_SIGNAL_OVERRIDE_)
  return WIFSIGNALED(exit_status) && WTERMSIG(exit_status) == signum_;
}
# endif  // !GTEST_OS_WINDOWS && !GTEST_OS_FUCHSIA

namespace internal {

// Utilities needed for death tests.

// Returns true if exit_status describes a process that was terminated
// by a signal, or exited normally with a nonzero exit code.
bool ExitedUnsuccessfully(int exit_status) {
  return !ExitedWithCode(0)(exit_status);
}

# if !GTEST_OS_WINDOWS && !GTEST_OS_FUCHSIA
// Generates a textual failure message when a death test finds more than
// one thread running, or cannot determine the number of threads, prior
// to executing the given statement.  It is the responsibility of the
// caller not to pass a thread_count of 1.
static std::string DeathTestThreadWarning(size_t thread_count) {
  Message msg;
  msg << "Death tests use fork(), which is unsafe particularly"
	  << " in a threaded context. For this test, " << GTEST_NAME_ << " ";
  if (thread_count == 0) {
	msg << "couldn't detect the number of threads.";
  } else {
	msg << "detected " << thread_count << " threads.";
  }
  msg << " See "
		 "https://github.com/google/googletest/blob/master/googletest/docs/"
		 "advanced.md#death-tests-and-threads"
	  << " for more explanation and suggested solutions, especially if"
	  << " this is the last message you see before your test times out.";
  return msg.GetString();
}
# endif  // !GTEST_OS_WINDOWS && !GTEST_OS_FUCHSIA

#if GTEST_OS_FUCHSIA

// File descriptor used for the pipe in the child process.
static const int kFuchsiaReadPipeFd = 3;

#endif


// Death test constructor.  Increments the running death test count
// for the current test.
DeathTest::DeathTest() {
  ::jmsd::cutf::TestInfo *const info = ::jmsd::cutf::internal::GetUnitTestImpl()->current_test_info();

  if (info == nullptr) {
	::jmsd::cutf::internal::DeathTestAbort("Cannot run a death test outside of a TEST or "
				   "TEST_F construct");
  }
}

// Creates and returns a death test by dispatching to the current
// death test factory.
bool DeathTest::Create(const char* statement,
					   Matcher<const std::string&> matcher, const char* file,
					   int line, DeathTest** test) {
  return ::jmsd::cutf::internal::GetUnitTestImpl()->death_test_factory()->Create(
	  statement, std::move(matcher), file, line, test);
}

const char* DeathTest::LastMessage() {
  return last_death_test_message_.c_str();
}

void DeathTest::set_last_death_test_message(const std::string& message) {
  last_death_test_message_ = message;
}

std::string DeathTest::last_death_test_message_;

#if GTEST_OS_FUCHSIA

class FuchsiaDeathTest : public DeathTestImpl {
 public:
  FuchsiaDeathTest(const char* a_statement, Matcher<const std::string&> matcher,
				   const char* file, int line)
	  : DeathTestImpl(a_statement, std::move(matcher)),
		file_(file),
		line_(line) {}

  // All of these virtual functions are inherited from DeathTest.
  int Wait() override;
  TestRole AssumeRole() override;
  std::string GetErrorLogs() override;

 private:
  // The name of the file in which the death test is located.
  const char* const file_;
  // The line number on which the death test is located.
  const int line_;
  // The stderr data captured by the child process.
  std::string captured_stderr_;

  zx::process child_process_;
  zx::channel exception_channel_;
  zx::socket stderr_socket_;
};

// Utility class for accumulating command-line arguments.
class Arguments {
 public:
  Arguments() { args_.push_back(nullptr); }

  ~Arguments() {
	for (std::vector<char*>::iterator i = args_.begin(); i != args_.end();
		 ++i) {
	  free(*i);
	}
  }
  void AddArgument(const char* argument) {
	args_.insert(args_.end() - 1, posix::StrDup(argument));
  }

  template <typename Str>
  void AddArguments(const ::std::vector<Str>& arguments) {
	for (typename ::std::vector<Str>::const_iterator i = arguments.begin();
		 i != arguments.end();
		 ++i) {
	  args_.insert(args_.end() - 1, posix::StrDup(i->c_str()));
	}
  }
  char* const* Argv() {
	return &args_[0];
  }

  int size() {
	return args_.size() - 1;
  }

 private:
  std::vector<char*> args_;
};

// Waits for the child in a death test to exit, returning its exit
// status, or 0 if no child process exists.  As a side effect, sets the
// outcome data member.
int FuchsiaDeathTest::Wait() {
  const int kProcessKey = 0;
  const int kSocketKey = 1;
  const int kExceptionKey = 2;

  if (!spawned())
	return 0;

  // Create a port to wait for socket/task/exception events.
  zx_status_t status_zx;
  zx::port port;
  status_zx = zx::port::create(0, &port);
  GTEST_DEATH_TEST_CHECK_(status_zx == ZX_OK);

  // Register to wait for the child process to terminate.
  status_zx = child_process_.wait_async(
	  port, kProcessKey, ZX_PROCESS_TERMINATED, ZX_WAIT_ASYNC_ONCE);
  GTEST_DEATH_TEST_CHECK_(status_zx == ZX_OK);

  // Register to wait for the socket to be readable or closed.
  status_zx = stderr_socket_.wait_async(
	  port, kSocketKey, ZX_SOCKET_READABLE | ZX_SOCKET_PEER_CLOSED,
	  ZX_WAIT_ASYNC_ONCE);
  GTEST_DEATH_TEST_CHECK_(status_zx == ZX_OK);

  // Register to wait for an exception.
  status_zx = exception_channel_.wait_async(
	  port, kExceptionKey, ZX_CHANNEL_READABLE, ZX_WAIT_ASYNC_ONCE);
  GTEST_DEATH_TEST_CHECK_(status_zx == ZX_OK);

  bool process_terminated = false;
  bool socket_closed = false;
  do {
	zx_port_packet_t packet = {};
	status_zx = port.wait(zx::time::infinite(), &packet);
	GTEST_DEATH_TEST_CHECK_(status_zx == ZX_OK);

	if (packet.key == kExceptionKey) {
	  // Process encountered an exception. Kill it directly rather than
	  // letting other handlers process the event. We will get a kProcessKey
	  // event when the process actually terminates.
	  status_zx = child_process_.kill();
	  GTEST_DEATH_TEST_CHECK_(status_zx == ZX_OK);
	} else if (packet.key == kProcessKey) {
	  // Process terminated.
	  GTEST_DEATH_TEST_CHECK_(ZX_PKT_IS_SIGNAL_ONE(packet.type));
	  GTEST_DEATH_TEST_CHECK_(packet.signal.observed & ZX_PROCESS_TERMINATED);
	  process_terminated = true;
	} else if (packet.key == kSocketKey) {
	  GTEST_DEATH_TEST_CHECK_(ZX_PKT_IS_SIGNAL_ONE(packet.type));
	  if (packet.signal.observed & ZX_SOCKET_READABLE) {
		// Read data from the socket.
		constexpr size_t kBufferSize = 1024;
		do {
		  size_t old_length = captured_stderr_.length();
		  size_t bytes_read = 0;
		  captured_stderr_.resize(old_length + kBufferSize);
		  status_zx = stderr_socket_.read(
			  0, &captured_stderr_.front() + old_length, kBufferSize,
			  &bytes_read);
		  captured_stderr_.resize(old_length + bytes_read);
		} while (status_zx == ZX_OK);
		if (status_zx == ZX_ERR_PEER_CLOSED) {
		  socket_closed = true;
		} else {
		  GTEST_DEATH_TEST_CHECK_(status_zx == ZX_ERR_SHOULD_WAIT);
		  status_zx = stderr_socket_.wait_async(
			  port, kSocketKey, ZX_SOCKET_READABLE | ZX_SOCKET_PEER_CLOSED,
			  ZX_WAIT_ASYNC_ONCE);
		  GTEST_DEATH_TEST_CHECK_(status_zx == ZX_OK);
		}
	  } else {
		GTEST_DEATH_TEST_CHECK_(packet.signal.observed & ZX_SOCKET_PEER_CLOSED);
		socket_closed = true;
	  }
	}
  } while (!process_terminated && !socket_closed);

  ReadAndInterpretStatusByte();

  zx_info_process_t buffer;
  status_zx = child_process_.get_info(
	  ZX_INFO_PROCESS, &buffer, sizeof(buffer), nullptr, nullptr);
  GTEST_DEATH_TEST_CHECK_(status_zx == ZX_OK);

  GTEST_DEATH_TEST_CHECK_(buffer.exited);
  set_status(buffer.return_code);
  return status();
}

// The AssumeRole process for a Fuchsia death test.  It creates a child
// process with the same executable as the current process to run the
// death test.  The child process is given the --gtest_filter and
// --gtest_internal_run_death_test flags such that it knows to run the
// current death test only.
DeathTest::TestRole FuchsiaDeathTest::AssumeRole() {
  const UnitTestImpl* const impl = GetUnitTestImpl();
  const InternalRunDeathTestFlag* const flag =
	  impl->internal_run_death_test_flag();
  const TestInfo* const info = impl->current_test_info();
  const int death_test_index = info->result()->death_test_count();

  if (flag != nullptr) {
	// ParseInternalRunDeathTestFlag() has performed all the necessary
	// processing.
	set_write_fd(kFuchsiaReadPipeFd);
	return EXECUTE_TEST;
  }

  // Flush the log buffers since the log streams are shared with the child.
  FlushInfoLog();

  // Build the child process command line.
  const std::string filter_flag = std::string("--") + GTEST_FLAG_PREFIX_ +
								  kFilterFlag + "=" + info->test_suite_name() +
								  "." + info->name();
  const std::string internal_flag =
	  std::string("--") + GTEST_FLAG_PREFIX_ + kInternalRunDeathTestFlag + "="
	  + file_ + "|"
	  + StreamableToString(line_) + "|"
	  + StreamableToString(death_test_index);
  Arguments args;
  args.AddArguments(GetInjectableArgvs());
  args.AddArgument(filter_flag.c_str());
  args.AddArgument(internal_flag.c_str());

  // Build the pipe for communication with the child.
  zx_status_t status;
  zx_handle_t child_pipe_handle;
  int child_pipe_fd;
  status = fdio_pipe_half(&child_pipe_fd, &child_pipe_handle);
  GTEST_DEATH_TEST_CHECK_(status == ZX_OK);
  set_read_fd(child_pipe_fd);

  // Set the pipe handle for the child.
  fdio_spawn_action_t spawn_actions[2] = {};
  fdio_spawn_action_t* add_handle_action = &spawn_actions[0];
  add_handle_action->action = FDIO_SPAWN_ACTION_ADD_HANDLE;
  add_handle_action->h.id = PA_HND(PA_FD, kFuchsiaReadPipeFd);
  add_handle_action->h.handle = child_pipe_handle;

  // Create a socket pair will be used to receive the child process' stderr.
  zx::socket stderr_producer_socket;
  status =
	  zx::socket::create(0, &stderr_producer_socket, &stderr_socket_);
  GTEST_DEATH_TEST_CHECK_(status >= 0);
  int stderr_producer_fd = -1;
  status =
	  fdio_fd_create(stderr_producer_socket.release(), &stderr_producer_fd);
  GTEST_DEATH_TEST_CHECK_(status >= 0);

  // Make the stderr socket nonblocking.
  GTEST_DEATH_TEST_CHECK_(fcntl(stderr_producer_fd, F_SETFL, 0) == 0);

  fdio_spawn_action_t* add_stderr_action = &spawn_actions[1];
  add_stderr_action->action = FDIO_SPAWN_ACTION_CLONE_FD;
  add_stderr_action->fd.local_fd = stderr_producer_fd;
  add_stderr_action->fd.target_fd = STDERR_FILENO;

  // Create a child job.
  zx_handle_t child_job = ZX_HANDLE_INVALID;
  status = zx_job_create(zx_job_default(), 0, & child_job);
  GTEST_DEATH_TEST_CHECK_(status == ZX_OK);
  zx_policy_basic_t policy;
  policy.condition = ZX_POL_NEW_ANY;
  policy.policy = ZX_POL_ACTION_ALLOW;
  status = zx_job_set_policy(
	  child_job, ZX_JOB_POL_RELATIVE, ZX_JOB_POL_BASIC, &policy, 1);
  GTEST_DEATH_TEST_CHECK_(status == ZX_OK);

  // Create an exception channel attached to the |child_job|, to allow
  // us to suppress the system default exception handler from firing.
  status =
	  zx_task_create_exception_channel(
		  child_job, 0, exception_channel_.reset_and_get_address());
  GTEST_DEATH_TEST_CHECK_(status == ZX_OK);

  // Spawn the child process.
  status = fdio_spawn_etc(
	  child_job, FDIO_SPAWN_CLONE_ALL, args.Argv()[0], args.Argv(), nullptr,
	  2, spawn_actions, child_process_.reset_and_get_address(), nullptr);
  GTEST_DEATH_TEST_CHECK_(status == ZX_OK);

  set_spawned(true);
  return OVERSEE_TEST;
}

std::string FuchsiaDeathTest::GetErrorLogs() {
  return captured_stderr_;
}

#elif GTEST_OS_WINDOWS

#else  // We are neither on Windows, nor on Fuchsia.

// ForkingDeathTest provides implementations for most of the abstract
// methods of the DeathTest interface.  Only the AssumeRole method is
// left undefined.
class ForkingDeathTest : public ::jmsd::cutf::internal::DeathTestImpl {
 public:
  ForkingDeathTest(const char* statement, Matcher<const std::string&> matcher);

  // All of these virtual functions are inherited from DeathTest.
  int Wait() override;

 protected:
  void set_child_pid(pid_t child_pid) { child_pid_ = child_pid; }

 private:
  // PID of child process during death test; 0 in the child process itself.
  pid_t child_pid_;
};

// Constructs a ForkingDeathTest.
ForkingDeathTest::ForkingDeathTest(const char* a_statement,
								   Matcher<const std::string&> matcher)
	: DeathTestImpl(a_statement, std::move(matcher)), child_pid_(-1) {}

// Waits for the child in a death test to exit, returning its exit
// status, or 0 if no child process exists.  As a side effect, sets the
// outcome data member.
int ForkingDeathTest::Wait() {
  if (!spawned())
	return 0;

  ReadAndInterpretStatusByte();

  int status_value;
  GTEST_DEATH_TEST_CHECK_SYSCALL_(waitpid(child_pid_, &status_value, 0));
  set_status(status_value);
  return status_value;
}

// A concrete death test class that forks, then immediately runs the test
// in the child process.
class NoExecDeathTest : public ForkingDeathTest {
 public:
  NoExecDeathTest(const char* a_statement, Matcher<const std::string&> matcher)
	  : ForkingDeathTest(a_statement, std::move(matcher)) {}
  TestRole AssumeRole() override;
};

// The AssumeRole process for a fork-and-run death test.  It implements a
// straightforward fork, with a simple pipe to transmit the status byte.
DeathTest::TestRole NoExecDeathTest::AssumeRole() {
  const size_t thread_count = GetThreadCount();
  if (thread_count != 1) {
	GTEST_LOG_(WARNING) << DeathTestThreadWarning(thread_count);
  }

  int pipe_fd[2];
  GTEST_DEATH_TEST_CHECK_(pipe(pipe_fd) != -1);

  DeathTest::set_last_death_test_message("");
  CaptureStderr();
  // When we fork the process below, the log file buffers are copied, but the
  // file descriptors are shared.  We flush all log files here so that closing
  // the file descriptors in the child process doesn't throw off the
  // synchronization between descriptors and buffers in the parent process.
  // This is as close to the fork as possible to avoid a race condition in case
  // there are multiple threads running before the death test, and another
  // thread writes to the log file.
  FlushInfoLog();

  const pid_t child_pid = fork();
  GTEST_DEATH_TEST_CHECK_(child_pid != -1);
  set_child_pid(child_pid);
  if (child_pid == 0) {
	GTEST_DEATH_TEST_CHECK_SYSCALL_(close(pipe_fd[0]));
	set_write_fd(pipe_fd[1]);
	// Redirects all logging to stderr in the child process to prevent
	// concurrent writes to the log files.  We capture stderr in the parent
	// process and append the child process' output to a log.
	LogToStderr();
	// Event forwarding to the listeners of event listener API mush be shut
	// down in death test subprocesses.
	::jmsd::cutf::internal::GetUnitTestImpl()->listeners()->SuppressEventForwarding();
	g_in_fast_death_test_child = true;
	return EXECUTE_TEST;
  } else {
	GTEST_DEATH_TEST_CHECK_SYSCALL_(close(pipe_fd[1]));
	set_read_fd(pipe_fd[0]);
	set_spawned(true);
	return OVERSEE_TEST;
  }
}

// A concrete death test class that forks and re-executes the main
// program from the beginning, with command-line flags set that cause
// only this specific death test to be run.
class ExecDeathTest : public ForkingDeathTest {
 public:
  ExecDeathTest(const char* a_statement, Matcher<const std::string&> matcher,
				const char* file, int line)
	  : ForkingDeathTest(a_statement, std::move(matcher)),
		file_(file),
		line_(line) {}
  TestRole AssumeRole() override;

 private:
  static ::std::vector<std::string> GetArgvsForDeathTestChildProcess() {
	::std::vector<std::string> args = GetInjectableArgvs();
#  if defined(GTEST_EXTRA_DEATH_TEST_COMMAND_LINE_ARGS_)
	::std::vector<std::string> extra_args =
		GTEST_EXTRA_DEATH_TEST_COMMAND_LINE_ARGS_();
	args.insert(args.end(), extra_args.begin(), extra_args.end());
#  endif  // defined(GTEST_EXTRA_DEATH_TEST_COMMAND_LINE_ARGS_)
	return args;
  }
  // The name of the file in which the death test is located.
  const char* const file_;
  // The line number on which the death test is located.
  const int line_;
};

// Utility class for accumulating command-line arguments.
class Arguments {
 public:
  Arguments() { args_.push_back(nullptr); }

  ~Arguments() {
	for (std::vector<char*>::iterator i = args_.begin(); i != args_.end();
		 ++i) {
	  free(*i);
	}
  }
  void AddArgument(const char* argument) {
	args_.insert(args_.end() - 1, posix::StrDup(argument));
  }

  template <typename Str>
  void AddArguments(const ::std::vector<Str>& arguments) {
	for (typename ::std::vector<Str>::const_iterator i = arguments.begin();
		 i != arguments.end();
		 ++i) {
	  args_.insert(args_.end() - 1, posix::StrDup(i->c_str()));
	}
  }
  char* const* Argv() {
	return &args_[0];
  }

 private:
  std::vector<char*> args_;
};

// A struct that encompasses the arguments to the child process of a
// threadsafe-style death test process.
struct ExecDeathTestArgs {
  char* const* argv;  // Command-line arguments for the child's call to exec
  int close_fd;       // File descriptor to close; the read end of a pipe
};

#  if GTEST_OS_MAC
inline char** GetEnviron() {
  // When Google Test is built as a framework on MacOS X, the environ variable
  // is unavailable. Apple's documentation (man environ) recommends using
  // _NSGetEnviron() instead.
  return *_NSGetEnviron();
}
#  else
// Some POSIX platforms expect you to declare environ. extern "C" makes
// it reside in the global namespace.
extern "C" char** environ;
inline char** GetEnviron() { return environ; }
#  endif  // GTEST_OS_MAC

#  if !GTEST_OS_QNX
// The main function for a threadsafe-style death test child process.
// This function is called in a clone()-ed process and thus must avoid
// any potentially unsafe operations like malloc or libc functions.
static int ExecDeathTestChildMain(void* child_arg) {
  ExecDeathTestArgs* const args = static_cast<ExecDeathTestArgs*>(child_arg);
  GTEST_DEATH_TEST_CHECK_SYSCALL_(close(args->close_fd));

  // We need to execute the test program in the same environment where
  // it was originally invoked.  Therefore we change to the original
  // working directory first.
  const char* const original_dir =
	  ::jmsd::cutf::UnitTest::GetInstance()->original_working_dir();
  // We can safely call chdir() as it's a direct system call.
  if (chdir(original_dir) != 0) {
	::jmsd::cutf::internal::DeathTestAbort(std::string("chdir(\"") + original_dir + "\") failed: " +
				   ::jmsd::cutf::internal::GetLastErrnoDescription());
	return EXIT_FAILURE;
  }

  // We can safely call execve() as it's a direct system call.  We
  // cannot use execvp() as it's a libc function and thus potentially
  // unsafe.  Since execve() doesn't search the PATH, the user must
  // invoke the test program via a valid path that contains at least
  // one path separator.
  execve(args->argv[0], args->argv, GetEnviron());
  ::jmsd::cutf::internal::DeathTestAbort(std::string("execve(") + args->argv[0] + ", ...) in " +
				 original_dir + " failed: " +
				 ::jmsd::cutf::internal::GetLastErrnoDescription());
  return EXIT_FAILURE;
}
#  endif  // !GTEST_OS_QNX

#  if GTEST_HAS_CLONE
// Two utility routines that together determine the direction the stack
// grows.
// This could be accomplished more elegantly by a single recursive
// function, but we want to guard against the unlikely possibility of
// a smart compiler optimizing the recursion away.
//
// GTEST_NO_INLINE_ is required to prevent GCC 4.6 from inlining
// StackLowerThanAddress into StackGrowsDown, which then doesn't give
// correct answer.
static void StackLowerThanAddress(const void* ptr,
								  bool* result) GTEST_NO_INLINE_;
// HWAddressSanitizer add a random tag to the MSB of the local variable address,
// making comparison result unpredictable.
GTEST_ATTRIBUTE_NO_SANITIZE_HWADDRESS_
static void StackLowerThanAddress(const void* ptr, bool* result) {
  int dummy;
  *result = (&dummy < ptr);
}

// Make sure AddressSanitizer does not tamper with the stack here.
GTEST_ATTRIBUTE_NO_SANITIZE_ADDRESS_
GTEST_ATTRIBUTE_NO_SANITIZE_HWADDRESS_
static bool StackGrowsDown() {
  int dummy;
  bool result;
  StackLowerThanAddress(&dummy, &result);
  return result;
}
#  endif  // GTEST_HAS_CLONE

// Spawns a child process with the same executable as the current process in
// a thread-safe manner and instructs it to run the death test.  The
// implementation uses fork(2) + exec.  On systems where clone(2) is
// available, it is used instead, being slightly more thread-safe.  On QNX,
// fork supports only single-threaded environments, so this function uses
// spawn(2) there instead.  The function dies with an error message if
// anything goes wrong.
static pid_t ExecDeathTestSpawnChild(char* const* argv, int close_fd) {
  ExecDeathTestArgs args = { argv, close_fd };
  pid_t child_pid = -1;

#  if GTEST_OS_QNX
  // Obtains the current directory and sets it to be closed in the child
  // process.
  const int cwd_fd = open(".", O_RDONLY);
  GTEST_DEATH_TEST_CHECK_(cwd_fd != -1);
  GTEST_DEATH_TEST_CHECK_SYSCALL_(fcntl(cwd_fd, F_SETFD, FD_CLOEXEC));
  // We need to execute the test program in the same environment where
  // it was originally invoked.  Therefore we change to the original
  // working directory first.
  const char* const original_dir =
	  UnitTest::GetInstance()->original_working_dir();
  // We can safely call chdir() as it's a direct system call.
  if (chdir(original_dir) != 0) {
	DeathTestAbort(std::string("chdir(\"") + original_dir + "\") failed: " +
				   GetLastErrnoDescription());
	return EXIT_FAILURE;
  }

  int fd_flags;
  // Set close_fd to be closed after spawn.
  GTEST_DEATH_TEST_CHECK_SYSCALL_(fd_flags = fcntl(close_fd, F_GETFD));
  GTEST_DEATH_TEST_CHECK_SYSCALL_(fcntl(close_fd, F_SETFD,
										fd_flags | FD_CLOEXEC));
  struct inheritance inherit = {0};
  // spawn is a system call.
  child_pid =
	  spawn(args.argv[0], 0, nullptr, &inherit, args.argv, GetEnviron());
  // Restores the current working directory.
  GTEST_DEATH_TEST_CHECK_(fchdir(cwd_fd) != -1);
  GTEST_DEATH_TEST_CHECK_SYSCALL_(close(cwd_fd));

#  else   // GTEST_OS_QNX
#   if GTEST_OS_LINUX
  // When a SIGPROF signal is received while fork() or clone() are executing,
  // the process may hang. To avoid this, we ignore SIGPROF here and re-enable
  // it after the call to fork()/clone() is complete.
  struct sigaction saved_sigprof_action;
  struct sigaction ignore_sigprof_action;
  memset(&ignore_sigprof_action, 0, sizeof(ignore_sigprof_action));
  sigemptyset(&ignore_sigprof_action.sa_mask);
  ignore_sigprof_action.sa_handler = SIG_IGN;
  GTEST_DEATH_TEST_CHECK_SYSCALL_(sigaction(
	  SIGPROF, &ignore_sigprof_action, &saved_sigprof_action));
#   endif  // GTEST_OS_LINUX

#   if GTEST_HAS_CLONE
  const bool use_fork = GTEST_FLAG(death_test_use_fork);

  if (!use_fork) {
	static const bool stack_grows_down = StackGrowsDown();
	const auto stack_size = static_cast<size_t>(getpagesize() * 2);
	// MMAP_ANONYMOUS is not defined on Mac, so we use MAP_ANON instead.
	void* const stack = mmap(nullptr, stack_size, PROT_READ | PROT_WRITE,
							 MAP_ANON | MAP_PRIVATE, -1, 0);
	GTEST_DEATH_TEST_CHECK_(stack != MAP_FAILED);

	// Maximum stack alignment in bytes:  For a downward-growing stack, this
	// amount is subtracted from size of the stack space to get an address
	// that is within the stack space and is aligned on all systems we care
	// about.  As far as I know there is no ABI with stack alignment greater
	// than 64.  We assume stack and stack_size already have alignment of
	// kMaxStackAlignment.
	const size_t kMaxStackAlignment = 64;
	void* const stack_top =
		static_cast<char*>(stack) +
			(stack_grows_down ? stack_size - kMaxStackAlignment : 0);
	GTEST_DEATH_TEST_CHECK_(
		static_cast<size_t>(stack_size) > kMaxStackAlignment &&
		reinterpret_cast<uintptr_t>(stack_top) % kMaxStackAlignment == 0);

	child_pid = clone(&ExecDeathTestChildMain, stack_top, SIGCHLD, &args);

	GTEST_DEATH_TEST_CHECK_(munmap(stack, stack_size) != -1);
  }
#   else
  const bool use_fork = true;
#   endif  // GTEST_HAS_CLONE

  if (use_fork && (child_pid = fork()) == 0) {
	  ExecDeathTestChildMain(&args);
	  _exit(0);
  }
#  endif  // GTEST_OS_QNX
#  if GTEST_OS_LINUX
  GTEST_DEATH_TEST_CHECK_SYSCALL_(
	  sigaction(SIGPROF, &saved_sigprof_action, nullptr));
#  endif  // GTEST_OS_LINUX

  GTEST_DEATH_TEST_CHECK_(child_pid != -1);
  return child_pid;
}

// The AssumeRole process for a fork-and-exec death test.  It re-executes the
// main program from the beginning, setting the --gtest_filter
// and --gtest_internal_run_death_test flags to cause only the current
// death test to be re-run.
DeathTest::TestRole ExecDeathTest::AssumeRole() {
  const ::jmsd::cutf::internal::UnitTestImpl* const impl = ::jmsd::cutf::internal::GetUnitTestImpl();
  const InternalRunDeathTestFlag* const flag =
	  impl->internal_run_death_test_flag();
  const ::jmsd::cutf::TestInfo* const info = impl->current_test_info();
  const int death_test_index = info->result()->death_test_count();

  if (flag != nullptr) {
	set_write_fd(flag->write_fd());
	return EXECUTE_TEST;
  }

  int pipe_fd[2];
  GTEST_DEATH_TEST_CHECK_(pipe(pipe_fd) != -1);
  // Clear the close-on-exec flag on the write end of the pipe, lest
  // it be closed when the child process does an exec:
  GTEST_DEATH_TEST_CHECK_(fcntl(pipe_fd[1], F_SETFD, 0) != -1);

  const std::string filter_flag = std::string("--") + GTEST_FLAG_PREFIX_ +
								  kFilterFlag + "=" + info->test_suite_name() +
								  "." + info->name();
  const std::string internal_flag =
	  std::string("--") + GTEST_FLAG_PREFIX_ + kInternalRunDeathTestFlag + "="
	  + file_ + "|" + ::jmsd::cutf::internal::StreamableToString(line_) + "|"
	  + ::jmsd::cutf::internal::StreamableToString(death_test_index) + "|"
	  + ::jmsd::cutf::internal::StreamableToString(pipe_fd[1]);
  Arguments args;
  args.AddArguments(GetArgvsForDeathTestChildProcess());
  args.AddArgument(filter_flag.c_str());
  args.AddArgument(internal_flag.c_str());

  DeathTest::set_last_death_test_message("");

  CaptureStderr();
  // See the comment in NoExecDeathTest::AssumeRole for why the next line
  // is necessary.
  FlushInfoLog();

  const pid_t child_pid = ExecDeathTestSpawnChild(args.Argv(), pipe_fd[0]);
  GTEST_DEATH_TEST_CHECK_SYSCALL_(close(pipe_fd[1]));
  set_child_pid(child_pid);
  set_read_fd(pipe_fd[0]);
  set_spawned(true);
  return OVERSEE_TEST;
}

# endif  // !GTEST_OS_WINDOWS

// Creates a concrete DeathTest-derived class that depends on the
// --gtest_death_test_style flag, and sets the pointer pointed to
// by the "test" argument to its address.  If the test should be
// skipped, sets that pointer to NULL.  Returns true, unless the
// flag is set to an invalid value.
bool DefaultDeathTestFactory::Create(const char* statement,
									 Matcher<const std::string&> matcher,
									 const char* file, int line,
									 DeathTest** test) {
  ::jmsd::cutf::internal::UnitTestImpl* const impl = ::jmsd::cutf::internal::GetUnitTestImpl();
  const InternalRunDeathTestFlag* const flag =
	  impl->internal_run_death_test_flag();
  const int death_test_index = impl->current_test_info()
	  ->increment_death_test_count();

  if (flag != nullptr) {
	if (death_test_index > flag->index()) {
	  DeathTest::set_last_death_test_message(
		  "Death test count (" + ::jmsd::cutf::internal::StreamableToString(death_test_index)
		  + ") somehow exceeded expected maximum ("
		  + ::jmsd::cutf::internal::StreamableToString(flag->index()) + ")");
	  return false;
	}

	if (!(flag->file() == file && flag->line() == line &&
		  flag->index() == death_test_index)) {
	  *test = nullptr;
	  return true;
	}
  }

# if GTEST_OS_WINDOWS

  if (GTEST_FLAG(death_test_style) == "threadsafe" ||
	  GTEST_FLAG(death_test_style) == "fast") {
	*test = new ::jmsd::cutf::internal::WindowsDeathTest(statement, std::move(matcher), file, line);
  }

# elif GTEST_OS_FUCHSIA

  if (GTEST_FLAG(death_test_style) == "threadsafe" ||
	  GTEST_FLAG(death_test_style) == "fast") {
	*test = new FuchsiaDeathTest(statement, std::move(matcher), file, line);
  }

# else

  if (GTEST_FLAG(death_test_style) == "threadsafe") {
	*test = new ExecDeathTest(statement, std::move(matcher), file, line);
  } else if (GTEST_FLAG(death_test_style) == "fast") {
	*test = new NoExecDeathTest(statement, std::move(matcher));
  }

# endif  // GTEST_OS_WINDOWS

  else {  // NOLINT - this is more readable than unbalanced brackets inside #if.
	DeathTest::set_last_death_test_message(
		"Unknown death test style \"" + GTEST_FLAG(death_test_style)
		+ "\" encountered");
	return false;
  }

  return true;
}

# if GTEST_OS_WINDOWS
// Recreates the pipe and event handles from the provided parameters,
// signals the event, and returns a file descriptor wrapped around the pipe
// handle. This function is called in the child process only.
static int GetStatusFileDescriptor(unsigned int parent_process_id,
							size_t write_handle_as_size_t,
							size_t event_handle_as_size_t) {
  AutoHandle parent_process_handle(::OpenProcess(PROCESS_DUP_HANDLE,
												   FALSE,  // Non-inheritable.
												   parent_process_id));
  if (parent_process_handle.Get() == INVALID_HANDLE_VALUE) {
	::jmsd::cutf::internal::DeathTestAbort("Unable to open parent process " + ::jmsd::cutf::internal::StreamableToString(parent_process_id));
  }

  GTEST_CHECK_(sizeof(HANDLE) <= sizeof(size_t));

  const HANDLE write_handle =
	  reinterpret_cast<HANDLE>(write_handle_as_size_t);
  HANDLE dup_write_handle;

  // The newly initialized handle is accessible only in the parent
  // process. To obtain one accessible within the child, we need to use
  // DuplicateHandle.
  if (!::DuplicateHandle(parent_process_handle.Get(), write_handle,
						 ::GetCurrentProcess(), &dup_write_handle,
						 0x0,    // Requested privileges ignored since
								 // DUPLICATE_SAME_ACCESS is used.
						 FALSE,  // Request non-inheritable handler.
						 DUPLICATE_SAME_ACCESS)) {
	::jmsd::cutf::internal::DeathTestAbort("Unable to duplicate the pipe handle " +
				   ::jmsd::cutf::internal::StreamableToString(write_handle_as_size_t) +
				   " from the parent process " +
				   ::jmsd::cutf::internal::StreamableToString(parent_process_id));
  }

  const HANDLE event_handle = reinterpret_cast<HANDLE>(event_handle_as_size_t);
  HANDLE dup_event_handle;

  if (!::DuplicateHandle(parent_process_handle.Get(), event_handle,
						 ::GetCurrentProcess(), &dup_event_handle,
						 0x0,
						 FALSE,
						 DUPLICATE_SAME_ACCESS)) {
	::jmsd::cutf::internal::DeathTestAbort("Unable to duplicate the event handle " +
				   ::jmsd::cutf::internal::StreamableToString(event_handle_as_size_t) +
				   " from the parent process " +
				   ::jmsd::cutf::internal::StreamableToString(parent_process_id));
  }

  const int write_fd =
	  ::_open_osfhandle(reinterpret_cast<intptr_t>(dup_write_handle), O_APPEND);
  if (write_fd == -1) {
	::jmsd::cutf::internal::DeathTestAbort("Unable to convert pipe handle " +
				   ::jmsd::cutf::internal::StreamableToString(write_handle_as_size_t) +
				   " to a file descriptor");
  }

  // Signals the parent that the write end of the pipe has been acquired
  // so the parent can release its own write end.
  ::SetEvent(dup_event_handle);

  return write_fd;
}
# endif  // GTEST_OS_WINDOWS

// Returns a newly created InternalRunDeathTestFlag object with fields
// initialized from the GTEST_FLAG(internal_run_death_test) flag if
// the flag is specified; otherwise returns NULL.
InternalRunDeathTestFlag* ParseInternalRunDeathTestFlag() {
  if (GTEST_FLAG(internal_run_death_test) == "") return nullptr;

  // GTEST_HAS_DEATH_TEST implies that we have ::std::string, so we
  // can use it here.
  int line = -1;
  int index = -1;
  ::std::vector< ::std::string> fields;
  SplitString(GTEST_FLAG(internal_run_death_test).c_str(), '|', &fields);
  int write_fd = -1;

# if GTEST_OS_WINDOWS

  unsigned int parent_process_id = 0;
  size_t write_handle_as_size_t = 0;
  size_t event_handle_as_size_t = 0;

  if (fields.size() != 6
	  || !ParseNaturalNumber(fields[1], &line)
	  || !ParseNaturalNumber(fields[2], &index)
	  || !ParseNaturalNumber(fields[3], &parent_process_id)
	  || !ParseNaturalNumber(fields[4], &write_handle_as_size_t)
	  || !ParseNaturalNumber(fields[5], &event_handle_as_size_t)) {
	::jmsd::cutf::internal::DeathTestAbort("Bad --gtest_internal_run_death_test flag: " +
				   GTEST_FLAG(internal_run_death_test));
  }
  write_fd = GetStatusFileDescriptor(parent_process_id,
									 write_handle_as_size_t,
									 event_handle_as_size_t);

# elif GTEST_OS_FUCHSIA

  if (fields.size() != 3
	  || !ParseNaturalNumber(fields[1], &line)
	  || !ParseNaturalNumber(fields[2], &index)) {
	DeathTestAbort("Bad --gtest_internal_run_death_test flag: "
		+ GTEST_FLAG(internal_run_death_test));
  }

# else

  if (fields.size() != 4
	  || !ParseNaturalNumber(fields[1], &line)
	  || !ParseNaturalNumber(fields[2], &index)
	  || !ParseNaturalNumber(fields[3], &write_fd)) {
	::jmsd::cutf::internal::DeathTestAbort("Bad --gtest_internal_run_death_test flag: "
		+ GTEST_FLAG(internal_run_death_test));
  }

# endif  // GTEST_OS_WINDOWS

  return new InternalRunDeathTestFlag(fields[0], line, index, write_fd);
}

}  // namespace internal

#endif  // GTEST_HAS_DEATH_TEST

}  // namespace testing
