#pragma once


#include "jmsd/platform_support/common_shared_library_support.h"


#if defined( JMSD_CMOF_SHARED_INTERFACE_INTERNAL )
	#define JMSD_CMOF_SHARED_INTERFACE		JMSD_SHARED_EXPORT_CONVENTION

#else
	#define JMSD_CMOF_SHARED_INTERFACE		JMSD_SHARED_IMPORT_CONVENTION

#endif
