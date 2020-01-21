#pragma once

#include "Json_test_result_printer.hxx"


#include "gtest/Empty_test_event_listener.h"

#include "gtest/Test_result.hxx"

#include <string>


namespace jmsd {
namespace cutf {
namespace internal {


// This class generates an JSON output file.
class JsonUnitTestResultPrinter : public EmptyTestEventListener {
 public:
  explicit JsonUnitTestResultPrinter(const char* output_file);

  void OnTestIterationEnd(const ::jmsd::cutf::UnitTest& unit_test, int iteration) override;

  // Prints an JSON summary of all unit tests.
  static void PrintJsonTestList(::std::ostream* stream,
								const std::vector<::jmsd::cutf::TestSuite*>& test_suites);

private:
	static ::std::string Indent( size_t width );

  // Returns an JSON-escaped copy of the input string str.
  static std::string EscapeJson(const std::string& str);

  //// Verifies that the given attribute belongs to the given element and
  //// streams the attribute as JSON.
  static void OutputJsonKey(std::ostream* stream,
							const std::string& element_name,
							const std::string& name,
							const std::string& value,
							const std::string& indent,
							bool comma = true);
  static void OutputJsonKey(std::ostream* stream,
							const std::string& element_name,
							const std::string& name,
							int value,
							const std::string& indent,
							bool comma = true);

  // Streams a JSON representation of a TestInfo object.
  static void OutputJsonTestInfo(::std::ostream* stream,
								 const char* test_suite_name,
								 const ::jmsd::cutf::TestInfo& test_info);

  // Prints a JSON representation of a TestSuite object
  static void PrintJsonTestSuite(::std::ostream* stream,
								 const ::jmsd::cutf::TestSuite& test_suite);

  // Prints a JSON summary of unit_test to output stream out.
  static void PrintJsonUnitTest(::std::ostream* stream,
								const ::jmsd::cutf::UnitTest& unit_test);

  // Produces a string representing the test properties in a result as
  // a JSON dictionary.
  static std::string TestPropertiesAsJson(const TestResult& result,
										  const std::string& indent);

  // The output file.
  const std::string output_file_;

  GTEST_DISALLOW_COPY_AND_ASSIGN_(JsonUnitTestResultPrinter);
};



} // namespace internal
} // namespace cutf
} // namespace jmsd
