//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLlog
//
// Description:
//
// Implementation
//
// Author: Andrew Vajoczki
// Date:   Thursday, March 19th, 2009 @ 11:25:52 AM
//
//---------------------------------------------------------------------------
#include "COLlog.h"

#ifdef COL_ENABLE_TRACING

#include "COLerror.h"
#include "COLutils.h"
#include "COLmutex.h"
#include "COLlocker.h"
#include "COLthreadLocal.h"
#include "COLarray.h"
#include "COLdateTime.h"
#include "COLsink.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <assert.h>

#ifdef _WIN32
#   include <windows.h>
#   include <process.h>
#else
#   include <unistd.h>
#   include <pthread.h>
#   include <sys/time.h>
#endif

#define COL_TRACE_INTERNAL(LOGGER,MODULE,MESSAGE) \
   do { COLlogStream logStream_(LOGGER.getLevel(),MODULE); LOGGER.log(logStream_ << MESSAGE); } while(0)

//
// These COLlog mutex accessor functions are needed due to the initialization
// of static global objects before main() that have COLlog statements.
// It cannot be guaranteed that COLlog.cpp's static mutexes will be initialized
// first, so unfortunately we have have this overhead in accessing mutexes.
//
static inline COLmutex& getTraceLogMutex()
{
   // mutex leaked by design to avoid crash after exit in static destructors
   static COLmutex* s_TraceLogMutex = new COLmutex;
   return *s_TraceLogMutex;
}
static inline COLmutex& getTraceTimeMutex()
{
   // mutex leaked by design to avoid crash after exit in static destructors
   static COLmutex* s_TraceTimeMutex = new COLmutex;
   return *s_TraceTimeMutex;
}

//
// This class was created to avoid a dependency on the FIL library.
//
class COLlogFile : public COLsink
{
public:
   COLlogFile()
   {
      m_FILE = 0;
   }
   ~COLlogFile()
   {
      close();
   }
   void open(const char* filename, const char* mode = "w")
   {
      COLPRECONDITION(m_FILE==0);
      m_FileName = filename;
      if ('+' == m_FileName[0])
      {
         // if filename has a '+' prefix, open in append mode
         ++filename; // skip the '+'
         mode = "a"; // append mode
      }
      m_FILE = fopen(filename, mode);
      if (NULL == m_FILE)
      {
         COLstring Error;
         COLostream ErrorStream(Error);
         ErrorStream << COL_T("Error: can not open log file '") << m_FileName << '\'';
         throw COLerror(Error);
      }
   }

   void close()
   {
      if (m_FILE)
      {
         fclose(m_FILE);
         m_FILE = 0;
      }
   }
   bool isOpen() const
   {
      return m_FILE != 0;
   }
   // COLsink virtual methods
   virtual COLuint32 write(const void* buffer, COLuint32 bufferSize)
   {
      COLPRECONDITION(m_FILE);
      int bytesWritten = fwrite(buffer, 1, bufferSize, m_FILE);
      COLPRECONDITION(bytesWritten == bufferSize);
      return bytesWritten;
   }
   virtual void flush()
   {
      if (m_FILE)
      {
         fflush(m_FILE);
      }
   }
   void reopen(){
      m_FILE = fopen(m_FileName.c_str(), "a");
      if (NULL == m_FILE)
      {
         COLstring Error;
         COLostream ErrorStream(Error);
         ErrorStream << COL_T("Error: can not open log file '") << m_FileName << '\'';
         throw COLerror(Error);
      }
   }
private:
   COLstring m_FileName;
   FILE* m_FILE;
};

static COLostream*        gStream = &COLcout;

static int                gWantIndenting = 1;  // indenting is on by default
static int                gStreamWasSet;
static int                gStreamWantFlush = 0;

// must leak thread local indent level due to static dtors that may trace
static COLthreadLocalLong gTraceThreadLocalIndent(false);

static time_t             gLastSecondsSinceEpoch = -1; // guarded by getTraceTimeMutex()
static char               gLastTimeBuffer[40];         // guarded by getTraceTimeMutex()

