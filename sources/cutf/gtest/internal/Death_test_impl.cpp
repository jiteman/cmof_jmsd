#include "Death_test_impl.h"


namespace jmsd {
namespace cutf {
namespace internal {



// Called in the parent process only. Reads the result code of the death
// test child process via a pipe, interprets it to set the outcome_
// member, and closes read_fd_.  Outputs diagnostics and terminates in
// case of unexpected codes.
void DeathTestImpl::ReadAndInterpretStatusByte() {
  char flag;
  int bytes_read;

  // The read() here blocks until data is available (signifying the
  // failure of the death test) or until the pipe is closed (signifying
  // its success), so it's okay to call this in the parent before
  // the child process has exited.
  do {
	bytes_read = posix::Read(read_fd(), &flag, 1);
  } while (bytes_read == -1 && errno == EINTR);

  if (bytes_read == 0) {
	set_outcome(DIED);
  } else if (bytes_read == 1) {
	switch (flag) {
	  case kDeathTestReturned:
		set_outcome(RETURNED);
		break;
	  case kDeathTestThrew:
		set_outcome(THREW);
		break;
	  case kDeathTestLived:
		set_outcome(LIVED);
		break;
	  case kDeathTestInternalError:
		FailFromInternalError(read_fd());  // Does not return.
		break;
	  default:
		GTEST_LOG_(FATAL) << "Death test child process reported "
						  << "unexpected status byte ("
						  << static_cast<unsigned int>(flag) << ")";
	}
  } else {
	GTEST_LOG_(FATAL) << "Read from death test child process failed: "
					  << GetLastErrnoDescription();
  }
  GTEST_DEATH_TEST_CHECK_SYSCALL_(posix::Close(read_fd()));
  set_read_fd(-1);
}

std::string DeathTestImpl::GetErrorLogs() {
  return GetCapturedStderr();
}

// Signals that the death test code which should have exited, didn't.
// Should be called only in a death test child process.
// Writes a status byte to the child's status file descriptor, then
// calls _exit(1).
void DeathTestImpl::Abort(AbortReason reason) {
  // The parent process considers the death test to be a failure if
  // it finds any data in our pipe.  So, here we write a single flag byte
  // to the pipe, then exit.
  const char status_ch =
	  reason == TEST_DID_NOT_DIE ? kDeathTestLived :
	  reason == TEST_THREW_EXCEPTION ? kDeathTestThrew : kDeathTestReturned;

  GTEST_DEATH_TEST_CHECK_SYSCALL_(posix::Write(write_fd(), &status_ch, 1));
  // We are leaking the descriptor here because on some platforms (i.e.,
  // when built as Windows DLL), destructors of global objects will still
  // run after calling _exit(). On such systems, write_fd_ will be
  // indirectly closed from the destructor of UnitTestImpl, causing double
  // close if it is also closed here. On debug configurations, double close
  // may assert. As there are no in-process buffers to flush here, we are
  // relying on the OS to close the descriptor after the process terminates
  // when the destructors are not run.
  _exit(1);  // Exits w/o any normal exit hooks (we were supposed to crash)
}

// Returns an indented copy of stderr output for a death test.
// This makes distinguishing death test output lines from regular log lines
// much easier.
static ::std::string FormatDeathTestOutput(const ::std::string& output) {
  ::std::string ret;
  for (size_t at = 0; ; ) {
	const size_t line_end = output.find('\n', at);
	ret += "[  DEATH   ] ";
	if (line_end == ::std::string::npos) {
	  ret += output.substr(at);
	  break;
	}
	ret += output.substr(at, line_end + 1 - at);
	at = line_end + 1;
  }
  return ret;
}

// Assesses the success or failure of a death test, using both private
// members which have previously been set, and one argument:
//
// Private data members:
//   outcome:  An enumeration describing how the death test
//             concluded: DIED, LIVED, THREW, or RETURNED.  The death test
//             fails in the latter three cases.
//   status:   The exit status of the child process. On *nix, it is in the
//             in the format specified by wait(2). On Windows, this is the
//             value supplied to the ExitProcess() API or a numeric code
//             of the exception that terminated the program.
//   matcher_: A matcher that's expected to match the stderr output by the child
//             process.
//
// Argument:
//   status_ok: true if exit_status is acceptable in the context of
//              this particular death test, which fails if it is false
//
// Returns true if and only if all of the above conditions are met.  Otherwise,
// the first failing condition, in the order given above, is the one that is
// reported. Also sets the last death test message string.
bool DeathTestImpl::Passed(bool status_ok) {
  if (!spawned())
	return false;

  const std::string error_message = GetErrorLogs();

  bool success = false;
  Message buffer;

  buffer << "Death test: " << statement() << "\n";
  switch (outcome()) {
	case LIVED:
	  buffer << "    Result: failed to die.\n"
			 << " Error msg:\n" << FormatDeathTestOutput(error_message);
	  break;
	case THREW:
	  buffer << "    Result: threw an exception.\n"
			 << " Error msg:\n" << FormatDeathTestOutput(error_message);
	  break;
	case RETURNED:
	  buffer << "    Result: illegal return in test statement.\n"
			 << " Error msg:\n" << FormatDeathTestOutput(error_message);
	  break;
	case DIED:
	  if (status_ok) {
		if (matcher_.Matches(error_message)) {
		  success = true;
		} else {
		  std::ostringstream stream;
		  matcher_.DescribeTo(&stream);
		  buffer << "    Result: died but not with expected error.\n"
				 << "  Expected: " << stream.str() << "\n"
				 << "Actual msg:\n"
				 << FormatDeathTestOutput(error_message);
		}
	  } else {
		buffer << "    Result: died but not with expected exit code:\n"
			   << "            " << ExitSummary(status()) << "\n"
			   << "Actual msg:\n" << FormatDeathTestOutput(error_message);
	  }
	  break;
	case IN_PROGRESS:
	default:
	  GTEST_LOG_(FATAL)
		  << "DeathTest::Passed somehow called before conclusion of test";
  }

  DeathTest::set_last_death_test_message(buffer.GetString());
  return success;
}


} // namespace internal
} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
