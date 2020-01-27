#include "cutf_test_main.h"

#include "jmsd/ctf/ctf_library_main.h"


namespace jmsd {
namespace cutf {
namespace tests {


int run_all_cutf_tests( int argc, char const *const argv[] ) {
	return ctf::ctf_main( argc, argv, false );
}


} // namespace tests
} // namespace cutf
} // namespace jmsd
