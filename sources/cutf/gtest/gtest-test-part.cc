// The Google C++ Testing and Mocking Framework (Google Test)

#include "gtest/gtest-test-part.h"

#include "internal/Unit_test_impl.h"
#include "internal/gtest-constants-internal.h"

#include "gtest/internal/gtest-port.h"
#include "gtest/gtest-internal-inl.h"

namespace testing {

// Gets the summary of the failure message by omitting the stack trace in it.
std::string TestPartResult::ExtractSummary(const char* message) {
  const char* const stack_trace = strstr(message, ::jmsd::cutf::constants::internal::kStackTraceMarker);
  return stack_trace == nullptr ? message : std::string(message, stack_trace);
}

// Prints a TestPartResult object.
std::ostream& operator<<(std::ostream& os, const TestPartResult& result) {
  return os << internal::FormatFileLocation(result.file_name(),
											result.line_number())
			<< " "
			<< (result.type() == TestPartResult::Type::kSuccess
					? "Success"
					: result.type() == TestPartResult::Type::kSkip
						  ? "Skipped"
						  : result.type() == TestPartResult::Type::kFatalFailure
								? "Fatal failure"
								: "Non-fatal failure")
			<< ":\n"
			<< result.message() << std::endl;
}

// Appends a TestPartResult to the array.
void TestPartResultArray::Append(const TestPartResult& result) {
  array_.push_back(result);
}

// Returns the TestPartResult at the given index (0-based).
const TestPartResult& TestPartResultArray::GetTestPartResult(int index) const {
  if (index < 0 || index >= size()) {
//    printf("\nInvalid index (%d) into TestPartResultArray.\n", index);
	internal::posix::Abort();
  }

  return array_[static_cast<size_t>(index)];
}

// Returns the number of TestPartResult objects in the array.
int TestPartResultArray::size() const {
  return static_cast<int>(array_.size());
}

namespace internal {

HasNewFatalFailureHelper::HasNewFatalFailureHelper()
	:
		has_new_fatal_failure_( false ),
		original_reporter_( ::jmsd::cutf::internal::GetUnitTestImpl()->GetTestPartResultReporterForCurrentThread() )
{
	::jmsd::cutf::internal::GetUnitTestImpl()->SetTestPartResultReporterForCurrentThread( this );
}

HasNewFatalFailureHelper::~HasNewFatalFailureHelper() {
	::jmsd::cutf::internal::GetUnitTestImpl()->SetTestPartResultReporterForCurrentThread( original_reporter_ );
}

void HasNewFatalFailureHelper::ReportTestPartResult(
	const TestPartResult& result) {
  if (result.fatally_failed())
	has_new_fatal_failure_ = true;
  original_reporter_->ReportTestPartResult(result);
}

}  // namespace internal

}  // namespace testing
