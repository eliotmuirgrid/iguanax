#ifndef __COL_LOG_H__
#define __COL_LOG_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLlog
//
// Author: Andrew Vajoczki
// Date:   Thursday, March 19th, 2009 @ 11:25:52 AM
//
// Description:
//
// COLlog class - heart of our tracing system.  Worth have a read through this.
//
//  Commandline parameters for controlling conditional logging:
//
//    --f log_filename            # +log_filename to append
//    --d debug_modules_pattern
//    --t trace_modules_pattern
//    --i info_modules_pattern
//    --w warning_modules_pattern
//    --e error_modules_pattern
//    --F true|false              # flush each trace line
//    --n true|false              # indenting; on by default.
//
//  Example:
//
//    program --t "ABCabc FOO* -FOObar*" --w "ABC* XYZ*" --f out.log
//
//  This will enable TRC INF WRN ERR conditional logging for module
//  'ABCabc' as well as modules beginning with 'FOO' - but not
//  modules beginning with 'FOObar'.
//  WRN and ERR conditional logging will be enabled for modules beginning
//  with 'ABC' or 'XYZ'. No DBG level logging will be enabled, as --d
//  was not specified.
//
//  A '-' before a pattern indicates exclusion. Pattern order is
//  significant - patterns are evaluated from left to right.
//
//  Tracing will be logged to the file "out.log".
//  If --f is not specified conditional logging will go to stdout.
//
//    --d applies to the levels: DBG TRC INF WRN ERR
//    --t applies to the levels:     TRC INF WRN ERR
//    --i applies to the levels:         INF WRN ERR
//    --w applies to the levels:             WRN ERR
//    --e applies to the levels:                 ERR
//
//  If no arguments are supplied conditional logging defaults to no logging.
//
//  Note: when used from the shell, the * character will expand to a random
//  filename on UNIX, so always remember to put quotes around each argument.
//
//---------------------------------------------------------------------------

#include "COLostream.h"
#include "COLstring.h"

#define COL_ENABLE_TRACING

#ifdef COL_ENABLE_TRACING // logging is on by default.

class COLlogStream : public COLostream
{
   COLstring m_String;
public:
   COLlogStream(char level, const char* module);
   ~COLlogStream() { }
   int length() const { return static_cast<int>(m_String.size()); }
   const char* buffer() const { return m_String.c_str(); }
private:
   COLlogStream(const COLlogStream&); // disallow
   COLlogStream& operator=(const COLlogStream&); // disallow
};

// --trace "* -COL" --level 4 --file "trace.log" --flush
// Defaults
//  level = 3
//  Standard output
//  No flushing

// API to use if you don't want or cannot set tracing via command line
// loads the tracing glob expression from env var CHM_TRACE by default
void COLtrace(const COLstring& Expression = "");
// loads the tracing file path from env var CHM_TRACE_FILE by default
void COLsetTraceLogFile(const COLstring& FilePath = "");
//void COLdebug(const char** Glob);

class COLlog{
public:
   static void init(int argc, const char** argv);
   static int init();
   static void fini();
   static void getArgs(COLstring& d, COLstring& t, COLstring& i, COLstring& w, COLstring& e, COLstring& F);
   static void reset(int argc, const char** argv);
   static void reset(const COLstring& d, const COLstring& t, const COLstring& i, const COLstring& w, const COLstring& e, const COLstring& F);
   static const char* getBaseModuleName(char* filename);
   static void reopenFile();
   COLlog(char level);
   virtual ~COLlog();
   void log(class COLostream& logstream);
   char getLevel() const { return m_Level; }
   void setFilter(const COLstring& ModulesPattern);
   int enabled(const char* module, int* logState) const;
   void dump(const char* module, const void* data, int nbytes) const;
private:
   COLlog();
   COLlog(const COLlog&);
   COLlog& operator=(const COLlog&);
private:
   char m_Level; // One of: T, I, W, E.
   // A whitespace-separated string of globs. Example: "ABC* XYZ* FOO*"
   COLstring m_Pattern; // guarded by gTraceLogMutex
};

class COLfunctionLogger{
public:
   COLfunctionLogger(const char* module, const char* fn, int line, bool doTrace);
   COLfunctionLogger(const void* Instance, const char* module, const char* fn, int line, bool doTrace);
   ~COLfunctionLogger();
   static const char* getIndentLevelString();
private:
   int _performTrace;
   const char* _module; // not owned by this instance - assumed to be static
   const char* _fn;     // not owned by this instance - assumed to be static
};

