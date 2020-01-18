#pragma once

#include "Assertion_result.hxx"


#include "internal/gtest-port.h"


namespace jmsd {
namespace cutf {


class GTEST_API_ AssertionResult {

public:
	// Makes a successful assertion result.
	static AssertionResult AssertionSuccess();

	// Makes a failed assertion result.
	static AssertionResult AssertionFailure();

	// Makes a failed assertion result with the given failure message.
	// Deprecated; use AssertionFailure() << msg.
	static AssertionResult AssertionFailure(const Message& msg);

public:
	// Copy constructor.
	// Used in EXPECT_TRUE/FALSE(assertion_result).
	AssertionResult(const AssertionResult& other);

	// Used in the EXPECT_TRUE/FALSE(bool_expression).
	//
	// T must be contextually convertible to bool.
	//
	// The second parameter prevents this overload from being considered if
	// the argument is implicitly convertible to AssertionResult. In that case
	// we want AssertionResult's copy constructor to be used.
	template< typename T >
	explicit AssertionResult(
		T const &success,
		typename ::std::enable_if< !::std::is_convertible< T, AssertionResult >::value >::type * /*enabler*/ = nullptr )
		:
			success_( success )
	{}

	// Assignment operator.
	AssertionResult &operator =( AssertionResult other ) {
		this->swap( other );
		return *this;
	}

  // Returns true if and only if the assertion succeeded.
  operator bool() const { return success_; }  // NOLINT

  // Returns the assertion's negation. Used with EXPECT/ASSERT_FALSE.
  AssertionResult operator!() const;

  // Returns the text streamed into this AssertionResult. Test assertions
  // use it when they fail (i.e., the predicate's outcome doesn't match the
  // assertion's expectation). When nothing has been streamed into the
  // object, returns an empty string.
  const char* message() const {
	return message_.get() != nullptr ? message_->c_str() : "";
  }
  // Deprecated; please use message() instead.
  const char* failure_message() const { return message(); }

  // Streams a custom failure message into this object.
  template <typename T> AssertionResult& operator<<(const T& value) {
	AppendMessage(Message() << value);
	return *this;
  }

  // Allows streaming basic output manipulators such as endl or flush into
  // this object.
  AssertionResult& operator<<(
	  ::std::ostream& (*basic_manipulator)(::std::ostream& stream)) {
	AppendMessage(Message() << basic_manipulator);
	return *this;
  }

 private:
  // Appends the contents of message to message_.
  void AppendMessage(const Message& a_message) {
	if (message_.get() == nullptr) message_.reset(new ::std::string);
	message_->append(a_message.GetString().c_str());
  }

  // Swap the contents of this AssertionResult with other.
  void swap(AssertionResult& other);

  // Stores result of the assertion predicate.
  bool success_;
  // Stores the message describing the condition in case the expectation
  // construct is not satisfied with the predicate's outcome.
  // Referenced via a pointer to avoid taking too much stack frame space
  // with test assertions.
  std::unique_ptr< ::std::string> message_;
};


} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
