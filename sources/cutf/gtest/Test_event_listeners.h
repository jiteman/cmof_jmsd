#pragma once

#include "Test_event_listeners.hxx"


#include "internal/gtest-port.h"

#include "Test_event_listener.hxx"
#include "Test_info.hxx"
#include "Test_suite.hxx"
#include "internal/Test_event_repeater.hxx"
#include "internal/Default_global_test_part_result_reporter.hxx"
#include "internal/Unit_test_impl.hxx"

#include "gtest-death-test.hxx"


namespace testing {
namespace internal {


class TestEventListenersAccessor; // gtest_unittest.cc


} // namespace internal
} // namespace testing


namespace jmsd {
namespace cutf {


// TestEventListeners lets users add listeners to track events in Google Test.
class JMSD_DEPRECATED_GTEST_API_ TestEventListeners {
 public:
  TestEventListeners();
  ~TestEventListeners();

  // Appends an event listener to the end of the list. Google Test assumes
  // the ownership of the listener (i.e. it will delete the listener when
  // the test program finishes).
  void Append(TestEventListener* listener);

  // Removes the given event listener from the list and returns it.  It then
  // becomes the caller's responsibility to delete the listener. Returns
  // NULL if the listener is not found in the list.
  TestEventListener* Release(TestEventListener* listener);

  // Returns the standard listener responsible for the default console
  // output.  Can be removed from the listeners list to shut down default
  // console output.  Note that removing this object from the listener list
  // with Release transfers its ownership to the caller and makes this
  // function return NULL the next time.
  TestEventListener* default_result_printer() const;

  // Returns the standard listener responsible for the default XML output
  // controlled by the --gtest_output=xml flag.  Can be removed from the
  // listeners list by users who want to shut down the default XML output
  // controlled by this flag and substitute it with custom one.  Note that
  // removing this object from the listener list with Release transfers its
  // ownership to the caller and makes this function return NULL the next
  // time.
  TestEventListener* default_xml_generator() const;

 private:
	friend TestSuite;
	friend TestInfo;
	friend internal::UnitTestImpl;
	friend ::testing::internal::NoExecDeathTest;
	friend internal::DefaultGlobalTestPartResultReporter;

private: // testing
	friend ::testing::internal::TestEventListenersAccessor; // gtest_unittest.cc
	//  friend class internal::NoExecDeathTest;

  // Returns repeater that broadcasts the TestEventListener events to all
  // subscribers.
  TestEventListener* repeater();

  // Sets the default_result_printer attribute to the provided listener.
  // The listener is also added to the listener list and previous
  // default_result_printer is removed from it and deleted. The listener can
  // also be NULL in which case it will not be added to the list. Does
  // nothing if the previous and the current listener objects are the same.
  void SetDefaultResultPrinter(TestEventListener* listener);

  // Sets the default_xml_generator attribute to the provided listener.  The
  // listener is also added to the listener list and previous
  // default_xml_generator is removed from it and deleted. The listener can
  // also be NULL in which case it will not be added to the list. Does
  // nothing if the previous and the current listener objects are the same.
  void SetDefaultXmlGenerator(TestEventListener* listener);

  // Controls whether events will be forwarded by the repeater to the
  // listeners in the list.
  bool EventForwardingEnabled() const;
  void SuppressEventForwarding();

  // The actual list of listeners.
  internal::TestEventRepeater* repeater_;
  // Listener responsible for the standard result output.
  TestEventListener* default_result_printer_;
  // Listener responsible for the creation of the XML output file.
  TestEventListener* default_xml_generator_;

  // We disallow copying TestEventListeners.
  GTEST_DISALLOW_COPY_AND_ASSIGN_(TestEventListeners);
};


} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
