#pragma once


#include <string>

#include "cutf.h"


namespace jmsd {
namespace cutf {
namespace internal {


// Returns the message describing the last system error in errno.
JMSD_CUTF_SHARED_INTERFACE ::std::string GetLastErrnoDescription();


} // namespace internal
} // namespace cutf
} // namespace jmsd