extern COLlog g_COLlog_DBG;
extern COLlog g_COLlog_TRC;
extern COLlog g_COLlog_INF;
extern COLlog g_COLlog_WRN;
extern COLlog g_COLlog_ERR;

// This must be put at the top of each .cpp to be logged (*).
// *NEVER* put this in a header file.
#define COL_LOG_MODULE \
   static char COLlogModule[] = __FILE__; \
   static const char* COL_MODULE = COLlog::getBaseModuleName(COLlogModule)

// (*) An alternative to COL_LOG_MODULE;
// Use either COL_LOG_MODULE *or* COL_LOG_MODULE_CUSTOM - not both.
// *NEVER* put this in a header file.
#define COL_LOG_MODULE_CUSTOM(MODULE_NAME) \
   static char COLlogModule[] = MODULE_NAME; \
   static const char* COL_MODULE = COLlog::getBaseModuleName(COLlogModule)

// The following macros are so cryptic because they have to evaluate
// the glob expression controlling each specific log line just *once*.
// Otherwise the logging would be over 100 times slower due to glob overhead.
// The macros also have to contend with performing logging only after
// the runtime conditional logging system is initialized in main(),
// and not log lines in static initialization code before main().

#define COL_TRACE_BASE(LOGGER,MODULE,MESSAGE) \
   do { \
      static int loGsTaTE_; \
      if (loGsTaTE_ > 0 || (!loGsTaTE_ && LOGGER.enabled(MODULE,&loGsTaTE_))) { \
         COLlogStream logStream_(LOGGER.getLevel(),MODULE); \
         LOGGER.log(logStream_ << COLfunctionLogger::getIndentLevelString() << MESSAGE); \
      } \
   } while (0)

#define COL_ENT(COL_BLOCK_ARG) \
   static int ColEntLogState;\
   COLfunctionLogger ColEntLoggeR(COL_MODULE,COL_BLOCK_ARG,__LINE__,\
      (ColEntLogState>0 || (!ColEntLogState && g_COLlog_TRC.enabled(COL_MODULE, &ColEntLogState))))

#define COL_FUNCTION(COL_FN_ARG) \
   static int ColFnLogState;\
   COLfunctionLogger ColEntFnLoggeR(COL_MODULE,#COL_FN_ARG,__LINE__,\
      (ColFnLogState>0 || (!ColFnLogState && g_COLlog_TRC.enabled(COL_MODULE, &ColFnLogState))))

#define COL_METHOD(COL_FN_ARG) \
   static int ColMethodLogState;\
   COLfunctionLogger ColMethodLoggeR((const void*)(this),COL_MODULE,#COL_FN_ARG,__LINE__,\
      (ColMethodLogState>0 || (!ColMethodLogState && g_COLlog_TRC.enabled(COL_MODULE, &ColMethodLogState))))

// should be first line in main(), or at least before any conditional logging macros.
#define COL_LOG_INIT(ARGC,ARGV) COLlog::init(ARGC,ARGV)
#define COL_LOG_RESET(ARGC, ARGV) COLlog::reset(ARGC, ARGV)
#define COL_LOG_FINI COLlog::fini()

#define COL_DBG(MESSAGE) COL_TRACE_BASE(g_COLlog_DBG,COL_MODULE,MESSAGE)
#define COL_TRC(MESSAGE) COL_TRACE_BASE(g_COLlog_TRC,COL_MODULE,MESSAGE)
#define COL_INF(MESSAGE) COL_TRACE_BASE(g_COLlog_INF,COL_MODULE,MESSAGE)
#define COL_WRN(MESSAGE) COL_TRACE_BASE(g_COLlog_WRN,COL_MODULE,MESSAGE)
#define COL_ERR(MESSAGE) COL_TRACE_BASE(g_COLlog_ERR,COL_MODULE,MESSAGE)

