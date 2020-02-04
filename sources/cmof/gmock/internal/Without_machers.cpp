#include "Without_machers.h"


namespace jmsd {
namespace cmof {
namespace internal {


WithoutMatchers::WithoutMatchers()
{}

// static
WithoutMatchers WithoutMatchers::Get() {
	return WithoutMatchers();
}


} // namespace internal
} // namespace cmof
} // namespace jmsd
