#include "cmof_test_main.h"

#include "jmsd/ctf/ctf_library_main.h"


namespace jmsd {
namespace tpa {
namespace gtm {


int run_all_cmof_tests( int argc, char const *const argv[] ) {
	return ctf::ctf_main( argc, argv, false );
}


} // namespace gtm
} // namespace tpa
} // namespace jmsd
