#pragma once


#include <string>
#include <vector>


namespace jmsd {
namespace cutf {


::std::vector< ::std::string > const &GetReservedAttributesForElement( ::std::string const& element_name );
::std::vector< ::std::string > const &GetReservedOutputAttributesForElement( ::std::string const &element_name );
::std::string FormatWordList( ::std::vector< ::std::string > const &words );
bool ValidateTestPropertyName( ::std::string const &property_name, ::std::vector< ::std::string > const &reserved_names );


} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
