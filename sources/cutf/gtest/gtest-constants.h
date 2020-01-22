#pragma once

//#include "gtest-constants.hxx"


#include "cutf.h"


namespace jmsd {
namespace cutf {
namespace constants {


// Constants.

// A test whose test suite name or test name matches this filter is
// disabled and not run.
extern const char kDisableTestFilter[];

// A test suite whose name matches this filter is considered a death
// test suite and will be run before test suites whose name doesn't
// match this filter.
extern const char kDeathTestSuiteFilter[];

// A test filter that matches everything.
extern const char kUniversalFilter[];

// The default output format.
extern const char kDefaultOutputFormat[];
// The default output file.
extern const char kDefaultOutputFile[];

// The environment variable name for the test shard index.
extern const char kTestShardIndex[];
// The environment variable name for the total number of test shards.
extern const char kTestTotalShards[];
// The environment variable name for the test shard status file.
extern const char kTestShardStatusFile[];

// The upper limit for valid stack trace depths.
JMSD_CUTF_SHARED_INTERFACE extern const int kMaxStackTraceDepth;


} // namespace constants
} // namespace cutf
} // namespace jmsd
