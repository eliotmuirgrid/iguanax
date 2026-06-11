//{{NO_DEPENDENCIES}}
// Microsoft Visual C++ generated include file.
// Used by checkout.rc
#include "AppVersion.h"

#ifdef APP_RELEASE_BUILD
#define APP_VERSION_BETA 0
#else
#define APP_VERSION_BETA 1
#endif

// macro magic to inline constants
#define BID_ASSTR(x) #x
#define BID_STRINGIFY(x) BID_ASSTR(x)

#define APP_STRING_VERSION BID_STRINGIFY(APP_VERSION_MAJOR) "," BID_STRINGIFY(APP_VERSION_MINOR) "," BID_STRINGIFY(APP_BUILD_NUMBER) "," BID_STRINGIFY(APP_VERSION_BETA)

#define APP_RC_FILE_NAME "machine_id_check.exe"

// Next default values for new objects
// 
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NEXT_RESOURCE_VALUE        101
#define _APS_NEXT_COMMAND_VALUE         40001
#define _APS_NEXT_CONTROL_VALUE         1001
#define _APS_NEXT_SYMED_VALUE           101
#endif
#endif

