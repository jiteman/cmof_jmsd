#include "Assertion_result.h"


#include "Assertion_result.hin"

#include "Message.hin"


namespace jmsd {
namespace cutf {


// Makes a successful assertion result.
AssertionResult AssertionResult::AssertionSuccess() {
	return AssertionResult(true);
}

// Makes a failed assertion result.
AssertionResult AssertionResult::AssertionFailure() {
	return AssertionResult(false);
}

// Makes a failed assertion result with the given failure message.
// Deprecated; use AssertionFailure() << message.
AssertionResult AssertionResult::AssertionFailure(const Message& message) {
	return THIS_STATIC::AssertionFailure() << message;
}

// Used in EXPECT_TRUE/FALSE(assertion_result).
AssertionResult::AssertionResult( const AssertionResult &other )
	:
		success_( other.success_ ),
		message_(
			other.message_.get() != nullptr ?
				new ::std::string( *other.message_) :
				static_cast< ::std::string * >( nullptr ) )
{}

// Returns true if and only if the assertion succeeded.
AssertionResult::operator bool() const {
	return success_;
}

// Swaps two AssertionResults.
void AssertionResult::swap(AssertionResult& other) {
  using std::swap;
  swap(success_, other.success_);
  swap(message_, other.message_);
}

// Returns the assertion's negation. Used with EXPECT/ASSERT_FALSE.
AssertionResult AssertionResult::operator !() const {
	AssertionResult negation( !success_ );

	if ( message_.get() != nullptr ) {
		negation << *message_;
	}

	return negation;
}

// Returns the text streamed into this AssertionResult. Test assertions
// use it when they fail (i.e., the predicate's outcome doesn't match the
// assertion's expectation). When nothing has been streamed into the
// object, returns an empty string.
const char *AssertionResult::message() const {
	return message_.get() != nullptr ? message_->c_str() : "";
}
// Deprecated; please use message() instead.
const char *AssertionResult::failure_message() const {
	return message();
}

// Allows streaming basic output manipulators such as endl or flush into this object.
AssertionResult &AssertionResult::operator <<( ::std::ostream &( *basic_manipulator )( ::std::ostream &stream ) ) {
	this->AppendMessage( Message() << basic_manipulator );
	return *this;
}

// Appends the contents of message to message_.
void AssertionResult::AppendMessage( Message const &a_message ) {
	if ( message_.get() == nullptr ) {
		message_.reset( new ::std::string );
	}

	message_->append(a_message.GetString().c_str());
}

} // namespace cutf
} // namespace jmsd