#define COL_VAR(V) COL_TRACE_BASE(g_COLlog_TRC,COL_MODULE,COL_T(#V" = ")<<V)
#define COL_VAR2(V1,V2) COL_TRACE_BASE(g_COLlog_TRC,COL_MODULE,COL_T(#V1" = ")<< V1<<COL_T(", "#V2" = ")<<V2)
#define COL_VAR3(V1,V2,V3) COL_TRACE_BASE(g_COLlog_TRC,COL_MODULE,COL_T(#V1" = ")<< V1<<COL_T(", "#V2" = ")<<V2<<COL_T(", "#V3" = ")<<V3)
#define COL_VAR4(V1,V2,V3,V4) COL_TRACE_BASE(g_COLlog_TRC,COL_MODULE,COL_T(#V1" = ")<< V1<<COL_T(", "#V2" = ")<<V2<<COL_T(", "#V3" = ")<<V3<<COL_T(", "#V4" = ")<<V4)

#define COL_DUMP_BASE(LOGGER,MODULE,MESSAGE,DATA,NBYTES) \
   do { \
      static int loGsTaTE_; \
      if (loGsTaTE_ > 0 || (!loGsTaTE_ && LOGGER.enabled(MODULE,&loGsTaTE_))) { \
         COLlogStream logStream_(LOGGER.getLevel(),MODULE); \
         LOGGER.log(logStream_ << COLfunctionLogger::getIndentLevelString() << MESSAGE << ": " << (NBYTES) << " bytes @ " << (void*)(DATA)); \
         LOGGER.dump(MODULE,DATA,NBYTES); \
      } \
   } while (0)

#define COL_DUMP(    MESSAGE,DATA,NBYTES) COL_DUMP_BASE(g_COLlog_DBG,COL_MODULE,MESSAGE,DATA,NBYTES)
#define COL_DBG_DUMP(MESSAGE,DATA,NBYTES) COL_DUMP_BASE(g_COLlog_DBG,COL_MODULE,MESSAGE,DATA,NBYTES)
#define COL_TRC_DUMP(MESSAGE,DATA,NBYTES) COL_DUMP_BASE(g_COLlog_TRC,COL_MODULE,MESSAGE,DATA,NBYTES)
#define COL_INF_DUMP(MESSAGE,DATA,NBYTES) COL_DUMP_BASE(g_COLlog_INF,COL_MODULE,MESSAGE,DATA,NBYTES)
#define COL_WRN_DUMP(MESSAGE,DATA,NBYTES) COL_DUMP_BASE(g_COLlog_WRN,COL_MODULE,MESSAGE,DATA,NBYTES)
#define COL_ERR_DUMP(MESSAGE,DATA,NBYTES) COL_DUMP_BASE(g_COLlog_ERR,COL_MODULE,MESSAGE,DATA,NBYTES)

// arguments are COLstring&
#define COL_LOG_GET_ARGS(d,t,i,w,e,F) COLlog::getArgs(d,t,i,w,e,F)

// arguments are const COLstring&
#define COL_LOG_RESET_ARGS(d,t,i,w,e,F) COLlog::reset(d,t,i,w,e,F)

#else // !COL_ENABLE_TRACING

#define COL_LOG_MODULE
#define COL_LOG_MODULE_CUSTOM(MODULE_NAME)
#define COL_DBG(MESSAGE)
#define COL_TRC(MESSAGE)
#define COL_INF(MESSAGE)
#define COL_WRN(MESSAGE)
#define COL_ERR(MESSAGE)
#define COL_VAR(V)
#define COL_VAR2(V1,V2)
#define COL_VAR3(V1,V2,V3)
#define COL_VAR4(V1,V2,V3,V4)
#define COL_ENT(BLOCKNAME)
#define COL_FUNCTION(FUNCTION)
#define COL_METHOD(FUNCTION)
#define COL_LOG_INIT(ARGC,ARGV)
#define COL_LOG_RESET(ARGC,ARGV)
#define COL_LOG_FINI
#define COL_LOG_GET_ARGS(d,t,i,w,e,F)
#define COL_LOG_RESET_ARGS(d,t,i,w,e,F)
#define COL_DUMP(    MESSAGE,DATA,NBYTES) do{}while(0)
#define COL_DBG_DUMP(MESSAGE,DATA,NBYTES) do{}while(0)
#define COL_TRC_DUMP(MESSAGE,DATA,NBYTES) do{}while(0)
#define COL_INF_DUMP(MESSAGE,DATA,NBYTES) do{}while(0)
#define COL_WRN_DUMP(MESSAGE,DATA,NBYTES) do{}while(0)
#define COL_ERR_DUMP(MESSAGE,DATA,NBYTES) do{}while(0)

#endif // COL_ENABLE_TRACING

#endif // end of defensive include
