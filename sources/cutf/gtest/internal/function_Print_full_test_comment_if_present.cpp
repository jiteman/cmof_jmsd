#include "function_Print_full_test_comment_if_present.h"


#include "gtest/Test_info.h"

#include "gtest-constants-internal.h"


namespace jmsd {
namespace cutf {
namespace internal {


// static
void function_Print_full_test_comment_if_present::PrintFullTestCommentIfPresent( TestInfo const &test_info ) {
	char const *const type_param = test_info.type_param();
	char const *const value_param = test_info.value_param();

	if ( type_param != nullptr || value_param != nullptr ) {
		::printf(", where ");

		if ( type_param != nullptr ) {
			::printf( "%s = %s", constants::internal::kTypeParamLabel, type_param );

			if ( value_param != nullptr ) {
				::printf(" and ");
			}
		}

		if ( value_param != nullptr ) {
			::printf( "%s = %s", constants::internal::kValueParamLabel, value_param );
		}
	}
}


} // namespace internal
} // namespace cutf
} // namespace jmsd
