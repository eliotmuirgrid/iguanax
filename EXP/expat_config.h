/*
   Hand crafted for iNTERFACEWARE
*/

#if defined(_WIN32)
   #include "expat_config_win32.h"
#elif defined(__APPLE__)
   #include "expat_config_mac.h"
#elif defined(__linux__)
   #if defined(__arm__) || defined(__aarch64__)
      #include "expat_config_linux_arm.h"
   #else
      #include "expat_config_linux_intel.h"
   #endif
#else
   #error "Platform does not have an include file for libexpat!"
#endif

