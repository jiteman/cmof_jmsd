#pragma once

#include "Format_time.hxx"


#include <string>
#include <ctime>


#include "gtest-port.h"


#include "cutf.h"


namespace jmsd {
namespace cutf {
namespace internal {


class JMSD_CUTF_SHARED_INTERFACE Format_time {

	typedef Format_time THIS_STATIC;

public:
	static bool PortableLocaltime( ::std::time_t seconds, struct tm *out );

	static ::std::string FormatTimeInMillisAsSeconds( ::testing::internal::TimeInMillis ms );
	static ::std::string FormatTimeInMillisAsDuration( ::testing::internal::TimeInMillis ms );
	static ::std::string FormatEpochTimeInMillisAsIso8601( ::testing::internal::TimeInMillis ms );
	static ::std::string FormatEpochTimeInMillisAsRFC3339( ::testing::internal::TimeInMillis ms );


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
public:

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
private:
	virtual ~Format_time() noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	Format_time() noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	Format_time( Format_time const &another ) noexcept = delete;
	Format_time &operator =( Format_time const &another ) noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	Format_time( Format_time &&another ) noexcept = delete;
	Format_time &operator =( Format_time &&another ) noexcept = delete;

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
private:

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:

};


} // namespace internal
} // namespace cutf
} // namespace jmsd
