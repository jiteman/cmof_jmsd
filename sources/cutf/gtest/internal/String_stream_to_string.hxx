#pragma once


#include <sstream>
#include <string>


#include "cutf.h"


namespace jmsd {
namespace cutf {
namespace internal {


// Converts the buffer in a stringstream to an std::string, converting NUL bytes to "\\0" along the way.
JMSD_CUTF_SHARED_INTERFACE ::std::string StringStreamToString( ::std::stringstream *ss );


} // namespace internal
} // namespace cutf
} // namespace jmsd
