#include "function_Print_test_part_result_to_string.h"


#include "gtest/Test_part_result_to_string.h"
#include "gtest/Message.hin"


namespace jmsd {
namespace cutf {
namespace internal {


// Prints a TestPartResult to an std::string.
// static
::std::string function_Print_test_part_result_to_string::PrintTestPartResultToString( ::testing::TestPartResult const &test_part_result ) {
  return
	( Message() <<
		  ::testing::internal::FormatFileLocation( test_part_result.file_name(), test_part_result.line_number() ) <<
		  " " <<
		  ::jmsd::cutf::TestPartResultTypeToString( test_part_result.type() ) <<
		  test_part_result.message() ).GetString();
}


} // namespace internal
} // namespace cutf
} // namespace jmsd
