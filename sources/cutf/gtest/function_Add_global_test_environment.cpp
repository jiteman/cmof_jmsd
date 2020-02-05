#include "function_Add_global_test_environment.h"


#include "Unit_test.h"


namespace jmsd {
namespace cutf {


// A convenient wrapper for adding an environment for the test
// program.
//
// You should call this before RUN_ALL_TESTS() is called, probably in
// main().  If you use gtest_main, you need to call this before main()
// starts for it to take effect.  For example, you can define a global
// variable like this:
//
//   testing::Environment* const foo_env =
//       testing::AddGlobalTestEnvironment(new FooEnvironment);
//
// However, we strongly recommend you to write your own main() and
// call AddGlobalTestEnvironment() there, as relying on initialization
// of global variables makes the code harder to read and may cause
// problems when you register multiple environments from different
// translation units and the environments have dependencies among them
// (remember that the compiler doesn't guarantee the order in which
// global variables from different translation units are initialized).
// static
Environment *function_Add_global_test_environment::AddGlobalTestEnvironment( Environment *const env ) {
	return UnitTest::GetInstance()->AddEnvironment( env );
}


} // namespace cutf
} // namespace jmsd
