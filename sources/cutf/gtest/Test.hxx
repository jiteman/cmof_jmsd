#pragma once


namespace jmsd {
namespace cutf {


// The friend relationship of some of these classes is cyclic.
// If we don't forward declare them the compiler might confuse the classes
// in friendship clauses with same named classes on the scope.
class Test;


} // namespace cutf
} // namespace jmsd
