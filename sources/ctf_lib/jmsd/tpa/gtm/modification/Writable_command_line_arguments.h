#pragma once

#include "Writable_command_line_arguments.hxx"


namespace jmsd {
namespace tpa {
namespace gtm {
namespace modification {


class Writable_command_line_arguments {

public:
	int *take_argument_counter() noexcept;
	char **take_argument_string_array() noexcept;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
public:

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
public:
	~Writable_command_line_arguments() noexcept;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
public:
	Writable_command_line_arguments( int const argument_counter, char const *const argument_string_array[] ) noexcept;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
public:
	Writable_command_line_arguments( Writable_command_line_arguments const &another ) = delete;
	const Writable_command_line_arguments &operator =( Writable_command_line_arguments const &another ) = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
public:
	Writable_command_line_arguments( Writable_command_line_arguments &&another ) = delete;
	Writable_command_line_arguments &operator =( Writable_command_line_arguments &&another ) = delete;

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
private:
	int const _argument_counter;
	int _writable_argument_counter;
	char **_writable_argument_string_array;
	char **_copy_of_writable_argument_string_array;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:

};


} // namespace modification
} // namespace gtm
} // namespace tpa
} // namespace jmsd
