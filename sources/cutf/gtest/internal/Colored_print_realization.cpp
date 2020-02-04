#include "Colored_print_realization.h"


namespace jmsd {
namespace cutf {
namespace internal {


#if GTEST_OS_WINDOWS && !GTEST_OS_WINDOWS_MOBILE && !GTEST_OS_WINDOWS_PHONE && !GTEST_OS_WINDOWS_RT && !GTEST_OS_WINDOWS_MINGW

// Returns the character attribute for the given color.
WORD Colored_print_realization::GetColorAttribute( GTestColor const color) {
	switch ( color ) {
		case COLOR_RED:
			return FOREGROUND_RED;
		case COLOR_GREEN:
			return FOREGROUND_GREEN;
		case COLOR_YELLOW:
			return FOREGROUND_RED | FOREGROUND_GREEN;
		default:
			return 0;
	}
}

int Colored_print_realization::GetBitOffset( WORD const color_mask_p ) {
	if ( color_mask_p == 0 ) return 0;

	int bitOffset = 0;

	WORD color_mask = color_mask_p;

	while ( ( color_mask & 1 ) == 0 ) {
		color_mask >>= 1;
		++bitOffset;
	}

	return bitOffset;
}

WORD Colored_print_realization::GetNewColor( GTestColor const color, WORD const old_color_attrs ) {
	// Let's reuse the BG
	static WORD const background_mask = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY;
	static WORD const foreground_mask = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
	WORD const existing_bg = old_color_attrs & background_mask;

	WORD new_color = THIS_STATIC::GetColorAttribute( color ) | existing_bg | FOREGROUND_INTENSITY;
	static int const bg_bitOffset = THIS_STATIC::GetBitOffset( background_mask );
	static int const fg_bitOffset = THIS_STATIC::GetBitOffset( foreground_mask );

	if ( ( ( new_color & background_mask ) >> bg_bitOffset ) == ( ( new_color & foreground_mask ) >> fg_bitOffset ) ) {
		new_color ^= FOREGROUND_INTENSITY; // invert intensity
	}

	return new_color;
}

#else

// Returns the ANSI color code for the given color. COLOR_DEFAULT is an invalid input.
char const *Colored_print_realization::GetAnsiColorCode( GTestColor const color ) {
	switch ( color ) {
		case COLOR_RED:
			return "1";
		case COLOR_GREEN:
			return "2";
		case COLOR_YELLOW:
			return "3";
		default:
			return nullptr;
	}
}

#endif  // GTEST_OS_WINDOWS && !GTEST_OS_WINDOWS_MOBILE


} // namespace internal
} // namespace cutf
} // namespace jmsd
