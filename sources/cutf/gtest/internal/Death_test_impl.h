#pragma once

#include "Death_test_impl.hxx"


#include "gtest/gtest-matchers.h"
#include "gtest-death-test-internal.h"


namespace jmsd {
namespace cutf {
namespace internal {


// Provides cross platform implementation for some death functionality.
class DeathTestImpl : public ::testing::internal::DeathTest {
protected:
	DeathTestImpl(const char* a_statement, ::testing::Matcher<const std::string&> matcher);

	// read_fd_ is expected to be closed and cleared by a derived class.
	~DeathTestImpl() override;

	void Abort(AbortReason reason) override;
	bool Passed(bool status_ok) override;

	const char* statement() const;
	bool spawned() const;
	void set_spawned(bool is_spawned);
	int status() const;
	void set_status(int a_status);
	::testing::internal::DeathTestOutcome outcome() const;
	void set_outcome( ::testing::internal::DeathTestOutcome an_outcome );
	int read_fd() const;
	void set_read_fd(int fd);
	int write_fd() const;
	void set_write_fd(int fd);

	// Called in the parent process only. Reads the result code of the death
	// test child process via a pipe, interprets it to set the outcome_
	// member, and closes read_fd_.  Outputs diagnostics and terminates in
	// case of unexpected codes.
	void ReadAndInterpretStatusByte();

	// Returns stderr output from the child process.
	virtual std::string GetErrorLogs();

private:
	// The textual content of the code this object is testing.  This class
	// doesn't own this string and should not attempt to delete it.
	const char* const statement_;

	// A matcher that's expected to match the stderr output by the child process.
	::testing::Matcher<const std::string&> matcher_;

	// True if the death test child process has been successfully spawned.
	bool spawned_;

	// The exit status of the child process.
	int status_;

	// How the death test concluded.
	::testing::internal::DeathTestOutcome outcome_;

	// Descriptor to the read end of the pipe to the child process.  It is
	// always -1 in the child process.  The child keeps its write end of the
	// pipe in write_fd_.
	int read_fd_;

	// Descriptor to the child's write end of the pipe to the parent process.
	// It is always -1 in the parent process.  The parent keeps its end of the
	// pipe in read_fd_.
	int write_fd_;
};


} // namespace internal
} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
