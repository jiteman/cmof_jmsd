#pragma once

#include "function_Int32_from_environment_or_die.hxx"


#include <cstdint>


#include "cutf.h"


namespace jmsd {
namespace cutf {
namespace internal {


class JMSD_CUTF_SHARED_INTERFACE function_Int32_from_environment_or_die {

public:
	// Parses the environment variable var as a 32-bit integer.
	// If it is unset, returns default_val.
	// If it is not a 32-bit integer, prints an error and aborts.
	static int32_t Int32FromEnvOrDie( char const *env_var, int32_t default_val );

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
public:

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
private:
	virtual ~function_Int32_from_environment_or_die() noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	function_Int32_from_environment_or_die() noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	function_Int32_from_environment_or_die( function_Int32_from_environment_or_die const &another ) noexcept = delete;
	function_Int32_from_environment_or_die &operator =( function_Int32_from_environment_or_die const &another ) noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	function_Int32_from_environment_or_die( function_Int32_from_environment_or_die &&another ) noexcept = delete;
	function_Int32_from_environment_or_die &operator =( function_Int32_from_environment_or_die &&another ) noexcept = delete;

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
private:

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:

};


} // namespace internal
} // namespace cutf
} // namespace jmsd
