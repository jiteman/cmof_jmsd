#include "Colored_print.h"


#include "Colored_print_realization.h"

#include "gtest/gtest-flags.h"

#include "gtest-string.h"

#include <cstdio>
#include <cstdarg>


#if GTEST_OS_WINDOWS && !GTEST_OS_WINDOWS_MOBILE && !GTEST_OS_WINDOWS_PHONE && !GTEST_OS_WINDOWS_RT && !GTEST_OS_WINDOWS_MINGW
#include <windows.h>
#endif // #if GTEST_OS_WINDOWS && !GTEST_OS_WINDOWS_MOBILE && !GTEST_OS_WINDOWS_PHONE && !GTEST_OS_WINDOWS_RT && !GTEST_OS_WINDOWS_MINGW


namespace jmsd {
namespace cutf {
namespace internal {


// Helpers for printing colored strings to stdout.
// Note that on Windows, we cannot simply emit special characters and have the terminal change colors.
// This routine must actually emit the characters rather than return a string that would be colored when printed, as can be done on Linux.
void Colored_print::ColoredPrintf( GTestColor const color, char const *const fmt, ... ) {
	va_list args;
	va_start( args, fmt );

#if GTEST_OS_WINDOWS_MOBILE || GTEST_OS_ZOS || GTEST_OS_IOS || GTEST_OS_WINDOWS_PHONE || GTEST_OS_WINDOWS_RT || defined( ESP_PLATFORM )
	bool const use_color = AlwaysFalse();
#else
	static bool const in_color_mode = THIS_STATIC::ShouldUseColor( ::testing::internal::posix::IsATTY( ::testing::internal::posix::FileNo( stdout ) ) != 0 );
	bool const use_color = in_color_mode && ( color != COLOR_DEFAULT );
#endif // #if GTEST_OS_WINDOWS_MOBILE || GTEST_OS_ZOS || GTEST_OS_IOS || GTEST_OS_WINDOWS_PHONE || GTEST_OS_WINDOWS_RT || defined( ESP_PLATFORM )

	if ( !use_color ) {
		vprintf( fmt, args );
		va_end( args );
		return;
	}

#if GTEST_OS_WINDOWS && !GTEST_OS_WINDOWS_MOBILE && !GTEST_OS_WINDOWS_PHONE && !GTEST_OS_WINDOWS_RT && !GTEST_OS_WINDOWS_MINGW
	HANDLE const stdout_handle = ::GetStdHandle( STD_OUTPUT_HANDLE );

	// Gets the current text color.
	CONSOLE_SCREEN_BUFFER_INFO buffer_info = {};
	::GetConsoleScreenBufferInfo( stdout_handle, &buffer_info );
	WORD const old_color_attrs = buffer_info.wAttributes;
	WORD const new_color = Colored_print_realization::GetNewColor( color, old_color_attrs );

	// We need to flush the stream buffers into the console before each SetConsoleTextAttribute call lest it affect the text that is already
	// printed but has not yet reached the console.
	fflush( stdout );
	::SetConsoleTextAttribute( stdout_handle, new_color );
	vprintf( fmt, args );
	fflush( stdout );
	::SetConsoleTextAttribute( stdout_handle, old_color_attrs ); // Restores the text color.

#else
	printf( "\033[0;3%sm", Colored_print_realization::GetAnsiColorCode( color ) );
	vprintf( fmt, args );
	printf( "\033[m" ); // Resets the terminal to default.

#endif  // GTEST_OS_WINDOWS && !GTEST_OS_WINDOWS_MOBILE

	va_end( args );
}


// Returns true if and only if Google Test should use colors in the output.
bool Colored_print::ShouldUseColor( bool const stdout_is_tty ) {
	char const *const gtest_color = ::testing:: GTEST_FLAG( color ).c_str();

	if ( ::testing::internal::String::CaseInsensitiveCStringEquals( gtest_color, "auto" ) ) {

#if GTEST_OS_WINDOWS && !GTEST_OS_WINDOWS_MINGW
	// On Windows the TERM variable is usually not set, but the console there does support colors.
	return stdout_is_tty;

#else
	// On non-Windows platforms, we rely on the TERM variable.
	char const *const term = ::testing::internal::posix::GetEnv( "TERM" );

	bool const term_supports_color =
		::testing::internal::String::CStringEquals( term, "xterm" ) ||
		::testing::internal::String::CStringEquals( term, "xterm-color" ) ||
		::testing::internal::String::CStringEquals( term, "xterm-256color" ) ||
		::testing::internal::String::CStringEquals( term, "screen" ) ||
		::testing::internal::String::CStringEquals( term, "screen-256color" ) ||
		::testing::internal::String::CStringEquals( term, "tmux" ) ||
		::testing::internal::String::CStringEquals( term, "tmux-256color" ) ||
		::testing::internal::String::CStringEquals( term, "rxvt-unicode" ) ||
		::testing::internal::String::CStringEquals( term, "rxvt-unicode-256color" ) ||
		::testing::internal::String::CStringEquals( term, "linux" ) ||
		::testing::internal::String::CStringEquals( term, "cygwin" );

	return stdout_is_tty && term_supports_color;

#endif  // GTEST_OS_WINDOWS
	}

	return
		::testing::internal::String::CaseInsensitiveCStringEquals( gtest_color, "yes" ) ||
		::testing::internal::String::CaseInsensitiveCStringEquals( gtest_color, "true" ) ||
		::testing::internal::String::CaseInsensitiveCStringEquals( gtest_color, "t" ) ||
		::testing::internal::String::CStringEquals( gtest_color, "1" );

	// We take "yes", "true", "t", and "1" as meaning "yes".
	// If the value is neither one of these nor "auto", we treat it as "no" to be conservative.
}


} // namespace internal
} // namespace cutf
} // namespace jmsd

