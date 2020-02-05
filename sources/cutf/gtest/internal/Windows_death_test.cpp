#include "Windows_death_test.h"


# if GTEST_OS_WINDOWS

#include "Unit_test_impl.h"
#include "Death_test_check.h"

#include "gtest/Message.hin"

#include "custom/temporary_windows_include.h"
#include <fcntl.h>
//#include <stdio.h>


namespace jmsd {
namespace cutf {
namespace internal {


// Waits for the child in a death test to exit, returning its exit
// status, or 0 if no child process exists.  As a side effect, sets the
// outcome data member.
int WindowsDeathTest::Wait() {
  if (!spawned())
	return 0;

  // Wait until the child either signals that it has acquired the write end
  // of the pipe or it dies.
  const HANDLE wait_handles[2] = { child_handle_.Get(), event_handle_.Get() };
  switch (::WaitForMultipleObjects(2,
								   wait_handles,
								   FALSE,  // Waits for any of the handles.
								   INFINITE)) {
	case WAIT_OBJECT_0:
	case WAIT_OBJECT_0 + 1:
	  break;
	default:
	  GTEST_DEATH_TEST_CHECK_(false);  // Should not get here.
  }

  // The child has acquired the write end of the pipe or exited.
  // We release the handle on our side and continue.
  write_handle_.Reset();
  event_handle_.Reset();

  ReadAndInterpretStatusByte();

  // Waits for the child process to exit if it haven't already. This
  // returns immediately if the child has already exited, regardless of
  // whether previous calls to WaitForMultipleObjects synchronized on this
  // handle or not.
  GTEST_DEATH_TEST_CHECK_(
	  WAIT_OBJECT_0 == ::WaitForSingleObject(child_handle_.Get(),
											 INFINITE));
  DWORD status_code;
  GTEST_DEATH_TEST_CHECK_(
	  ::GetExitCodeProcess(child_handle_.Get(), &status_code) != FALSE);
  child_handle_.Reset();
  set_status(static_cast<int>(status_code));
  return status();
}

// The AssumeRole process for a Windows death test.  It creates a child
// process with the same executable as the current process to run the
// death test.  The child process is given the --gtest_filter and
// --gtest_internal_run_death_test flags such that it knows to run the
// current death test only.
::testing::internal::DeathTest::TestRole WindowsDeathTest::AssumeRole() {
  const UnitTestImpl* const impl = GetUnitTestImpl();
  const ::testing::internal::InternalRunDeathTestFlag* const flag =
	  impl->internal_run_death_test_flag();
  const TestInfo* const info = impl->current_test_info();
  const int death_test_index = info->result()->death_test_count();

  if (flag != nullptr) {
	// ParseInternalRunDeathTestFlag() has performed all the necessary
	// processing.
	set_write_fd(flag->write_fd());
	return EXECUTE_TEST;
  }

  // WindowsDeathTest uses an anonymous pipe to communicate results of
  // a death test.
  SECURITY_ATTRIBUTES handles_are_inheritable = {sizeof(SECURITY_ATTRIBUTES),
												 nullptr, TRUE};
  HANDLE read_handle, write_handle;
  GTEST_DEATH_TEST_CHECK_(
	  ::CreatePipe(&read_handle, &write_handle, &handles_are_inheritable,
				   0)  // Default buffer size.
	  != FALSE);
  set_read_fd(::_open_osfhandle(reinterpret_cast<intptr_t>(read_handle),
								_O_RDONLY));
  write_handle_.Reset(write_handle);
  event_handle_.Reset(::CreateEvent(
	  &handles_are_inheritable,
	  TRUE,       // The event will automatically reset to non-signaled state.
	  FALSE,      // The initial state is non-signalled.
	  nullptr));  // The even is unnamed.
  GTEST_DEATH_TEST_CHECK_(event_handle_.Get() != nullptr);
  const std::string filter_flag = std::string("--") + GTEST_FLAG_PREFIX_ +
								  ::testing::internal::kFilterFlag + "=" + info->test_suite_name() +
								  "." + info->name();
  const std::string internal_flag =
	  std::string("--") + GTEST_FLAG_PREFIX_ + ::testing::internal::kInternalRunDeathTestFlag +
	  "=" + file_ + "|" + function_Streamable_to_string::StreamableToString(line_) + "|" +
	  function_Streamable_to_string::StreamableToString(death_test_index) + "|" +
	  function_Streamable_to_string::StreamableToString(static_cast<unsigned int>(::GetCurrentProcessId())) +
	  // size_t has the same width as pointers on both 32-bit and 64-bit
	  // Windows platforms.
	  // See http://msdn.microsoft.com/en-us/library/tcxf1dw6.aspx.
	  "|" + function_Streamable_to_string::StreamableToString(reinterpret_cast<size_t>(write_handle)) +
	  "|" + function_Streamable_to_string::StreamableToString(reinterpret_cast<size_t>(event_handle_.Get()));

  char executable_path[_MAX_PATH + 1];  // NOLINT
  GTEST_DEATH_TEST_CHECK_(_MAX_PATH + 1 != ::GetModuleFileNameA(nullptr,
																executable_path,
																_MAX_PATH));

  std::string command_line =
	  std::string(::GetCommandLineA()) + " " + filter_flag + " \"" +
	  internal_flag + "\"";

  DeathTest::set_last_death_test_message("");

  ::testing::internal::CaptureStderr();
  // Flush the log buffers since the log streams are shared with the child.
  ::testing::internal::FlushInfoLog();

  // The child process will share the standard handles with the parent.
  STARTUPINFOA startup_info;
  memset(&startup_info, 0, sizeof(STARTUPINFO));
  startup_info.dwFlags = STARTF_USESTDHANDLES;
  startup_info.hStdInput = ::GetStdHandle(STD_INPUT_HANDLE);
  startup_info.hStdOutput = ::GetStdHandle(STD_OUTPUT_HANDLE);
  startup_info.hStdError = ::GetStdHandle(STD_ERROR_HANDLE);

  PROCESS_INFORMATION process_info;
  GTEST_DEATH_TEST_CHECK_(
	  ::CreateProcessA(
		  executable_path, const_cast<char*>(command_line.c_str()),
		  nullptr,  // Retuned process handle is not inheritable.
		  nullptr,  // Retuned thread handle is not inheritable.
		  TRUE,  // Child inherits all inheritable handles (for write_handle_).
		  0x0,   // Default creation flags.
		  nullptr,  // Inherit the parent's environment.
		  ::jmsd::cutf::UnitTest::GetInstance()->original_working_dir(), &startup_info,
		  &process_info) != FALSE);
  child_handle_.Reset(process_info.hProcess);
  ::CloseHandle(process_info.hThread);
  set_spawned(true);
  return OVERSEE_TEST;
}


} // namespace internal
} // namespace cutf
} // namespace jmsd


#endif // # if GTEST_OS_WINDOWS

namespace testing {


} // namespace testing
