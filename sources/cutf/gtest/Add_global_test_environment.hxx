#pragma once


namespace testing {


class Environment;


} // namespace testing


namespace jmsd {
namespace cutf {


::testing::Environment* AddGlobalTestEnvironment( ::testing::Environment* env);


} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
