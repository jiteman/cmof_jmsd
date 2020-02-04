// This is the main header file a user should include.

// This file implements the following syntax:
//
//   ON_CALL(mock_object, Method(...))
//     .With(...) ?
//     .WillByDefault(...);
//
// where With() is optional and WillByDefault() must appear exactly
// once.
//
//   EXPECT_CALL(mock_object, Method(...))
//     .With(...) ?
//     .Times(...) ?
//     .InSequence(...) *
//     .WillOnce(...) *
//     .WillRepeatedly(...) ?
//     .RetiresOnSaturation() ? ;
//
// where all clauses are optional and WillOnce() can be repeated.

#include "gmock/gmock-actions.h"
#include "gmock/gmock-cardinalities.h"
#include "gmock/gmock-function-mocker.h"
#include "gmock/gmock-generated-actions.h"
#include "gmock/gmock-generated-function-mockers.h"
#include "gmock/gmock-generated-matchers.h"
#include "gmock/gmock-matchers.h"
#include "gmock/gmock-more-actions.h"
#include "gmock/gmock-more-matchers.h"
#include "gmock/gmock-nice-strict.h"
#include "gmock/internal/gmock-internal-utils.h"

namespace testing {

// Declares Google Mock flags that we want a user to use programmatically.
GMOCK_DECLARE_bool_(catch_leaked_mocks);
GMOCK_DECLARE_string_(verbose);
GMOCK_DECLARE_int32_(default_mock_behavior);

// Initializes Google Mock.  This must be called before running the
// tests.  In particular, it parses the command line for the flags
// that Google Mock recognizes.  Whenever a Google Mock flag is seen,
// it is removed from argv, and *argc is decremented.
//
// No value is returned.  Instead, the Google Mock flag variables are
// updated.
//
// Since Google Test is needed for Google Mock to work, this function
// also initializes Google Test and parses its flags, if that hasn't
// been done.
JMSD_DEPRECATED_GMOCK_API_ void InitGoogleMock(int* argc, char** argv);

// This overloaded version can be used in Windows programs compiled in
// UNICODE mode.
JMSD_DEPRECATED_GMOCK_API_ void InitGoogleMock(int* argc, wchar_t** argv);

// This overloaded version can be used on Arduino/embedded platforms where
// there is no argc/argv.
JMSD_DEPRECATED_GMOCK_API_ void InitGoogleMock();

}  // namespace testing
