#pragma once


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
void PrintColorEncoded( char const *str );


} // namespace internal
} // namespace cutf
} // namespace jmsd
