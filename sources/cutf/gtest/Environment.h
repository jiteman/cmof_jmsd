#pragma once

#include "Environment.hxx"


#include "cutf.h"


namespace jmsd {
namespace cutf {


// An Environment object is capable of setting up and tearing down an
// environment.  You should subclass this to define your own
// environment(s).
//
// An Environment object does the set-up and tear-down in virtual
// methods SetUp() and TearDown() instead of the constructor and the
// destructor, as:
//
//   1. You cannot safely throw from a destructor.  This is a problem
//      as in some cases Google Test is used where exceptions are enabled, and
//      we may want to implement ASSERT_* using exceptions where they are
//      available.
//   2. You cannot use ASSERT_* directly in a constructor or
//      destructor.
class JMSD_CUTF_SHARED_INTERFACE Environment {

public:
	virtual ~Environment();

	// Override this to define how to set up the environment.
	virtual void SetUp();

	// Override this to define how to tear down the environment.
	virtual void TearDown();

private:
	// If you see an error about overriding the following function or
	// about it being private, you have mis-spelled SetUp() as Setup().
	struct Setup_should_be_spelled_SetUp;
	virtual Setup_should_be_spelled_SetUp* Setup();

};


} // namespace cutf
} // namespace jmsd
