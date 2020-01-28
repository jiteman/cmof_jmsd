#include "Text_output_utilities.h"


#include "Text_output_attributes.h"

#include "Message.hin"

#include "gtest.h"

#include "internal/gtest-port.h"


namespace jmsd {
namespace cutf {


static ::std::vector< ::std::string > const empty_vector_of_strings;


::std::vector< ::std::string > const &GetReservedAttributesForElement( ::std::string const& element_name ) {
	if ( element_name == "testsuites" ) {
		return kReservedTestSuitesAttributes;
	} else if (element_name == "testsuite") {
		return kReservedTestSuiteAttributes;
	} else if (element_name == "testcase") {
		return kReservedTestCaseAttributes;
	} else {
		GTEST_CHECK_( false ) << "Unrecognized xml_element provided: " << element_name;
	}

	// This code is unreachable but some compilers may not realizes that.
	return empty_vector_of_strings;
}

// TODO(jdesprez): Merge the two getReserved attributes once skip is improved
::std::vector< ::std::string > const &GetReservedOutputAttributesForElement( ::std::string const &element_name ) {
	if ( element_name == "testsuites" ) {
		return kReservedTestSuitesAttributes;
	} else if (element_name == "testsuite") {
		return kReservedTestSuiteAttributes;
	} else if ( element_name == "testcase" ) {
		return kReservedOutputTestCaseAttributes;
	} else {
		GTEST_CHECK_( false ) << "Unrecognized xml_element provided: " << element_name;
	}

	// This code is unreachable but some compilers may not realizes that.
	return empty_vector_of_strings;
}

::std::string FormatWordList( ::std::vector< ::std::string > const &words ) {
	Message word_list;

	for ( size_t i = 0; i < words.size(); ++i ) {
		if ( i > 0 && words.size() > 2 ) {
			word_list << ", ";
		}

		if (i == words.size() - 1) {
			word_list << "and ";
		}

		word_list << "'" << words[ i ] << "'";
	}

	return word_list.GetString();
}

bool ValidateTestPropertyName( ::std::string const &property_name, ::std::vector< ::std::string > const &reserved_names ) {
  if (std::find(reserved_names.begin(), reserved_names.end(), property_name) !=
		  reserved_names.end()) {
	ADD_FAILURE() << "Reserved key used in RecordProperty(): " << property_name
				  << " (" << FormatWordList(reserved_names)
				  << " are reserved by " << GTEST_NAME_ << ")";
	return false;
  }
  return true;
}


} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
