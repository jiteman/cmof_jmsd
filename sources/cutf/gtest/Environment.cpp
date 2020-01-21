#include "Environment.h"


namespace jmsd {
namespace cutf {


Environment::~Environment()
{}

// Override this to define how to set up the environment.
void Environment::SetUp()
{}

// Override this to define how to tear down the environment.
void Environment::TearDown()
{}

// If you see an error about overriding the following function or
// about it being private, you have mis-spelled SetUp() as Setup().
struct Environment::Setup_should_be_spelled_SetUp
{};

Environment::Setup_should_be_spelled_SetUp *Environment::Setup() {
	return nullptr;
}


} // namespace cutf
} // namespace jmsd
