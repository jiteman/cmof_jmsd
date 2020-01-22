#pragma once

#include "Test_property.hxx"


#include <string>


#include "cutf.h"


namespace jmsd {
namespace cutf {


// A copyable object representing a user specified test property which can be
// output as a key/value string pair.
//
// Don't inherit from TestProperty as its destructor is not virtual.
class JMSD_CUTF_SHARED_INTERFACE TestProperty {

public:
	// C'tor.  TestProperty does NOT have a default constructor.
	// Always use this constructor (with parameters) to create a
	// TestProperty object.
	TestProperty( ::std::string const& a_key, ::std::string const &a_value );

	// Gets the user supplied key.
	char const *key() const;

	// Gets the user supplied value.
	char const *value() const;

	// Sets a new value, overriding the one supplied in the constructor.
	void SetValue( ::std::string const &new_value );

private:
	::std::string key_; // The key supplied by the user.
	:: std::string value_; // The value supplied by the user.

};


} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
