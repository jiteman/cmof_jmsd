#include "cmof_test_main.h"

#include "jmsd/ctf/ctf_library_main.h"


namespace jmsd {
namespace cmof {
namespace tests {


int run_all_cmof_tests( int argc, char const *const argv[] ) {
	return ctf::ctf_main( argc, argv, false );
}

int init_all_cmof_tests() {
	return 0;
}


} // namespace tests
} // namespace cmof
} // namespace jmsd