static COLarray<int*>    gLineEnableVector;   // guarded by getTraceLogMutex()
static COLstring          gDbgArg;             // guarded by getTraceLogMutex()
static COLstring          gTrcArg;             // guarded by getTraceLogMutex()
static COLstring          gInfArg;             // guarded by getTraceLogMutex()
static COLstring          gWrnArg;             // guarded by getTraceLogMutex()
static COLstring          gErrArg;             // guarded by getTraceLogMutex()
static COLstring          gFlushArg;           // guarded by getTraceLogMutex()

// global tracing objects
COLlog g_COLlog_DBG('D');
COLlog g_COLlog_TRC('T');
COLlog g_COLlog_INF('I');
COLlog g_COLlog_WRN('W');
COLlog g_COLlog_ERR('E');

inline unsigned long COL_getCurrentThreadID()
{
#ifdef _WIN32
   return (unsigned long)GetCurrentThreadId();
#else
   return (unsigned long)pthread_self();
#endif
}

COLlogStream::COLlogStream(char level, const char* module)
: COLostream(m_String)
{
   m_String.setCapacity(240);
   char LinePrefix[128];
   COLtimeStamp currentTime;
   COLcurrentTimeStamp(&currentTime);
   time_t now = (time_t)currentTime.tv_sec;
   {
      // Optimization: cache time string since it is the same 99% of the time.
      // Calling localtime and formatting the string is a very expensive operation.
      COLlocker Lock(getTraceTimeMutex());

      if (now != gLastSecondsSinceEpoch)
      {
         // optimization to not recompute time string if time is the same
         struct tm tm;
#ifdef _WIN32
         tm = *::localtime(&now); // threadsafe on Windows: uses thread local storage
#else
         localtime_r(&now, &tm);
#endif
         ::snprintf(gLastTimeBuffer, 40, "%02d:%02d:%02d.", tm.tm_hour, tm.tm_min, tm.tm_sec);
         gLastSecondsSinceEpoch = now;
      }
      ::memcpy(LinePrefix+2, gLastTimeBuffer, 9);
   }
   LinePrefix[0] = level;
   LinePrefix[1] = ' ';

   const int millisStart = 11; // position immediately after "T HH:MM:SS."
   const int nowMillis = int(currentTime.tv_usec / 1000);

   // 97% of interfaceware's .cpp file names (w/o suffix) are 28 characters and under.
   // The 2 longest file names are:
   //   45 DBDRPCinterfaceOnMessageRequestCompletedEvent
   //   46 com_interfaceware_chameleon_ChameleonException
   // Only 20 cpp file names are not unique up to 28 characters.
   // The average .cpp file name (w/o suffix) is 15 characters.
   //
   int PrefixLength = millisStart + ::snprintf(LinePrefix+millisStart, 128-millisStart,
       "%03d %012lx %-28s ", nowMillis, COL_getCurrentThreadID(), module);

   assert(PrefixLength<sizeof(LinePrefix)); // do not use COLASSERT or COLPRECONDITION

   // output the tracing prefix "L HH:MM:SS.mmm TTTTTTTT ModuleNameNNNNNNNNNNNNNNNNNN "
    this->write(LinePrefix, PrefixLength);
}

#undef B
#define B "| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | "
static char s_indent_string[] = B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B B;
#undef B

const long s_indent_levels = sizeof(s_indent_string) / 2;

COLfunctionLogger::COLfunctionLogger(const char* module, const char* fn, int line, bool doTrace)
: _performTrace(doTrace)
{
   if (_performTrace)
   {
      _module = module;
      _fn = fn;

      if (gWantIndenting)
      {
         // get the thread specific indent level
         long indentLevel = gTraceThreadLocalIndent.get();

         // abort on indent level overflow
         assert(indentLevel < s_indent_levels - 2);

         // output the function with appropriate indent level
         const char* indent = s_indent_string + (s_indent_levels - indentLevel) * 2;
         COL_TRACE_INTERNAL(g_COLlog_TRC, module, indent << '>' << fn << COL_T(" L:") << line);

         // increase the indent level
         gTraceThreadLocalIndent.set(++indentLevel);
      }
      else
      {
         COL_TRACE_INTERNAL(g_COLlog_TRC, module, '>' << fn << COL_T(" L:") << line);
      }
   }
}

