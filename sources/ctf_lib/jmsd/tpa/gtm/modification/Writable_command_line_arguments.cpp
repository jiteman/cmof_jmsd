#include "Writable_command_line_arguments.h"


#include <cstddef>
#include <cstring>


namespace jmsd {
namespace tpa {
namespace gtm {
namespace modification {


int *Writable_command_line_arguments::take_argument_counter() noexcept {
	return &_writable_argument_counter;
}

char **Writable_command_line_arguments::take_argument_string_array() noexcept {
	return _copy_of_writable_argument_string_array;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
Writable_command_line_arguments::~Writable_command_line_arguments() noexcept {
	for ( int argument_counter = 0; argument_counter < _argument_counter; ++argument_counter ) {
		delete[] _writable_argument_string_array[ argument_counter ];
	}

	delete[] _writable_argument_string_array;
	delete[] _copy_of_writable_argument_string_array;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Writable_command_line_arguments::Writable_command_line_arguments( int const p_argument_counter, char const *const argument_string_array[] ) noexcept
	:
		_argument_counter( argument_string_array != nullptr ? p_argument_counter : 0 ),
		_writable_argument_counter( argument_string_array != nullptr ? p_argument_counter : 0 ),
		_writable_argument_string_array( nullptr ),
		_copy_of_writable_argument_string_array( nullptr )
{
	if ( argument_string_array == nullptr ) return;

	// fucking google gays restruct parameters as they want instead of parsing it like straightforward people do
		// they like it through their asses

	_writable_argument_string_array = new char *[ _argument_counter ]; // for correct deletion
	_copy_of_writable_argument_string_array = new char *[ _argument_counter ];

	for ( int argument_counter = 0; argument_counter < _argument_counter; ++argument_counter ) {
		::std::size_t const length_of_argument_string = ::std::strlen( argument_string_array[ argument_counter ] ) + 1;
		_writable_argument_string_array[ argument_counter ] = new char[ length_of_argument_string + 1 ];
		::std::strncpy( _writable_argument_string_array[ argument_counter ], argument_string_array[ argument_counter ], length_of_argument_string );
		_copy_of_writable_argument_string_array[ argument_counter ] = _writable_argument_string_array[ argument_counter ];
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//Writable_command_line_arguments::Writable_command_line_arguments( Writable_command_line_arguments const &/*another*/ ) noexcept {
//}

//const Writable_command_line_arguments &Writable_command_line_arguments::operator =( Writable_command_line_arguments const &another ) noexcept {
//	if ( &another == this ) return *this;

//	// copy
//	return *this;
//}

//// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//Writable_command_line_arguments::Writable_command_line_arguments( Writable_command_line_arguments &&/*another*/ ) noexcept {
//}

//Writable_command_line_arguments &Writable_command_line_arguments::operator =( Writable_command_line_arguments &&another ) noexcept {
//	if ( &another == this ) return *this;

//	// move
//	return *this;
//}

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


} // namespace modification
} // namespace gtm
} // namespace tpa
} // namespace jmsd

