#include "function_Int32_from_environment_or_die.h"

#include "gtest/Message.hin"


namespace jmsd {
namespace cutf {
namespace internal {


// Parses the environment variable var as an Int32.
// If it is unset, returns default_val.
// If it is not an Int32, prints an error and aborts.
// static
int32_t function_Int32_from_environment_or_die::Int32FromEnvOrDie(const char* var, int32_t default_val) {
  const char* str_val = ::testing::internal::posix::GetEnv(var);
  if (str_val == nullptr) {
	return default_val;
  }

  int32_t result;
  if (!::testing::internal::ParseInt32( Message() << "The value of environment variable " << var, str_val, &result)) {
	exit(EXIT_FAILURE);
  }
  return result;
}


} // namespace internal
} // namespace cutf
} // namespace jmsd