COLfunctionLogger::COLfunctionLogger(const void* Instance, const char* module, const char* fn, int line, bool doTrace)
: _performTrace(doTrace)
{
   if (_performTrace)
   {
      _module = module;
      _fn = fn;

      if (gWantIndenting)
      {
         // get the thread specific indent level
         long indentLevel = gTraceThreadLocalIndent.get();

         // abort on indent level overflow
         assert(indentLevel < s_indent_levels - 2);

         // output the function with appropriate indent level
         const char* indent = s_indent_string + (s_indent_levels - indentLevel) * 2;
         COL_TRACE_INTERNAL(g_COLlog_TRC, module, indent << '>' << fn << COL_T(" this=") << Instance << COL_T(" L:") << line);

         // increase the indent level
         gTraceThreadLocalIndent.set(++indentLevel);
      }
      else
      {
         COL_TRACE_INTERNAL(g_COLlog_TRC, module, '>' << fn << COL_T(" this=") << Instance << COL_T(" L:") << line);
      }
   }
}

const char* COLfunctionLogger::getIndentLevelString()
{
   if (gWantIndenting)
   {
      return s_indent_string + (s_indent_levels - gTraceThreadLocalIndent.get()) * 2;
   }
   return "";
}

COLfunctionLogger::~COLfunctionLogger()
{
   if (_performTrace)
   {
      if (gWantIndenting)
      {
         // get the thread specific indent level and decrement it
         long indentLevel = gTraceThreadLocalIndent.get() - 1;
         if (indentLevel < 0)
         {
            indentLevel = 0;
         }
         gTraceThreadLocalIndent.set(indentLevel);

         // output the function with appropriate indent level
         const char* indent = s_indent_string + (s_indent_levels - indentLevel) * 2;
         COL_TRACE_INTERNAL(g_COLlog_TRC, _module, indent << '<' << _fn);
      }
      else
      {
         COL_TRACE_INTERNAL(g_COLlog_TRC, _module, '<' << _fn);
      }
   }
}

COLlog::COLlog(char level)
{
   m_Level = level;
}

COLlog::~COLlog()
{
}

// called before main() when only a single thread running.
// do not make static.
int COL_gWasInitialized = COLlog::init();

int COLlog::init()
{
   return 1;
}

#ifdef _WIN32
#  define COL_LOG_GET_PID() _getpid()
#else
#  define COL_LOG_GET_PID() getpid()
#endif

static COLstring COLlogGetCurrentDir()
{
   char Buffer[1024];
#ifdef _WIN32
   GetCurrentDirectory(sizeof(Buffer), Buffer);
#else
   getcwd(Buffer, sizeof(Buffer));
#endif
   return Buffer;
}

static const char COLlogInternal[] = "iNTERFACEWARE";

static int gAfterMain; // only log if we are after main()

static void COLlogHeader(int argc, const char** argv)
{
   COL_TRACE_INTERNAL(g_COLlog_INF, "", "\n  Timestamp    Thread ID    File  ");

   static const char Line[] = "----------------------------------------------------------------------------";
   COL_TRACE_INTERNAL(g_COLlog_INF, COLlogInternal, Line);

   // date
   time_t Now = ::time(0);
   COLstring DateString = ::ctime(&Now);
   DateString.stripAll('\n');
   DateString.stripAll('\r');
   COL_TRACE_INTERNAL(g_COLlog_INF, COLlogInternal, DateString);

   // PID
   COL_TRACE_INTERNAL(g_COLlog_INF, COLlogInternal, COL_T("PID: ") << COL_LOG_GET_PID());

   // USER
   COL_TRACE_INTERNAL(g_COLlog_INF, COLlogInternal, COL_T("USER: ") << ::getenv("USER"));

   // current directory
   COL_TRACE_INTERNAL(g_COLlog_INF, COLlogInternal, COL_T("directory: ") << COLlogGetCurrentDir());

   // commandline args
   for (int ArgIndex = 0; ArgIndex < argc; ++ArgIndex)
   {
      COL_TRACE_INTERNAL(g_COLlog_INF, COLlogInternal, COL_T("argv[") <<ArgIndex<< COL_T("]: ") << argv[ArgIndex]);
   }

   COL_TRACE_INTERNAL(g_COLlog_INF, COLlogInternal, Line);
}

