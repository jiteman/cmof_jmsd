#include "Json_test_result_printer.h"


#include "gtest/Text_output_utilities.h"
#include "gtest/Test_info.h"

#include "function_Open_file_for_writing.h"
#include "Format_time.h"
#include "function_String_stream_to_string.h"

#include "gtest-string.h"

#include "function_Streamable_to_string.hin"


namespace jmsd {
namespace cutf {
namespace internal {


// Creates a new JsonUnitTestResultPrinter.
JsonUnitTestResultPrinter::JsonUnitTestResultPrinter(const char* output_file)
	: output_file_(output_file) {
  if (output_file_.empty()) {
	GTEST_LOG_(FATAL) << "JSON output file may not be null";
  }
}

void JsonUnitTestResultPrinter::OnTestIterationEnd(const ::jmsd::cutf::UnitTest& unit_test,
												  int /*iteration*/) {
  FILE* jsonout = function_Open_file_for_writing::OpenFileForWriting(output_file_);
  std::stringstream stream;
  PrintJsonUnitTest(&stream, unit_test);
  fprintf(jsonout, "%s", function_String_stream_to_string::StringStreamToString( stream ).c_str());
  fclose(jsonout);
}

// static
::std::string JsonUnitTestResultPrinter::Indent( size_t const width ) {
	return ::std::string( width, ' ' );
}

// Returns an JSON-escaped copy of the input string str.
std::string JsonUnitTestResultPrinter::EscapeJson(const std::string& str) {
  Message m;

  for (size_t i = 0; i < str.size(); ++i) {
	const char ch = str[i];
	switch (ch) {
	  case '\\':
	  case '"':
	  case '/':
		m << '\\' << ch;
		break;
	  case '\b':
		m << "\\b";
		break;
	  case '\t':
		m << "\\t";
		break;
	  case '\n':
		m << "\\n";
		break;
	  case '\f':
		m << "\\f";
		break;
	  case '\r':
		m << "\\r";
		break;
	  default:
		if (ch < ' ') {
		  m << "\\u00" << ::testing::internal::String::FormatByte(static_cast<unsigned char>(ch));
		} else {
		  m << ch;
		}
		break;
	}
  }

  return m.GetString();
}

void JsonUnitTestResultPrinter::OutputJsonKey(
	std::ostream* stream,
	const std::string& element_name,
	const std::string& name,
	const std::string& value,
	const std::string& indent,
	bool comma) {
  const std::vector<std::string>& allowed_names = GetReservedOutputAttributesForElement(element_name);

  GTEST_CHECK_(std::find(allowed_names.begin(), allowed_names.end(), name) !=
				   allowed_names.end())
	  << "Key \"" << name << "\" is not allowed for value \"" << element_name
	  << "\".";

  *stream << indent << "\"" << name << "\": \"" << EscapeJson(value) << "\"";
  if (comma)
	*stream << ",\n";
}

void JsonUnitTestResultPrinter::OutputJsonKey(
	std::ostream* stream,
	const std::string& element_name,
	const std::string& name,
	int value,
	const std::string& indent,
	bool comma) {
  const std::vector<std::string>& allowed_names = GetReservedOutputAttributesForElement(element_name);

  GTEST_CHECK_(std::find(allowed_names.begin(), allowed_names.end(), name) !=
				   allowed_names.end())
	  << "Key \"" << name << "\" is not allowed for value \"" << element_name
	  << "\".";

  *stream << indent << "\"" << name << "\": " << function_Streamable_to_string::StreamableToString(value);
  if (comma)
	*stream << ",\n";
}

// Prints a JSON representation of a TestInfo object.
void JsonUnitTestResultPrinter::OutputJsonTestInfo(::std::ostream* stream,
												   const char* test_suite_name,
												   const TestInfo& test_info) {
  const TestResult& result = *test_info.result();
  const std::string kTestsuite = "testcase";
  const std::string kIndent = Indent(10);

  *stream << Indent(8) << "{\n";
  OutputJsonKey(stream, kTestsuite, "name", test_info.name(), kIndent);

  if (test_info.value_param() != nullptr) {
	OutputJsonKey(stream, kTestsuite, "value_param", test_info.value_param(),
				  kIndent);
  }
  if (test_info.type_param() != nullptr) {
	OutputJsonKey(stream, kTestsuite, "type_param", test_info.type_param(),
				  kIndent);
  }
  if ( ::testing:: GTEST_FLAG(list_tests)) {
	OutputJsonKey(stream, kTestsuite, "file", test_info.file(), kIndent);
	OutputJsonKey(stream, kTestsuite, "line", test_info.line(), kIndent, false);
	*stream << "\n" << Indent(8) << "}";
	return;
  }

  OutputJsonKey( stream, kTestsuite, "status", test_info.should_run() ? "RUN" : "NOTRUN", kIndent );
  OutputJsonKey( stream, kTestsuite, "result", test_info.should_run() ? ( result.Skipped() ? "SKIPPED" : "COMPLETED" ) : "SUPPRESSED", kIndent );
  OutputJsonKey( stream, kTestsuite, "timestamp", Format_time::FormatEpochTimeInMillisAsRFC3339( result.start_timestamp() ), kIndent );
  OutputJsonKey( stream, kTestsuite, "time", Format_time::FormatTimeInMillisAsDuration(result.elapsed_time()), kIndent );
  OutputJsonKey( stream, kTestsuite, "classname", test_suite_name, kIndent, false );
  *stream << TestPropertiesAsJson(result, kIndent);

  int failures = 0;
  for (int i = 0; i < result.total_part_count(); ++i) {
	const ::testing::TestPartResult& part = result.GetTestPartResult(i);
	if (part.failed()) {
	  *stream << ",\n";
	  if (++failures == 1) {
		*stream << kIndent << "\"" << "failures" << "\": [\n";
	  }
	  const std::string location =
		  ::testing::internal::FormatCompilerIndependentFileLocation(part.file_name(),
														  part.line_number());
	  const std::string message = EscapeJson(location + "\n" + part.message());
	  *stream << kIndent << "  {\n"
			  << kIndent << "    \"failure\": \"" << message << "\",\n"
			  << kIndent << "    \"type\": \"\"\n"
			  << kIndent << "  }";
	}
  }

  if (failures > 0)
	*stream << "\n" << kIndent << "]";
  *stream << "\n" << Indent(8) << "}";
}

// Prints an JSON representation of a TestSuite object
void JsonUnitTestResultPrinter::PrintJsonTestSuite(
	std::ostream* stream, const ::jmsd::cutf::TestSuite& test_suite) {
  const std::string kTestsuite = "testsuite";
  const std::string kIndent = Indent(6);

  *stream << Indent(4) << "{\n";
  OutputJsonKey(stream, kTestsuite, "name", test_suite.name(), kIndent);
  OutputJsonKey(stream, kTestsuite, "tests", test_suite.reportable_test_count(),
				kIndent);
  if (!::testing:: GTEST_FLAG(list_tests)) {
	OutputJsonKey(stream, kTestsuite, "failures",
				  test_suite.failed_test_count(), kIndent);
	OutputJsonKey(stream, kTestsuite, "disabled",
				  test_suite.reportable_disabled_test_count(), kIndent);
	OutputJsonKey(stream, kTestsuite, "errors", 0, kIndent);
	OutputJsonKey(
		stream, kTestsuite, "timestamp",
		Format_time::FormatEpochTimeInMillisAsRFC3339(test_suite.start_timestamp()),
		kIndent);
	OutputJsonKey(stream, kTestsuite, "time",
				  Format_time::FormatTimeInMillisAsDuration(test_suite.elapsed_time()),
				  kIndent, false);
	*stream << TestPropertiesAsJson(test_suite.ad_hoc_test_result(), kIndent)
			<< ",\n";
  }

  *stream << kIndent << "\"" << kTestsuite << "\": [\n";

  bool comma = false;
  for (int i = 0; i < test_suite.total_test_count(); ++i) {
	if (test_suite.GetTestInfo(i)->is_reportable()) {
	  if (comma) {
		*stream << ",\n";
	  } else {
		comma = true;
	  }
	  OutputJsonTestInfo(stream, test_suite.name(), *test_suite.GetTestInfo(i));
	}
  }
  *stream << "\n" << kIndent << "]\n" << Indent(4) << "}";
}

// Prints a JSON summary of unit_test to output stream out.
void JsonUnitTestResultPrinter::PrintJsonUnitTest(std::ostream* stream,
												  const ::jmsd::cutf::UnitTest& unit_test) {
  const std::string kTestsuites = "testsuites";
  const std::string kIndent = Indent(2);
  *stream << "{\n";

  OutputJsonKey(stream, kTestsuites, "tests", unit_test.reportable_test_count(),
				kIndent);
  OutputJsonKey(stream, kTestsuites, "failures", unit_test.failed_test_count(),
				kIndent);
  OutputJsonKey(stream, kTestsuites, "disabled",
				unit_test.reportable_disabled_test_count(), kIndent);
  OutputJsonKey(stream, kTestsuites, "errors", 0, kIndent);
  if ( ::testing:: GTEST_FLAG(shuffle)) {
	OutputJsonKey(stream, kTestsuites, "random_seed", unit_test.random_seed(),
				  kIndent);
  }
  OutputJsonKey(stream, kTestsuites, "timestamp",
				Format_time::FormatEpochTimeInMillisAsRFC3339(unit_test.start_timestamp()),
				kIndent);
  OutputJsonKey(stream, kTestsuites, "time",
				Format_time::FormatTimeInMillisAsDuration(unit_test.elapsed_time()), kIndent,
				false);

  *stream << TestPropertiesAsJson(unit_test.ad_hoc_test_result(), kIndent)
		  << ",\n";

  OutputJsonKey(stream, kTestsuites, "name", "AllTests", kIndent);
  *stream << kIndent << "\"" << kTestsuites << "\": [\n";

  bool comma = false;
  for (int i = 0; i < unit_test.total_test_suite_count(); ++i) {
	if (unit_test.GetTestSuite(i)->reportable_test_count() > 0) {
	  if (comma) {
		*stream << ",\n";
	  } else {
		comma = true;
	  }
	  PrintJsonTestSuite(stream, *unit_test.GetTestSuite(i));
	}
  }

  *stream << "\n" << kIndent << "]\n" << "}\n";
}

void JsonUnitTestResultPrinter::PrintJsonTestList(
	std::ostream* stream, const std::vector<::jmsd::cutf::TestSuite*>& test_suites) {
  const std::string kTestsuites = "testsuites";
  const std::string kIndent = Indent(2);
  *stream << "{\n";
  int total_tests = 0;
  for (auto test_suite : test_suites) {
	total_tests += test_suite->total_test_count();
  }
  OutputJsonKey(stream, kTestsuites, "tests", total_tests, kIndent);

  OutputJsonKey(stream, kTestsuites, "name", "AllTests", kIndent);
  *stream << kIndent << "\"" << kTestsuites << "\": [\n";

  for (size_t i = 0; i < test_suites.size(); ++i) {
	if (i != 0) {
	  *stream << ",\n";
	}
	PrintJsonTestSuite(stream, *test_suites[i]);
  }

  *stream << "\n"
		  << kIndent << "]\n"
		  << "}\n";
}
// Produces a string representing the test properties in a result as
// a JSON dictionary.
std::string JsonUnitTestResultPrinter::TestPropertiesAsJson( const TestResult& result, const std::string& indent) {
  Message attributes;
  for (int i = 0; i < result.test_property_count(); ++i) {
	const TestProperty& property = result.GetTestProperty(i);
	attributes << ",\n" << indent << "\"" << property.key() << "\": "
			   << "\"" << EscapeJson(property.value()) << "\"";
  }
  return attributes.GetString();
}


} // namespace internal
} // namespace cutf
} // namespace jmsd
