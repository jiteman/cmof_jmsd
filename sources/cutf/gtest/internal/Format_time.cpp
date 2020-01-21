#include "Format_time.h"


#include "gtest-string.h"
#include "gtest/gtest-message.h" // for StreamableToString only !!!!!

#include <sstream>


namespace jmsd {
namespace cutf {
namespace internal {


// static
bool PortableLocaltime( time_t seconds, struct tm *out ) {
#if defined(_MSC_VER)
	return localtime_s(out, &seconds) == 0;
#elif defined(__MINGW32__) || defined(__MINGW64__)
	// MINGW <time.h> provides neither localtime_r nor localtime_s, but uses Windows' localtime(), which has a thread-local tm buffer.
	struct tm* tm_ptr = localtime( &seconds );
	if ( tm_ptr == nullptr ) return false;
	*out = *tm_ptr;
	return true;
#else
	return localtime_r( &seconds, out ) != nullptr;
#endif
}

// Formats the given time in milliseconds as seconds.
// static
std::string FormatTimeInMillisAsSeconds( ::testing::internal::TimeInMillis ms ) {
	::std::stringstream ss;
	ss << ( static_cast< double >( ms ) * 1e-3 );
	return ss.str();
}

// Converts the given epoch time in milliseconds to a date string in the ISO 8601 format, without the timezone information.
// static
std::string FormatEpochTimeInMillisAsIso8601( ::testing::internal::TimeInMillis ms ) {
  struct tm time_struct;
  if (!PortableLocaltime(static_cast<time_t>(ms / 1000), &time_struct))
	return "";
  // YYYY-MM-DDThh:mm:ss
  return ::testing::internal::StreamableToString(time_struct.tm_year + 1900) + "-" +
	  ::testing::internal::String::FormatIntWidth2(time_struct.tm_mon + 1) + "-" +
	  ::testing::internal::String::FormatIntWidth2(time_struct.tm_mday) + "T" +
	  ::testing::internal::String::FormatIntWidth2(time_struct.tm_hour) + ":" +
	  ::testing::internal::String::FormatIntWidth2(time_struct.tm_min) + ":" +
	  ::testing::internal::String::FormatIntWidth2(time_struct.tm_sec);
}

// Formats the given time in milliseconds as seconds.
// static
std::string FormatTimeInMillisAsDuration( ::testing::internal::TimeInMillis ms) {
  ::std::stringstream ss;
  ss << (static_cast<double>(ms) * 1e-3) << "s";
  return ss.str();
}

// Converts the given epoch time in milliseconds to a date string in the RFC3339 format, without the timezone information.
// static
std::string FormatEpochTimeInMillisAsRFC3339( ::testing::internal::TimeInMillis ms) {
  struct tm time_struct;
  if (!PortableLocaltime(static_cast<time_t>(ms / 1000), &time_struct))
	return "";
  // YYYY-MM-DDThh:mm:ss
  return ::testing::internal::StreamableToString(time_struct.tm_year + 1900) + "-" +
	  ::testing::internal::String::FormatIntWidth2(time_struct.tm_mon + 1) + "-" +
	  ::testing::internal::String::FormatIntWidth2(time_struct.tm_mday) + "T" +
	  ::testing::internal::String::FormatIntWidth2(time_struct.tm_hour) + ":" +
	  ::testing::internal::String::FormatIntWidth2(time_struct.tm_min) + ":" +
	  ::testing::internal::String::FormatIntWidth2(time_struct.tm_sec) + "Z";
}


} // namespace internal
} // namespace cutf
} // namespace jmsd
