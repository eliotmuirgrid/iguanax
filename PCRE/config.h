/*
   Hand crafted for iNTERFACEWARE
  */

#ifdef _WIN32
// Same for 32/64 bit
#include "config_win32.h"
#elif defined(__APPLE__)
// Just happens to be the same as linux so far
#include "config_mac_intel.h"
#elif defined(__linux__)
// No difference between Ubuntu/RHEL 32/64 bit - all 4 are the same
#include "config_linux.h"
#else
#warning "Platform does not have a config file for PCRE!"
// But the generic might be able to chug along anyway
#include "config_generic.h"
#endif
