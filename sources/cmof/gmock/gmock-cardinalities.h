#pragma once

// This file implements some commonly used cardinalities.
// More cardinalities can be defined by the user implementing the CardinalityInterface interface if necessary.

#include "gmock/internal/gmock-port.h"
#include "gtest/gtest.h"

#include <limits.h>
#include <memory>
#include <ostream>


namespace testing {

// To implement a cardinality Foo, define:
//   1. a class FooCardinality that implements the
//      CardinalityInterface interface, and
//   2. a factory function that creates a Cardinality object from a
//      const FooCardinality*.
//
// The two-level delegation design follows that of Matcher, providing
// consistency for extension developers.  It also eases ownership
// management as Cardinality objects can now be copied like plain values.

// The implementation of a cardinality.
class CardinalityInterface {
 public:
  virtual ~CardinalityInterface() {}

  // Conservative estimate on the lower/upper bound of the number of
  // calls allowed.
  virtual int ConservativeLowerBound() const { return 0; }
  virtual int ConservativeUpperBound() const { return INT_MAX; }

  // Returns true if and only if call_count calls will satisfy this
  // cardinality.
  virtual bool IsSatisfiedByCallCount(int call_count) const = 0;

  // Returns true if and only if call_count calls will saturate this
  // cardinality.
  virtual bool IsSaturatedByCallCount(int call_count) const = 0;

  // Describes self to an ostream.
  virtual void DescribeTo(::std::ostream* os) const = 0;
};

// A Cardinality is a copyable and IMMUTABLE (except by assignment)
// object that specifies how many times a mock function is expected to
// be called.  The implementation of Cardinality is just a std::shared_ptr
// to const CardinalityInterface. Don't inherit from Cardinality!
class JMSD_DEPRECATED_GMOCK_API_ Cardinality {
 public:
  // Constructs a null cardinality.  Needed for storing Cardinality
  // objects in STL containers.
  Cardinality() {}

  // Constructs a Cardinality from its implementation.
  explicit Cardinality(const CardinalityInterface* impl) : impl_(impl) {}

  // Conservative estimate on the lower/upper bound of the number of
  // calls allowed.
  int ConservativeLowerBound() const { return impl_->ConservativeLowerBound(); }
  int ConservativeUpperBound() const { return impl_->ConservativeUpperBound(); }

  // Returns true if and only if call_count calls will satisfy this
  // cardinality.
  bool IsSatisfiedByCallCount(int call_count) const {
    return impl_->IsSatisfiedByCallCount(call_count);
  }

  // Returns true if and only if call_count calls will saturate this
  // cardinality.
  bool IsSaturatedByCallCount(int call_count) const {
    return impl_->IsSaturatedByCallCount(call_count);
  }

  // Returns true if and only if call_count calls will over-saturate this
  // cardinality, i.e. exceed the maximum number of allowed calls.
  bool IsOverSaturatedByCallCount(int call_count) const {
    return impl_->IsSaturatedByCallCount(call_count) &&
        !impl_->IsSatisfiedByCallCount(call_count);
  }

  // Describes self to an ostream
  void DescribeTo(::std::ostream* os) const { impl_->DescribeTo(os); }

  // Describes the given actual call count to an ostream.
  static void DescribeActualCallCountTo(int actual_call_count,
                                        ::std::ostream* os);

 private:
  std::shared_ptr<const CardinalityInterface> impl_;
};

// Creates a cardinality that allows at least n calls.
JMSD_DEPRECATED_GMOCK_API_ Cardinality AtLeast(int n);

// Creates a cardinality that allows at most n calls.
JMSD_DEPRECATED_GMOCK_API_ Cardinality AtMost(int n);

// Creates a cardinality that allows any number of calls.
JMSD_DEPRECATED_GMOCK_API_ Cardinality AnyNumber();

// Creates a cardinality that allows between min and max calls.
JMSD_DEPRECATED_GMOCK_API_ Cardinality Between(int min, int max);

// Creates a cardinality that allows exactly n calls.
JMSD_DEPRECATED_GMOCK_API_ Cardinality Exactly(int n);

// Creates a cardinality from its implementation.
inline Cardinality MakeCardinality(const CardinalityInterface* c) {
  return Cardinality(c);
}

}  // namespace testing
