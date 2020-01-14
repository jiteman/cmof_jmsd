#include "Test_property.h"


namespace jmsd {
namespace cutf {


TestProperty::TestProperty( ::std::string const &a_key, ::std::string const &a_value )
	:
		key_(a_key), value_(a_value)
{}

// Gets the user supplied key.
char const *TestProperty::key() const {
	return key_.c_str();
}

// Gets the user supplied value.
char const *TestProperty::value() const {
return value_.c_str();
}

// Sets a new value, overriding the one supplied in the constructor.
void TestProperty::SetValue( ::std::string const &new_value ) {
	value_ = new_value;
}


} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