void COLlog::init(int argc, const char** argv)
{
   if (gAfterMain) return;

   // all tracing is initially disabled
   COLstring dbg;
   COLstring trc;
   COLstring inf;
   COLstring wrn;
   COLstring err;

   for (int a = 1; a < argc; a++)
   {
      COLstring newFlags = argv[a];

      if (newFlags == "--trace") {
         newFlags = "--t";
      }
      if (newFlags == "--out") {
         newFlags = "--f";
      }

      const COLstring s = newFlags;

      COLstring Argument;
      if (a + 1 < argc)
      {
         Argument = argv[a+1];
      }

      if (s == "--d")
      {
         dbg += ' ' + Argument;
         trc += ' ' + Argument;
         inf += ' ' + Argument;
         wrn += ' ' + Argument;
         err += ' ' + Argument;
         COLlocker Lock(getTraceLogMutex());
         gDbgArg = Argument;
      }
      if (s == "--t")
      {
         trc += ' ' + Argument;
         inf += ' ' + Argument;
         wrn += ' ' + Argument;
         err += ' ' + Argument;
         COLlocker Lock(getTraceLogMutex());
         gTrcArg = Argument;
       }
      else if (s == "--i")
      {
         inf += ' ' + Argument;
         wrn += ' ' + Argument;
         err += ' ' + Argument;
         COLlocker Lock(getTraceLogMutex());
         gInfArg = Argument;
      }
      else if (s == "--w")
      {
         wrn += ' ' + Argument;
         err += ' ' + Argument;
         COLlocker Lock(getTraceLogMutex());
         gWrnArg = Argument;
      }
      else if (s == "--e")
      {
         err += ' ' + Argument;
         COLlocker Lock(getTraceLogMutex());
         gErrArg = Argument;
      }
      else if (s == "--F")
      {
         const char ch = Argument.size() ? Argument[0] : '1';
         switch (ch)
         {
         case 'f':
         case 'F':
         case 'n':
         case 'N':
         case '0':
            gStreamWantFlush = 0;
            break;
         default:
            gStreamWantFlush = 1;
            break;
         }
         COLlocker Lock(getTraceLogMutex());
         gFlushArg = Argument;
      }
      else if (s == "--n")
      {
         const char ch = Argument.size() ? Argument[0] : '1';
         switch (ch)
         {
         case 'f':
         case 'F':
         case 'n':
         case 'N':
         case '0':
            gWantIndenting = 0;
            break;
         default:
            gWantIndenting = 1;
            break;
         }
      }
      else if (s == "--f")
      {
         if (gStreamWasSet)
         {
            COLcerr << COL_T("Error: log output file cannot be specified more than once.\n");
            ::exit(1);
         }

         if (Argument == "stdout")
         {
            gStream = &COLcout;
            gStreamWasSet = 1;
         }
         else if (Argument == "stderr")
         {
            gStream = &COLcerr;
            gStreamWasSet = 1;
         }
         else if (Argument != "" && Argument.substr(0,1) != "-"){
            COLlogFile* LogFile = new COLlogFile(); // owned by gStream
            try{
               LogFile->open(Argument.c_str());
            } catch (COLerror& Error)    {
               COLcerr << Error << newline;
               ::exit(1);
            } if (!LogFile->isOpen()) {
               COLcerr << COL_T("Error: cannot open file '") << Argument << COL_T("'\n");
               ::exit(1);
            }
            //XXX TODO: should lock log file to prevent reuse

            // NOTE: *must* leak because may be used after main().
            gStream = new COLostream(LogFile, true);

            gStreamWasSet = 1;
         }
      }
   }

   {
      COLlocker Lock(getTraceLogMutex());

      // Enable errors and warnings automatically if any of --d, --t or --i is set.
      // To avoid this, just set --e ' ' which is a no-op.
      if ((dbg.size() || trc.size() || inf.size()) && (gWrnArg.is_null() && gErrArg.is_null())) {
         wrn = "*";
         err = "*";
      }

      g_COLlog_DBG.setFilter(dbg);
      g_COLlog_TRC.setFilter(trc);
      g_COLlog_INF.setFilter(inf);
      g_COLlog_WRN.setFilter(wrn);
      g_COLlog_ERR.setFilter(err);
      gAfterMain = 1; // must be last line
   }

   // if --f was specified, or if tracing is on at all - output the log header
   if (gStreamWasSet || g_COLlog_ERR.m_Pattern.size())
   {
      COLlogHeader(argc, argv);
      gStream->flush();
   }
}

