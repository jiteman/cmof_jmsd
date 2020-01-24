#include "Streaming_listener.h"


namespace jmsd {
namespace cutf {
namespace internal {


#if GTEST_CAN_STREAM_RESULTS_

StreamingListener::StreamingListener(const std::string& host, const std::string& port)
	: socket_writer_(new SocketWriter(host, port)) {
  Start();
}

StreamingListener::StreamingListener(AbstractSocketWriter* socket_writer)
	: socket_writer_(socket_writer) { Start(); }

void StreamingListener::OnTestProgramStart(const ::jmsd::cutf::UnitTest& /* unit_test */) override {
  SendLn("event=TestProgramStart");
}

void StreamingListener::OnTestProgramEnd(const ::jmsd::cutf::UnitTest& unit_test) override {
  // Note that Google Test current only report elapsed time for each
  // test iteration, not for the entire test program.
  SendLn("event=TestProgramEnd&passed=" + FormatBool(unit_test.Passed()));

  // Notify the streaming server to stop.
  socket_writer_->CloseConnection();
}

void StreamingListener::OnTestIterationStart(const ::jmsd::cutf::UnitTest& /* unit_test */,
						  int iteration) override {
  SendLn("event=TestIterationStart&iteration=" +
		 StreamableToString(iteration));
}

void StreamingListener::OnTestIterationEnd(const ::jmsd::cutf::UnitTest& unit_test,
						int /* iteration */) override {
  SendLn("event=TestIterationEnd&passed=" +
		 FormatBool(unit_test.Passed()) + "&elapsed_time=" +
		 StreamableToString(unit_test.elapsed_time()) + "ms");
}

void StreamingListener::OnTestSuiteStart(const ::jmsd::cutf::TestSuite& test_case) override {
  SendLn(std::string("event=TestSuiteStart&name=") + test_case.name());
}

void StreamingListener::OnTestSuiteEnd(const ::jmsd::cutf::TestSuite& test_case) override {
  SendLn("event=TestSuiteEnd&passed=" + FormatBool(test_case.Passed()) +
		 "&elapsed_time=" + StreamableToString(test_case.elapsed_time()) +
		 "ms");
}

#ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_
// Note that "event=TestCaseStart" is a wire format and has to remain
// "case" for compatibilty
void StreamingListener::OnTestCaseStart(const TestCase& test_case) override {
  SendLn(std::string("event=TestCaseStart&name=") + test_case.name());
}

// Note that "event=TestCaseEnd" is a wire format and has to remain
// "case" for compatibilty
void StreamingListener::OnTestCaseEnd(const TestCase& test_case) override {
  SendLn("event=TestCaseEnd&passed=" + FormatBool(test_case.Passed()) +
		 "&elapsed_time=" + StreamableToString(test_case.elapsed_time()) +
		 "ms");
}
#endif // #ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_

void StreamingListener::OnTestStart(const TestInfo& test_info) override {
  SendLn(std::string("event=TestStart&name=") + test_info.name());
}

void StreamingListener::OnTestEnd(const TestInfo& test_info) override {
  SendLn("event=TestEnd&passed=" +
		 REFACTOR_ME_FormatBool((test_info.result())->Passed()) +
		 "&elapsed_time=" +
		 StreamableToString((test_info.result())->elapsed_time()) + "ms");
}

void StreamingListener::OnTestPartResult(const ::testing::TestPartResult& test_part_result) override {
  const char* file_name = test_part_result.file_name();
  if (file_name == nullptr) file_name = "";
  SendLn("event=TestPartResult&file=" + UrlEncode(file_name) +
		 "&line=" + StreamableToString(test_part_result.line_number()) +
		 "&message=" + UrlEncode(test_part_result.message()));
}

// Sends the given message and a newline to the socket.
void StreamingListener::SendLn(const std::string& message) {
	socket_writer_->SendLn(message);
}

// Called at the start of streaming to notify the receiver what protocol we are using.
void StreamingListener::Start() {
	SendLn("gtest_streaming_protocol_version=1.0");
}

// static
::std::string StreamingListener::REFACTOR_ME_FormatBool(bool value) {
	return value ? "1" : "0";
}

#endif // #if GTEST_CAN_STREAM_RESULTS_


} // namespace internal
} // namespace cutf
} // namespace jmsd
