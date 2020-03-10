#include "Streaming_listener.h"


#include "gtest/Unit_test.h"

#include "gtest/Message.hin"


namespace jmsd {
namespace cutf {
namespace internal {


#if GTEST_CAN_STREAM_RESULTS_

// Checks if str contains '=', '&', '%' or '\n' characters. If yes,
// replaces them by "%xx" where xx is their hexadecimal value. For
// example, replaces "=" with "%3D".  This algorithm is O(strlen(str))
// in both time and space -- important as the input str may contain an
// arbitrarily long test failure message and stack trace.
// static
::std::string StreamingListener::UrlEncode(const char* str) {
  std::string result;
  result.reserve(strlen(str) + 1);
  for (char ch = *str; ch != '\0'; ch = *++str) {
	switch (ch) {
	  case '%':
	  case '=':
	  case '&':
	  case '\n':
		result.append("%" + ::testing::internal::String::FormatByte(static_cast<unsigned char>(ch)));
		break;
	  default:
		result.push_back(ch);
		break;
	}
  }
  return result;
}

StreamingListener::StreamingListener(const std::string& host, const std::string& port)
	: socket_writer_(new SocketWriter(host, port)) {
  Start();
}

StreamingListener::StreamingListener(AbstractSocketWriter* socket_writer)
	: socket_writer_(socket_writer) { Start(); }

void StreamingListener::OnTestProgramStart(const ::jmsd::cutf::UnitTest& /* unit_test */) {
  SendLn("event=TestProgramStart");
}

void StreamingListener::OnTestProgramEnd(const ::jmsd::cutf::UnitTest& unit_test) {
  // Note that Google Test current only report elapsed time for each
  // test iteration, not for the entire test program.
  SendLn("event=TestProgramEnd&passed=" + REFACTOR_ME_FormatBool(unit_test.Passed()));

  // Notify the streaming server to stop.
  socket_writer_->CloseConnection();
}

void StreamingListener::OnTestIterationStart(const ::jmsd::cutf::UnitTest& /* unit_test */, int iteration) {
  SendLn("event=TestIterationStart&iteration=" +
		 function_Streamable_to_string::StreamableToString(iteration));
}

void StreamingListener::OnTestIterationEnd(const ::jmsd::cutf::UnitTest& unit_test, int /* iteration */) {
  SendLn("event=TestIterationEnd&passed=" +
		 REFACTOR_ME_FormatBool(unit_test.Passed()) + "&elapsed_time=" +
		 function_Streamable_to_string::StreamableToString(unit_test.elapsed_time()) + "ms");
}

void StreamingListener::OnTestSuiteStart(const ::jmsd::cutf::TestSuite& test_case) {
  SendLn(std::string("event=TestSuiteStart&name=") + test_case.name());
}

void StreamingListener::OnTestSuiteEnd(const ::jmsd::cutf::TestSuite& test_case) {
  SendLn("event=TestSuiteEnd&passed=" + REFACTOR_ME_FormatBool(test_case.Passed()) +
		 "&elapsed_time=" + function_Streamable_to_string::StreamableToString(test_case.elapsed_time()) +
		 "ms");
}

#ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_
// Note that "event=TestCaseStart" is a wire format and has to remain
// "case" for compatibilty
void StreamingListener::OnTestCaseStart(const TestCase& test_case) {
  SendLn(std::string("event=TestCaseStart&name=") + test_case.name());
}

// Note that "event=TestCaseEnd" is a wire format and has to remain
// "case" for compatibilty
void StreamingListener::OnTestCaseEnd(const TestCase& test_case) {
  SendLn("event=TestCaseEnd&passed=" + FormatBool(test_case.Passed()) +
		 "&elapsed_time=" + StreamableToString(test_case.elapsed_time()) +
		 "ms");
}
#endif // #ifdef GTEST_KEEP_LEGACY_TEST_CASEAPI_

void StreamingListener::OnTestStart(const TestInfo& test_info) {
  SendLn(std::string("event=TestStart&name=") + test_info.name());
}

void StreamingListener::OnTestEnd(const TestInfo& test_info) {
  SendLn("event=TestEnd&passed=" +
		 REFACTOR_ME_FormatBool((test_info.result())->Passed()) +
		 "&elapsed_time=" +
		 function_Streamable_to_string::StreamableToString((test_info.result())->elapsed_time()) + "ms");
}

void StreamingListener::OnTestPartResult(const ::testing::TestPartResult& test_part_result) {
  const char* file_name = test_part_result.file_name();

  if (file_name == nullptr) file_name = "";

  SendLn("event=TestPartResult&file=" + UrlEncode(file_name) +
		 "&line=" + function_Streamable_to_string::StreamableToString(test_part_result.line_number()) +
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