void COLtrace(const COLstring& Expression){
   const char* pGlob = NULL;
   if (!Expression.is_null()) {
      pGlob = Expression.c_str();
   } else {
      pGlob = getenv("CHM_TRACE");
      if (!pGlob) { return; }
   }
   COLcout << "Tracing on CHM_TRACE=" << pGlob << newline;
   COLlocker Lock(getTraceLogMutex());
   g_COLlog_TRC.setFilter(pGlob);
   g_COLlog_INF.setFilter("");
   g_COLlog_DBG.setFilter("");
   g_COLlog_WRN.setFilter("*");
   g_COLlog_ERR.setFilter("*");
   gAfterMain = 1;
}

void COLsetTraceLogFile(const COLstring& FilePath){
   const char* pFileName = NULL;
   COLlogFile* LogFile = new COLlogFile(); // owned by gStream
   try{
      LogFile->open(FilePath.c_str());
   } catch (COLerror& Error) {
      COLcerr << Error << newline;
      ::exit(1);
   } if (!LogFile->isOpen()) {
      COLcout << COL_T("Error: cannot open file '") << pFileName << newline;
      ::exit(1);
   }
   //XXX TODO: should lock log file to prevent reuse

   // NOTE: *must* leak because may be used after main().
   COLlocker Lock(getTraceLogMutex());
   gStream = new COLostream(LogFile, true);
   gStreamWasSet = 1;
   gStreamWantFlush = 1; // Default flushing on .
}

void COLlog::getArgs(COLstring& d, COLstring& t, COLstring& i, COLstring& w, COLstring& e, COLstring& F)
{
   COLlocker Lock(getTraceLogMutex());
#if 1
   d = gDbgArg;
   t = gTrcArg;
   i = gInfArg;
   w = gWrnArg;
   e = gErrArg;
#else
   d = g_COLlog_DBG.m_Pattern;
   t = g_COLlog_TRC.m_Pattern;
   i = g_COLlog_INF.m_Pattern;
   w = g_COLlog_WRN.m_Pattern;
   e = g_COLlog_ERR.m_Pattern;
#endif
   F = gFlushArg;
}

static void COLlog_Write(const COLstring& Data)
{
   COLlocker Lock(getTraceLogMutex());
   gStream->write(Data.c_str(), Data.size());
}

void COLlog::reset(int argc, const char** argv){
   COL_TRACE_INTERNAL(g_COLlog_INF, COLlogInternal, "==reset=====================================================================");
   for (int loop = 0; loop < 2; ++loop)
   {
      gStreamWasSet = 0;
      gAfterMain = 0;
      COLsleep(50); // lame attempt to wait for stray tracing on other threads

      {
         COLlocker Lock(getTraceLogMutex());

         g_COLlog_DBG.setFilter("");
         g_COLlog_TRC.setFilter("");
         g_COLlog_INF.setFilter("");
         g_COLlog_WRN.setFilter("");
         g_COLlog_ERR.setFilter("");

         // reset all the per-line tracing information
         COLarray<int*>::iterator Iter = gLineEnableVector.begin();
         for (; Iter != gLineEnableVector.end(); ++Iter) { *(*Iter) = 0; }
      }

      init(argc, argv);
   }
}

