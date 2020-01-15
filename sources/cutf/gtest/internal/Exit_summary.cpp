#include "Exit_summary.h"


#include "gtest/gtest-message.h"


namespace jmsd {
namespace cutf {
namespace internal {


// Generates a textual description of a given exit code, in the format specified by wait(2).
::std::string ExitSummary( int const exit_code ) {
	::testing::Message m;

# if GTEST_OS_WINDOWS || GTEST_OS_FUCHSIA
	m << "Exited with exit status " << exit_code;

# else
	if ( WIFEXITED( exit_code ) ) {
		m << "Exited with exit status " << WEXITSTATUS( exit_code );
	} else if ( WIFSIGNALED( exit_code ) ) {
		m << "Terminated by signal " << WTERMSIG( exit_code );
	}

#  ifdef WCOREDUMP
	if ( WCOREDUMP( exit_code ) ) {
		m << " (core dumped)";
	}

#  endif

# endif  // GTEST_OS_WINDOWS || GTEST_OS_FUCHSIA

	return m.GetString();
}



} // namespace internal
} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
