#include "ctf_test_main.h"

//#include "jmsd/ctf/ctf_library_main.h"
#include "jmsd/cutf/tests/cutf_test_main.h"
#include "jmsd/cmof/tests/cmof_test_main.h"


namespace jmsd {
namespace ctf {
namespace tests {


int run_all_ctf_tests( int const argc, char const *const argv[] ) {
//	return ctf::ctf_main( argc, argv, false );

	int const cutf_test_result = cutf::tests::run_all_cutf_tests( argc, argv );

	if ( cutf_test_result != 0 ) {
		return cutf_test_result;
	}

	return cmof::tests::run_all_cmof_tests( argc, argv );
}


} // namespace tests
} // namespace ctf
} // namespace jmsd