void COLlog::reset(const COLstring& d, const COLstring& t, const COLstring& i, const COLstring& w, const COLstring& e, const COLstring& F)
{
   //if (gWantIndenting) return; // can't reset tracing if you have indenting active

   // Note: we're not going to allow resetting the --f logfilename argument

   COL_TRACE_INTERNAL(g_COLlog_INF, COLlogInternal, "==reset=====================================================================");

   // This is a race condition, so we loop twice to try to reset
   // all the threads' static tracing int's correctly.
   for (int loop = 0; loop < 2; ++loop)
   {
      gAfterMain = 0;
      COLsleep(50); // lame attempt to wait for stray tracing on other threads

      const char* argv[20];
      int argc = 0;
      argv[argc++] = "<program>";
      argv[argc++] = "--d";
      argv[argc++] = d.c_str();
      argv[argc++] = "--t";
      argv[argc++] = t.c_str();
      argv[argc++] = "--i";
      argv[argc++] = i.c_str();
      argv[argc++] = "--w";
      argv[argc++] = w.c_str();
      argv[argc++] = "--e";
      argv[argc++] = e.c_str();
      argv[argc++] = "--F";
      argv[argc++] = F.c_str();

      {
         COLlocker Lock(getTraceLogMutex());

         g_COLlog_DBG.setFilter("");
         g_COLlog_TRC.setFilter("");
         g_COLlog_INF.setFilter("");
         g_COLlog_WRN.setFilter("");
         g_COLlog_ERR.setFilter("");

         // reset all the per-line tracing information
         COLarray<int*>::iterator Iter = gLineEnableVector.begin();
         for (; Iter != gLineEnableVector.end(); ++Iter) { *(*Iter) = 0; }
      }

      init(argc, argv);
   }
}

void COLlog::setFilter(const COLstring& Filter)
{
   COLlocker Lock(getTraceLogMutex());
   m_Pattern = Filter;
}

//
// Execute the pattern match at most once per line traced.
//
// Returns:
//  0 for pattern match not executed.
//  1 for pattern match executed, and matched.
// -1 for pattern match executed, but failed to match.
//
int COLlog::enabled(const char* module, int* logstate) const
{
   COLstring Pattern;
   {
      COLlocker Lock(getTraceLogMutex());
      gLineEnableVector.push_back(logstate);
      Pattern = m_Pattern;
   }   
#ifdef IFW_RELEASE  
   if (COLstring(module).find("DRX") != npos) { // IX-2935 Hide tracing for license library
      *logstate = 0;
      return *logstate;
   }
#endif
   if (Pattern.size())
   {
      int State = COLglobMatch(Pattern.c_str(), module) ? 1 : -1;
      if (gAfterMain)
      {
         *logstate = State;
         return *logstate > 0;
      }
      return State;
   }
   // no pattern is defined - do not log
   if (gAfterMain)
   {
      *logstate = -1;
   }
   return 0;
}

void COLlog::fini()
{
   gStream->flush();
   //XXX Should logging be terminated or should the log file object
   //XXX be allowed to leak in the event that global destructors
   //XXX have tracing?
   if (gStream != &COLcout && gStream != &COLcerr)
   {
      COLostream* oldStream = gStream;
      gStream = &COLcout;
      delete oldStream;
   }
}

void COLlog::log(COLostream& Stream)
{
   Stream << '\n';
   COLlogStream* LogStream = dynamic_cast<COLlogStream*>(&Stream);
   COLASSERT(LogStream);
   COLlocker Lock(getTraceLogMutex());
   gStream->write(LogStream->buffer(), LogStream->length());
   if (gStreamWantFlush)
   {
      gStream->flush();
   }
}

