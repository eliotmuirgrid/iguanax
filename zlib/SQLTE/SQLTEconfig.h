#ifndef __SQLITE_CONFIG_H__
#define __SQLITE_CONFIG_H__

#ifdef __APPLE__
// Our SQLite source code uses the deprecated OSAtomicCompareAndSwapPtrBarrier macOS API.
// We disable the error using #pragma here instead of compiler flag '-Wno-deprecated-declarations'.
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
// Note: this build error should be resolved in v3.18.0+
#endif

#define SQLITE_THREADSAFE 1
#define SQLITE_ENABLE_LOCKING_STYLE 0
#define SQLITE_ENABLE_MEMORY_MANAGEMENT 1
#define SQLITE_TEMP_STORE 3
#define SQLITE_ENABLE_FTS3_PARENTHESIS 1
#define SQLITE_ENABLE_FTS3 1
#define SQLITE_HAS_CODEC 1

#ifndef _WIN32
   /* these work on all POSIX platforms */
   #define HAVE_GMTIME_R      1
   #define HAVE_LOCALTIME_R   1
   #define HAVE_USLEEP        1
   #define USE_PREAD          1
#endif

#endif
