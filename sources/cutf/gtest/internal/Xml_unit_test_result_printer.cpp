#include "Xml_unit_test_result_printer.h"


#include "gtest/gtest-message.h"
#include "gtest/Text_output_utilities.h"
#include "gtest/Test_info.h"
#include "gtest-string.h"

#include "Open_file_for_writing.h"


namespace jmsd {
namespace cutf {
namespace internal {


XmlUnitTestResultPrinter::XmlUnitTestResultPrinter(const char* output_file)
	: output_file_(output_file) {
  if (output_file_.empty()) {
	GTEST_LOG_(FATAL) << "XML output file may not be null";
  }
}

// Called after the unit test ends.
void XmlUnitTestResultPrinter::OnTestIterationEnd(const ::jmsd::cutf::UnitTest& unit_test,
												  int /*iteration*/) {
  FILE* xmlout = OpenFileForWriting(output_file_);
  std::stringstream stream;
  PrintXmlUnitTest(&stream, unit_test);
  fprintf(xmlout, "%s", ::testing::internal::StringStreamToString(&stream).c_str());
  fclose(xmlout);
}

void XmlUnitTestResultPrinter::ListTestsMatchingFilter(
	const std::vector<::jmsd::cutf::TestSuite*>& test_suites) {
  FILE* xmlout = OpenFileForWriting(output_file_);
  std::stringstream stream;
  PrintXmlTestsList(&stream, test_suites);
  fprintf(xmlout, "%s", ::testing::internal::StringStreamToString(&stream).c_str());
  fclose(xmlout);
}

// Returns an XML-escaped copy of the input string str.  If is_attribute
// is true, the text is meant to appear as an attribute value, and
// normalizable whitespace is preserved by replacing it with character
// references.
//
// Invalid XML characters in str, if any, are stripped from the output.
// It is expected that most, if not all, of the text processed by this
// module will consist of ordinary English text.
// If this module is ever modified to produce version 1.1 XML output,
// most invalid characters can be retained using character references.
std::string XmlUnitTestResultPrinter::EscapeXml( ::std::string const &str, bool const is_attribute ) {
  ::testing::Message m;

  for (size_t i = 0; i < str.size(); ++i) {
	const char ch = str[i];
	switch (ch) {
	  case '<':
		m << "&lt;";
		break;
	  case '>':
		m << "&gt;";
		break;
	  case '&':
		m << "&amp;";
		break;
	  case '\'':
		if (is_attribute)
		  m << "&apos;";
		else
		  m << '\'';
		break;
	  case '"':
		if (is_attribute)
		  m << "&quot;";
		else
		  m << '"';
		break;
	  default:
		if (IsValidXmlCharacter(ch)) {
		  if (is_attribute && IsNormalizableWhitespace(ch))
			m << "&#x" << ::testing::internal::String::FormatByte(static_cast<unsigned char>(ch))
			  << ";";
		  else
			m << ch;
		}
		break;
	}
  }

  return m.GetString();
}

// Returns the given string with all characters invalid in XML removed.
// Currently invalid characters are dropped from the string. An
// alternative is to replace them with certain characters such as . or ?.
std::string XmlUnitTestResultPrinter::RemoveInvalidXmlCharacters(
	const std::string& str) {
  std::string output;
  output.reserve(str.size());
  for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
	if (IsValidXmlCharacter(*it))
	  output.push_back(*it);

  return output;
}

// Streams an XML CDATA section, escaping invalid CDATA sequences as needed.
void XmlUnitTestResultPrinter::OutputXmlCDataSection(::std::ostream* stream,
													 const char* data) {
  const char* segment = data;
  *stream << "<![CDATA[";
  for (;;) {
	const char* const next_segment = strstr(segment, "]]>");
	if (next_segment != nullptr) {
	  stream->write(
		  segment, static_cast<std::streamsize>(next_segment - segment));
	  *stream << "]]>]]&gt;<![CDATA[";
	  segment = next_segment + strlen("]]>");
	} else {
	  *stream << segment;
	  break;
	}
  }
  *stream << "]]>";
}

void XmlUnitTestResultPrinter::OutputXmlAttribute(
	std::ostream* stream,
	const std::string& element_name,
	const std::string& name,
	const std::string& value)
{
  const std::vector<std::string>& allowed_names = GetReservedOutputAttributesForElement(element_name);

  GTEST_CHECK_(std::find(allowed_names.begin(), allowed_names.end(), name) !=
				   allowed_names.end())
	  << "Attribute " << name << " is not allowed for element <" << element_name
	  << ">.";

  *stream << " " << name << "=\"" << EscapeXmlAttribute(value) << "\"";
}

// Prints an XML representation of a TestInfo object.
void XmlUnitTestResultPrinter::OutputXmlTestInfo(::std::ostream* stream,
												 const char* test_suite_name,
												 const TestInfo& test_info) {
  const TestResult& result = *test_info.result();
  const std::string kTestsuite = "testcase";

  if (test_info.is_in_another_shard()) {
	return;
  }

  *stream << "    <testcase";
  OutputXmlAttribute(stream, kTestsuite, "name", test_info.name());

  if (test_info.value_param() != nullptr) {
	OutputXmlAttribute(stream, kTestsuite, "value_param",
					   test_info.value_param());
  }
  if (test_info.type_param() != nullptr) {
	OutputXmlAttribute(stream, kTestsuite, "type_param",
					   test_info.type_param());
  }
  if (GTEST_FLAG(list_tests)) {
	OutputXmlAttribute(stream, kTestsuite, "file", test_info.file());
	OutputXmlAttribute(stream, kTestsuite, "line",
					   StreamableToString(test_info.line()));
	*stream << " />\n";
	return;
  }

  OutputXmlAttribute(stream, kTestsuite, "status",
					 test_info.should_run() ? "run" : "notrun");
  OutputXmlAttribute(stream, kTestsuite, "result",
					 test_info.should_run()
						 ? (result.Skipped() ? "skipped" : "completed")
						 : "suppressed");
  OutputXmlAttribute(stream, kTestsuite, "time",
					 FormatTimeInMillisAsSeconds(result.elapsed_time()));
  OutputXmlAttribute(
	  stream, kTestsuite, "timestamp",
	  FormatEpochTimeInMillisAsIso8601(result.start_timestamp()));
  OutputXmlAttribute(stream, kTestsuite, "classname", test_suite_name);

  int failures = 0;
  for (int i = 0; i < result.total_part_count(); ++i) {
	const TestPartResult& part = result.GetTestPartResult(i);
	if (part.failed()) {
	  if (++failures == 1) {
		*stream << ">\n";
	  }
	  const std::string location =
		  internal::FormatCompilerIndependentFileLocation(part.file_name(),
														  part.line_number());
	  const std::string summary = location + "\n" + part.summary();
	  *stream << "      <failure message=\""
			  << EscapeXmlAttribute(summary.c_str())
			  << "\" type=\"\">";
	  const std::string detail = location + "\n" + part.message();
	  OutputXmlCDataSection(stream, RemoveInvalidXmlCharacters(detail).c_str());
	  *stream << "</failure>\n";
	}
  }

  if (failures == 0 && result.test_property_count() == 0) {
	*stream << " />\n";
  } else {
	if (failures == 0) {
	  *stream << ">\n";
	}
	OutputXmlTestProperties(stream, result);
	*stream << "    </testcase>\n";
  }
}

// Prints an XML representation of a TestSuite object
void XmlUnitTestResultPrinter::PrintXmlTestSuite(std::ostream* stream,
												 const ::jmsd::cutf::TestSuite& test_suite) {
  const std::string kTestsuite = "testsuite";
  *stream << "  <" << kTestsuite;
  OutputXmlAttribute(stream, kTestsuite, "name", test_suite.name());
  OutputXmlAttribute(stream, kTestsuite, "tests",
					 StreamableToString(test_suite.reportable_test_count()));
  if (!GTEST_FLAG(list_tests)) {
	OutputXmlAttribute(stream, kTestsuite, "failures",
					   StreamableToString(test_suite.failed_test_count()));
	OutputXmlAttribute(
		stream, kTestsuite, "disabled",
		StreamableToString(test_suite.reportable_disabled_test_count()));
	OutputXmlAttribute(stream, kTestsuite, "errors", "0");
	OutputXmlAttribute(stream, kTestsuite, "time",
					   FormatTimeInMillisAsSeconds(test_suite.elapsed_time()));
	OutputXmlAttribute(
		stream, kTestsuite, "timestamp",
		FormatEpochTimeInMillisAsIso8601(test_suite.start_timestamp()));
	*stream << TestPropertiesAsXmlAttributes(test_suite.ad_hoc_test_result());
  }
  *stream << ">\n";
  for (int i = 0; i < test_suite.total_test_count(); ++i) {
	if (test_suite.GetTestInfo(i)->is_reportable())
	  OutputXmlTestInfo(stream, test_suite.name(), *test_suite.GetTestInfo(i));
  }
  *stream << "  </" << kTestsuite << ">\n";
}

// Prints an XML summary of unit_test to output stream out.
void XmlUnitTestResultPrinter::PrintXmlUnitTest(std::ostream* stream,
												const ::jmsd::cutf::UnitTest& unit_test) {
  const std::string kTestsuites = "testsuites";

  *stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  *stream << "<" << kTestsuites;

  OutputXmlAttribute(stream, kTestsuites, "tests",
					 StreamableToString(unit_test.reportable_test_count()));
  OutputXmlAttribute(stream, kTestsuites, "failures",
					 StreamableToString(unit_test.failed_test_count()));
  OutputXmlAttribute(
	  stream, kTestsuites, "disabled",
	  StreamableToString(unit_test.reportable_disabled_test_count()));
  OutputXmlAttribute(stream, kTestsuites, "errors", "0");
  OutputXmlAttribute(stream, kTestsuites, "time",
					 FormatTimeInMillisAsSeconds(unit_test.elapsed_time()));
  OutputXmlAttribute(
	  stream, kTestsuites, "timestamp",
	  FormatEpochTimeInMillisAsIso8601(unit_test.start_timestamp()));

  if (GTEST_FLAG(shuffle)) {
	OutputXmlAttribute(stream, kTestsuites, "random_seed",
					   StreamableToString(unit_test.random_seed()));
  }
  *stream << TestPropertiesAsXmlAttributes(unit_test.ad_hoc_test_result());

  OutputXmlAttribute(stream, kTestsuites, "name", "AllTests");
  *stream << ">\n";

  for (int i = 0; i < unit_test.total_test_suite_count(); ++i) {
	if (unit_test.GetTestSuite(i)->reportable_test_count() > 0)
	  PrintXmlTestSuite(stream, *unit_test.GetTestSuite(i));
  }
  *stream << "</" << kTestsuites << ">\n";
}

void XmlUnitTestResultPrinter::PrintXmlTestsList(
	std::ostream* stream, const std::vector<::jmsd::cutf::TestSuite*>& test_suites) {
  const std::string kTestsuites = "testsuites";

  *stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  *stream << "<" << kTestsuites;

  int total_tests = 0;
  for (auto test_suite : test_suites) {
	total_tests += test_suite->total_test_count();
  }
  OutputXmlAttribute(stream, kTestsuites, "tests",
					 StreamableToString(total_tests));
  OutputXmlAttribute(stream, kTestsuites, "name", "AllTests");
  *stream << ">\n";

  for (auto test_suite : test_suites) {
	PrintXmlTestSuite(stream, *test_suite);
  }
  *stream << "</" << kTestsuites << ">\n";
}

// Produces a string representing the test properties in a result as space
// delimited XML attributes based on the property key="value" pairs.
std::string XmlUnitTestResultPrinter::TestPropertiesAsXmlAttributes(
	const ::jmsd::cutf::TestResult& result) {
  Message attributes;
  for (int i = 0; i < result.test_property_count(); ++i) {
	const ::jmsd::cutf::TestProperty& property = result.GetTestProperty(i);
	attributes << " " << property.key() << "="
		<< "\"" << EscapeXmlAttribute(property.value()) << "\"";
  }
  return attributes.GetString();
}

void XmlUnitTestResultPrinter::OutputXmlTestProperties(
	std::ostream* stream, const ::jmsd::cutf::TestResult& result) {
  const std::string kProperties = "properties";
  const std::string kProperty = "property";

  if (result.test_property_count() <= 0) {
	return;
  }

  *stream << "<" << kProperties << ">\n";
  for (int i = 0; i < result.test_property_count(); ++i) {
	const ::jmsd::cutf::TestProperty& property = result.GetTestProperty(i);
	*stream << "<" << kProperty;
	*stream << " name=\"" << EscapeXmlAttribute(property.key()) << "\"";
	*stream << " value=\"" << EscapeXmlAttribute(property.value()) << "\"";
	*stream << "/>\n";
  }
  *stream << "</" << kProperties << ">\n";
}


} // namespace internal
} // namespace cutf
} // namespace jmsd
