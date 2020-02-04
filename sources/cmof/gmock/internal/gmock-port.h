#pragma once

// Low-level types and utilities for porting Google Mock to various
// platforms.  All macros ending with _ and symbols defined in an
// internal namespace are subject to change without notice.  Code
// outside Google Mock MUST NOT USE THEM DIRECTLY.  Macros that don't
// end with _ are part of Google Mock's public API and can be used by
// code outside Google Mock.

// Most of the utilities needed for porting Google Mock are also
// required for Google Test and are defined in gtest-port.h.
//
// Note to maintainers: to reduce code duplication, prefer adding
// portability utilities to Google Test's gtest-port.h instead of
// here, as Google Mock depends on Google Test.  Only add a utility
// here if it's truly specific to Google Mock.

#include "gtest/internal/gtest-port.h"
#include "gmock/internal/custom/gmock-port.h"


// For MS Visual C++, check the compiler version. At least VS 2015 is
// required to compile Google Mock.
#if defined(_MSC_VER) && _MSC_VER < 1900
# error "At least Visual C++ 2015 (14.0) is required to compile Google Mock."
#endif

// JMSD_DEPRECATED_GMOCK_API_ qualifies all symbols that must be exported. The definitions below
// are guarded by #ifndef to give embedders a chance to define JMSD_DEPRECATED_GMOCK_API_ in
// gmock/internal/custom/gtest-port.h

#if defined( _MSC_VER )
	#if defined( JMSD_DEPRECATED_GMOCK_CREATE_SHARED_LIBRARY )
		#define JMSD_DEPRECATED_GMOCK_API_		__declspec( dllexport )
	#else
		#define JMSD_DEPRECATED_GMOCK_API_		__declspec( dllimport )
	#endif
#elif ( defined( __GNUC__ ) && __GNUC__ >= 4 ) || defined( __clang__ )
	#define JMSD_DEPRECATED_GMOCK_API_			__attribute__( ( visibility( "default" ) ) )
#else // _MSC_VER
	#error
	#define JMSD_DEPRECATED_GMOCK_API_
#endif

//#ifndef GMOCK_API_
//	#ifdef _MSC_VER
//		#if GMOCK_LINKED_AS_SHARED_LIBRARY
//			#define GMOCK_API_ __declspec(dllimport)
//		#elif JMSD_DEPRECATED_GMOCK_CREATE_SHARED_LIBRARY
//			#define GMOCK_API_ __declspec(dllexport)
//		#endif
//	#elif __GNUC__ >= 4 || defined(__clang__)
//		#define GMOCK_API_ __attribute__((visibility ("default")))
//	#endif  // _MSC_VER
//#endif  // GMOCK_API_

//#ifndef GMOCK_API_
//	#define GMOCK_API_
//#endif  // GMOCK_API_

// Macro for referencing flags.  This is public as we want the user to
// use this syntax to reference Google Mock flags.
#define GMOCK_FLAG(name) FLAGS_gmock_##name

#if !defined(GMOCK_DECLARE_bool_)

// Macros for declaring flags.
# define GMOCK_DECLARE_bool_(name) extern JMSD_DEPRECATED_GMOCK_API_ bool GMOCK_FLAG(name)
# define GMOCK_DECLARE_int32_(name) extern JMSD_DEPRECATED_GMOCK_API_ int32_t GMOCK_FLAG(name)
# define GMOCK_DECLARE_string_(name) extern JMSD_DEPRECATED_GMOCK_API_ ::std::string GMOCK_FLAG(name)

// Macros for defining flags.
# define GMOCK_DEFINE_bool_(name, default_val, doc) JMSD_DEPRECATED_GMOCK_API_ bool GMOCK_FLAG(name) = (default_val)
# define GMOCK_DEFINE_int32_(name, default_val, doc) JMSD_DEPRECATED_GMOCK_API_ int32_t GMOCK_FLAG(name) = (default_val)
# define GMOCK_DEFINE_string_(name, default_val, doc) JMSD_DEPRECATED_GMOCK_API_ ::std::string GMOCK_FLAG(name) = (default_val)

#endif  // !defined(GMOCK_DECLARE_bool_)
