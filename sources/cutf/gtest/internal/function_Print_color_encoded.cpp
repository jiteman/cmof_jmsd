#include "function_Print_color_encoded.h"


#include "Colored_print.h"
#include "gtest-color.h"

#include <string>
#include <cstring>


namespace jmsd {
namespace cutf {
namespace internal {


// Prints a string containing code-encoded text.  The following escape
// sequences can be used in the string to control the text color:
//   @@    prints a single '@' character.
//   @R    changes the color to red.
//   @G    changes the color to green.
//   @Y    changes the color to yellow.
//   @D    changes to the default terminal text color.
// static
void function_Print_color_encoded::PrintColorEncoded( char const *str ) {
	GTestColor color = GTestColor::COLOR_DEFAULT;  // The current color.

	// Conceptually, we split the string into segments divided by escape
	// sequences.  Then we print one segment at a time.  At the end of
	// each iteration, the str pointer advances to the beginning of the
	// next segment.
	for ( ;; ) {
		char const *p = strchr(str, '@');

		if (p == nullptr) {
			Colored_print::ColoredPrintf(color, "%s", str);
			return;
		}

		Colored_print::ColoredPrintf(color, "%s", std::string(str, p).c_str());

		const char ch = p[1];
		str = p + 2;

		if (ch == '@') {
			Colored_print::ColoredPrintf(color, "@");
		} else if (ch == 'D') {
			color = GTestColor::COLOR_DEFAULT;
		} else if (ch == 'R') {
			color = GTestColor::COLOR_RED;
		} else if (ch == 'G') {
			color = GTestColor::COLOR_GREEN;
		} else if (ch == 'Y') {
			color = GTestColor::COLOR_YELLOW;
		} else {
			--str;
		}
	}
}


} // namespace internal
} // namespace cutf
} // namespace jmsd

