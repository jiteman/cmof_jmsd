#pragma once

#include "Streaming_listener.hxx"


#include "gtest/Empty_test_event_listener.h"

#include "Socket_writer.h"
#include "Abstract_socket_writer.h"


#include "gtest-port.h"


namespace jmsd {
namespace cutf {
namespace internal {


#if GTEST_CAN_STREAM_RESULTS_

// Streams test results to the given port on the given host machine.
class StreamingListener :
	public EmptyTestEventListener
{
public:
  // Escapes '=', '&', '%', and '\n' characters in str as "%xx".
  static ::std::string UrlEncode(const char* str);

  StreamingListener(const std::string& host, const std::string& port);

  explicit StreamingListener(AbstractSocketWriter* socket_writer);

  void OnTestProgramStart(const ::jmsd::cutf::UnitTest &unit_test ) override;

  void OnTestProgramEnd(const ::jmsd::cutf::UnitTest& unit_test) override;

  void OnTestIterationStart( ::jmsd::cutf::UnitTest const &unit_test, int iteration ) override;

  void OnTestIterationEnd( ::jmsd::cutf::UnitTest const &unit_test, int iteration ) override;

  void OnTestSuiteStart(const ::jmsd::cutf::TestSuite& test_case) override;

  // Note that "event=TestCaseEnd" is a wire format and has to remain
  // "case" for compatibilty
  void OnTestSuiteEnd(const ::jmsd::cutf::TestSuite& test_case) override;


 #ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_
  // Note that "event=TestCaseStart" is a wire format and has to remain
  // "case" for compatibilty
  void OnTestCaseStart(const TestCase& test_case) override;

  // Note that "event=TestCaseEnd" is a wire format and has to remain
  // "case" for compatibilty
  void OnTestCaseEnd(const TestCase& test_case) override;
 #endif // #ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_

  void OnTestStart(const TestInfo& test_info) override;

  void OnTestEnd(const TestInfo& test_info) override;

  void OnTestPartResult(const ::testing::TestPartResult& test_part_result) override;

 private:
  // Sends the given message and a newline to the socket.
  void SendLn(const std::string& message);

  // Called at the start of streaming to notify the receiver what
  // protocol we are using.
  void Start();
  static ::std::string REFACTOR_ME_FormatBool(bool value);

  const std::unique_ptr<AbstractSocketWriter> socket_writer_;

  GTEST_DISALLOW_COPY_AND_ASSIGN_(StreamingListener);
};

#endif // #if GTEST_CAN_STREAM_RESULTS_


} // namespace internal
} // namespace cutf
} // namespace jmsd
