#include "gtest-constants.h"


namespace jmsd {
namespace cutf {
namespace constants {


// Constants.

// A test whose test suite name or test name matches this filter is
// disabled and not run.
const char kDisableTestFilter[] = "DISABLED_*:*/DISABLED_*";

// A test suite whose name matches this filter is considered a death
// test suite and will be run before test suites whose name doesn't
// match this filter.
const char kDeathTestSuiteFilter[] = "*DeathTest:*DeathTest/*";

// A test filter that matches everything.
const char kUniversalFilter[] = "*";

// The default output format.
const char kDefaultOutputFormat[] = "xml";
// The default output file.
const char kDefaultOutputFile[] = "test_detail";

// The environment variable name for the test shard index.
const char kTestShardIndex[] = "GTEST_SHARD_INDEX";
// The environment variable name for the total number of test shards.
const char kTestTotalShards[] = "GTEST_TOTAL_SHARDS";
// The environment variable name for the test shard status file.
const char kTestShardStatusFile[] = "GTEST_SHARD_STATUS_FILE";

// The upper limit for valid stack trace depths.
const int kMaxStackTraceDepth = 100;


} // namespace constants
} // namespace cutf
} // namespace jmsd
