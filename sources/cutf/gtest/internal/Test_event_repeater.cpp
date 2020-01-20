#include "Test_event_repeater.h"


#include "Stl_utilities.hin"


namespace jmsd {
namespace cutf {
namespace internal {


TestEventRepeater::~TestEventRepeater() {
	ForEach(listeners_, Delete<TestEventListener>);
}

void TestEventRepeater::Append(TestEventListener *listener) {
  listeners_.push_back(listener);
}

TestEventListener* TestEventRepeater::Release(TestEventListener *listener) {
  for (size_t i = 0; i < listeners_.size(); ++i) {
	if (listeners_[i] == listener) {
	  listeners_.erase(listeners_.begin() + static_cast<int>(i));
	  return listener;
	}
  }

  return nullptr;
}


} // namespace internal
} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
