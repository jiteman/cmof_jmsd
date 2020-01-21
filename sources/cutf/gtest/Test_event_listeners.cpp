#include "Test_event_listeners.h"


#include "internal/Test_event_repeater.h"


namespace jmsd {
namespace cutf {


// class TestEventListeners

TestEventListeners::TestEventListeners()
	: repeater_(new internal::TestEventRepeater()),
	  default_result_printer_(nullptr),
	  default_xml_generator_(nullptr) {}

TestEventListeners::~TestEventListeners() { delete repeater_; }

// Returns the standard listener responsible for the default console
// output.  Can be removed from the listeners list to shut down default
// console output.  Note that removing this object from the listener list
// with Release transfers its ownership to the user.
void TestEventListeners::Append(TestEventListener* listener) {
  repeater_->Append(listener);
}

// Removes the given event listener from the list and returns it.  It then
// becomes the caller's responsibility to delete the listener. Returns
// NULL if the listener is not found in the list.
TestEventListener* TestEventListeners::Release(TestEventListener* listener) {
  if (listener == default_result_printer_)
	default_result_printer_ = nullptr;
  else if (listener == default_xml_generator_)
	default_xml_generator_ = nullptr;
  return repeater_->Release(listener);
}

// Returns the standard listener responsible for the default console
// output.  Can be removed from the listeners list to shut down default
// console output.  Note that removing this object from the listener list
// with Release transfers its ownership to the caller and makes this
// function return NULL the next time.
TestEventListener* TestEventListeners::default_result_printer() const {
  return default_result_printer_;
}

// Returns the standard listener responsible for the default XML output
// controlled by the --gtest_output=xml flag.  Can be removed from the
// listeners list by users who want to shut down the default XML output
// controlled by this flag and substitute it with custom one.  Note that
// removing this object from the listener list with Release transfers its
// ownership to the caller and makes this function return NULL the next
// time.
TestEventListener* TestEventListeners::default_xml_generator() const {
  return default_xml_generator_;
}

// Returns repeater that broadcasts the TestEventListener events to all
// subscribers.
TestEventListener* TestEventListeners::repeater() {
	return repeater_;
}

// Sets the default_result_printer attribute to the provided listener.
// The listener is also added to the listener list and previous
// default_result_printer is removed from it and deleted. The listener can
// also be NULL in which case it will not be added to the list. Does
// nothing if the previous and the current listener objects are the same.
void TestEventListeners::SetDefaultResultPrinter(TestEventListener* listener) {
  if (default_result_printer_ != listener) {
	// It is an error to pass this method a listener that is already in the
	// list.
	delete Release(default_result_printer_);
	default_result_printer_ = listener;
	if (listener != nullptr) Append(listener);
  }
}

// Sets the default_xml_generator attribute to the provided listener.  The
// listener is also added to the listener list and previous
// default_xml_generator is removed from it and deleted. The listener can
// also be NULL in which case it will not be added to the list. Does
// nothing if the previous and the current listener objects are the same.
void TestEventListeners::SetDefaultXmlGenerator(TestEventListener* listener) {
  if (default_xml_generator_ != listener) {
	// It is an error to pass this method a listener that is already in the
	// list.
	delete Release(default_xml_generator_);
	default_xml_generator_ = listener;
	if (listener != nullptr) Append(listener);
  }
}

// Controls whether events will be forwarded by the repeater to the
// listeners in the list.
bool TestEventListeners::EventForwardingEnabled() const {
  return repeater_->forwarding_enabled();
}

void TestEventListeners::SuppressEventForwarding() {
  repeater_->set_forwarding_enabled(false);
}


} // namespace cutf
} // namespace jmsd
