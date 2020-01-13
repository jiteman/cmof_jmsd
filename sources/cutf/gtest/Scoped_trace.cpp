#include "Scoped_trace.h"


#include "Unit_test.h"

#include "gtest-internal-inl.h"


namespace jmsd {
namespace cutf {


// Class ScopedTrace

// Pushes the given source file location and message onto a per-thread
// trace stack maintained by Google Test.
void ScopedTrace::PushTrace(const char* file, int line, std::string message) {
  ::testing::internal::TraceInfo trace;
  trace.file = file;
  trace.line = line;
  trace.message.swap(message);

  UnitTest::GetInstance()->PushGTestTrace(trace);
}

// Pops the info pushed by the c'tor.
ScopedTrace::~ScopedTrace()
	GTEST_LOCK_EXCLUDED_(&UnitTest::mutex_) {
  UnitTest::GetInstance()->PopGTestTrace();
}


} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
