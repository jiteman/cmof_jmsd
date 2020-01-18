#include "Assertion_result.h"


namespace jmsd {
namespace cutf {


// Used in EXPECT_TRUE/FALSE(assertion_result).
AssertionResult::AssertionResult(const AssertionResult& other)
	: success_(other.success_),
	  message_(other.message_.get() != nullptr
				   ? new ::std::string(*other.message_)
				   : static_cast< ::std::string*>(nullptr)) {}

// Swaps two AssertionResults.
void AssertionResult::swap(AssertionResult& other) {
  using std::swap;
  swap(success_, other.success_);
  swap(message_, other.message_);
}

// Returns the assertion's negation. Used with EXPECT/ASSERT_FALSE.
AssertionResult AssertionResult::operator!() const {
  AssertionResult negation(!success_);
  if (message_.get() != nullptr) negation << *message_;
  return negation;
}

// Makes a successful assertion result.
AssertionResult AssertionSuccess() {
  return AssertionResult(true);
}

// Makes a failed assertion result.
AssertionResult AssertionFailure() {
  return AssertionResult(false);
}

// Makes a failed assertion result with the given failure message.
// Deprecated; use AssertionFailure() << message.
AssertionResult AssertionFailure(const Message& message) {
  return AssertionFailure() << message;
}


} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
