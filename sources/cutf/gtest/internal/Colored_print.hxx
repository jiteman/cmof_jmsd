#pragma once


#include "gtest-port.h"

#include "gtest-color.hxx"

#include "cutf.h"


#if GTEST_OS_WINDOWS && !GTEST_OS_WINDOWS_MOBILE && !GTEST_OS_WINDOWS_PHONE && !GTEST_OS_WINDOWS_RT && !GTEST_OS_WINDOWS_MINGW
#include <windows.h>
#endif // #if GTEST_OS_WINDOWS && !GTEST_OS_WINDOWS_MOBILE && !GTEST_OS_WINDOWS_PHONE && !GTEST_OS_WINDOWS_RT && !GTEST_OS_WINDOWS_MINGW


namespace jmsd {
namespace cutf {
namespace internal {


JMSD_DEPRECATED_GTEST_API_ GTEST_ATTRIBUTE_PRINTF_( 2, 3 )
void ColoredPrintf( GTestColor color, char const *fmt, ... );

#if GTEST_OS_WINDOWS && !GTEST_OS_WINDOWS_MOBILE && !GTEST_OS_WINDOWS_PHONE && !GTEST_OS_WINDOWS_RT && !GTEST_OS_WINDOWS_MINGW

// Returns the character attribute for the given color.
WORD GetColorAttribute( GTestColor color );

int GetBitOffset( WORD color_mask );

WORD GetNewColor( GTestColor color, WORD old_color_attrs );

#else // #if GTEST_OS_WINDOWS && !GTEST_OS_WINDOWS_MOBILE && !GTEST_OS_WINDOWS_PHONE && !GTEST_OS_WINDOWS_RT && !GTEST_OS_WINDOWS_MINGW

// Returns the ANSI color code for the given color. COLOR_DEFAULT is an invalid input.
char const *GetAnsiColorCode( GTestColor color );

#endif // #if GTEST_OS_WINDOWS && !GTEST_OS_WINDOWS_MOBILE && !GTEST_OS_WINDOWS_PHONE && !GTEST_OS_WINDOWS_RT && !GTEST_OS_WINDOWS_MINGW

// Returns true if and only if Google Test should use colors in the output.
JMSD_CUTF_SHARED_INTERFACE bool ShouldUseColor( bool stdout_is_tty );


} // namespace internal
} // namespace cutf
} // namespace jmsd
