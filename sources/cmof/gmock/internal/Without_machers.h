#pragma once

#include "Without_machers.hxx"


#include "cmof.h"


namespace jmsd {
namespace cmof {
namespace internal {


// A marker class that is used to resolve parameterless expectations to the correct overload.
// This must not be instantiable, to prevent client code from accidentally resolving to the overload; for example:
//    ON_CALL( mock, Method( {}, nullptr ) )...
class JMSD_CMOF_SHARED_INTERFACE WithoutMatchers {

public:
	static WithoutMatchers Get();

private:
	WithoutMatchers();

};

// eisha: ??? Singleton?!! Default constuctor is used there.
// Internal use only: access the singleton instance of WithoutMatchers.


} // namespace internal
} // namespace cmof
} // namespace jmsd
