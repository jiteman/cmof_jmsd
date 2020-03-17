#pragma once


#include "jmsd/platform_support/platform_detection/compiler_detection.h"


#if !defined( JMSD_MICROSOFT_VISUAL_CPP_COMPILER_IS_DETECTED )
	#error This file "jmsf/ms_windows/temporary_windows_include.h" is included, but there is no MS Windows detected.

#endif


#pragma warning( push )
#pragma warning( disable : 4668 ) // is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'


#define WIN32_LEAN_AND_MEAN
#include <windows.h>


#pragma warning( pop )


#if defined( max )
	#undef max
#endif

#if defined( min )
	#undef min
#endif

#if defined( small )
	#undef small
#endif
