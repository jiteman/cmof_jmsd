#pragma once

#include "Colored_print_realization.hxx"


#include "gtest-color.hxx"

#include "gtest-port.h"

#if GTEST_OS_WINDOWS && !GTEST_OS_WINDOWS_MOBILE && !GTEST_OS_WINDOWS_PHONE && !GTEST_OS_WINDOWS_RT && !GTEST_OS_WINDOWS_MINGW
#include <windows.h>
#endif // #if GTEST_OS_WINDOWS && !GTEST_OS_WINDOWS_MOBILE && !GTEST_OS_WINDOWS_PHONE && !GTEST_OS_WINDOWS_RT && !GTEST_OS_WINDOWS_MINGW


namespace jmsd {
namespace cutf {
namespace internal {


class Colored_print_realization {

	typedef Colored_print_realization THIS_STATIC;

public:
#if GTEST_OS_WINDOWS && !GTEST_OS_WINDOWS_MOBILE && !GTEST_OS_WINDOWS_PHONE && !GTEST_OS_WINDOWS_RT && !GTEST_OS_WINDOWS_MINGW

	// Returns the character attribute for the given color.
	static WORD GetColorAttribute( GTestColor color );
	static int GetBitOffset( WORD color_mask );
	static WORD GetNewColor( GTestColor color, WORD old_color_attrs );

#else // #if GTEST_OS_WINDOWS && !GTEST_OS_WINDOWS_MOBILE && !GTEST_OS_WINDOWS_PHONE && !GTEST_OS_WINDOWS_RT && !GTEST_OS_WINDOWS_MINGW

	// Returns the ANSI color code for the given color. COLOR_DEFAULT is an invalid input.
	static char const *GetAnsiColorCode( GTestColor color );

#endif // #if GTEST_OS_WINDOWS && !GTEST_OS_WINDOWS_MOBILE && !GTEST_OS_WINDOWS_PHONE && !GTEST_OS_WINDOWS_RT && !GTEST_OS_WINDOWS_MINGW

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
public:

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
private:
	virtual ~Colored_print_realization() noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	Colored_print_realization() noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	Colored_print_realization( Colored_print_realization const &another ) noexcept = delete;
	Colored_print_realization &operator =( Colored_print_realization const &another ) noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	Colored_print_realization( Colored_print_realization &&another ) noexcept = delete;
	Colored_print_realization &operator =( Colored_print_realization &&another ) noexcept = delete;

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
