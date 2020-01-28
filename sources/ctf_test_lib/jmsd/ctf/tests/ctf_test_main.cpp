#include "ctf_test_main.h"

#include "jmsd/ctf/ctf_library_main.h"
#include "jmsd/cutf/tests/cutf_test_main.h"
#include "jmsd/cmof/tests/cmof_test_main.h"


namespace jmsd {
namespace ctf {
namespace tests {


int run_all_ctf_tests( int const argc, char const *const argv[] ) {
	int const cutf_init = cutf::tests::init_all_cutf_tests();

	if ( cutf_init != 0 ) return -1;

	int const cmof_init = cmof::tests::init_all_cmof_tests();

	if ( cmof_init != 0 ) return -1;

	return ctf::ctf_main( argc, argv, false );
}


} // namespace tests
} // namespace ctf
} // namespace jmsd