// Given a filename like ../../foo/MyModule.cpp return "MyModule".
// Note: this call assumes that a a writable char array passed
// as a parameter.  This array will be changed.
const char* COLlog::getBaseModuleName(char* FileName)
{
   assert(FileName && FileName[0]);
   char* module = FileName;
   int len = ::strlen(FileName);
   for (int i = len - 1; i >= 0; --i)
   {
      char ch = FileName[i];
      if (ch == '/' || ch == '\\')
      {
         module = FileName + i + 1;
         break;
      }
   }
   for (int m = 0; module[m]; m++)
   {
      if (module[m] == '.')
      {
         module[m] = 0;
         break;
      }
   }
   return module;
}

void COLlog::reopenFile()
{
   COLlogFile* pLogFile = dynamic_cast<COLlogFile*>(gStream->sink());
   if (pLogFile){
      pLogFile->reopen();
   }
}

static const int s_Max_COL_DUMP_WIDTH = 120;

static int get_COL_DUMP_WIDTH()
{
   const char* COL_DUMP_WIDTH_string = ::getenv("COL_DUMP_WIDTH");
   if (COL_DUMP_WIDTH_string == NULL) {
      return 16;
   }
   const int COL_DUMP_WIDTH = ::atoi(COL_DUMP_WIDTH_string);
   if (COL_DUMP_WIDTH <= 0) {
      return 10;
   }
   if (COL_DUMP_WIDTH > s_Max_COL_DUMP_WIDTH) {
      return s_Max_COL_DUMP_WIDTH;
   }
   return COL_DUMP_WIDTH;
}

static int get_COL_DUMP_MODE()
{
   const char* COL_DUMP_MODE_string = ::getenv("COL_DUMP_MODE");
   if (COL_DUMP_MODE_string == NULL) {
      return 0;
   }
   const int COL_DUMP_MODE = ::atoi(COL_DUMP_MODE_string);
   return COL_DUMP_MODE;
}

void COLlog::dump(const char* module, const void* pData, int nbytes) const
{
   //const char COL_MODULE[] = "COLlog";
   //COL_FUNCTION(COLlog::dump);
   static const char Hex[] = "0123456789abcdef?";
   static int s_COL_DUMP_WIDTH = ::get_COL_DUMP_WIDTH();
   static int s_COL_DUMP_MODE  = ::get_COL_DUMP_MODE();
   if (nbytes <= 0 || pData == NULL) { return; }
   const unsigned char* data = (const unsigned char*)pData;
   char HexBuf[3 * s_Max_COL_DUMP_WIDTH + 16];
   memset(HexBuf, ' ', 3*(s_COL_DUMP_WIDTH+1));
   char CharBuf[s_Max_COL_DUMP_WIDTH + 16];
   CharBuf[0] = 0;
   int c = 0;
   for (int i = 0; i < nbytes; ++i) {
      COLASSERT(c < s_COL_DUMP_WIDTH);
      const unsigned char ch = data[i];
      int HexBufPos = c * 3;
      const int Printable = isprint(ch);
      if (Printable && s_COL_DUMP_MODE) {
         HexBuf[HexBufPos+1] = (char)ch;
      } else {
         HexBuf[HexBufPos+0] = Hex[(ch & 0xf0) >> 4];
         HexBuf[HexBufPos+1] = Hex[(ch & 0x0f)];
         CharBuf[c] = Printable ? (char)ch : '.';
      }
      if (++c >= s_COL_DUMP_WIDTH || i+1 >= nbytes) {
         CharBuf[c] = 0;
         HexBuf[3*s_COL_DUMP_WIDTH+1] = 0; // subscript is as intended
         char RowPrefix[16];
         snprintf(RowPrefix, 16, "%6d: ", i/s_COL_DUMP_WIDTH*s_COL_DUMP_WIDTH);
         COL_TRACE_INTERNAL((const_cast<COLlog&>(*this)), module,
            COLfunctionLogger::getIndentLevelString() << RowPrefix << HexBuf
            << (s_COL_DUMP_MODE ? "" : CharBuf));
         c = 0;
         memset(HexBuf, ' ', 3*s_COL_DUMP_WIDTH);
      }
   }
}

#endif // COL_ENABLE_TRACING
