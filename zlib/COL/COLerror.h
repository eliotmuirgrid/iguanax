#ifndef __COL_ERROR_H__
#define __COL_ERROR_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLerror
//
// Description:
//
// Error class used in project
//
// Author: Eliot Muir
// Date:   Wed 09/09/1998
//---------------------------------------------------------------------------

#include "COLrefCounted.h"
#include "COLostream.h"
#include "COLsinkString.h"

#include <stdlib.h>

class COLvar;

bool COLdebugInit(COLstring* pError);
bool COLdebugShutdown(COLstring* pError);

// Artificially trigger an abort if env var COL_ERR_ABORT is set to the current function name (__func__)
#define COL_ABORT_HERE_IF_ENV_VAR_IS_SET() \
   { \
      COLstring Crash = getenv("COL_ERR_ABORT"); \
      if (__func__ == Crash) { \
         COLcout << "Artificially abort Iguana in " << __func__ << " as specified by environment variable COL_ERR_ABORT." << newline; \
         COLASSERT(false); \
      } \
   }
// Artificially trigger a segfault if env var COL_ERR_CRASH is set to the current function name (__func__)
#define COL_CRASH_HERE_IF_ENV_VAR_IS_SET() \
   { \
      COLstring Crash = getenv("COL_ERR_CRASH"); \
      if (__func__ == Crash) { \
         COLcout << "Artificially crash Iguana in " << __func__ << " as specified by environment variable COL_ERR_CRASH." << newline; \
         int* pInt = NULL; \
         *pInt = 42; \
      } \
   }
#define COL_CRASH_HERE_IF_ENV_VAR_IS_SET_ON_CALL(N) \
   { \
      COLstring Crash = getenv("COL_ERR_CRASH"); \
      if (__func__ == Crash) { \
         static int i = 1; \
         if (i++ == N) { \
            COLcout << "Artificially crash Iguana in " << __func__ << " as specified by environment variable COL_ERR_CRASH." << newline; \
            int* pInt = NULL; \
            *pInt = 42; \
         } \
      } \
   }
COLstring COLbacktrace();
void COLbacktrace2(COLostreamString& bt);

void COLsetDebugSearchPath(const COLstring& Path);
COLstring COLdebugSearchPath();

#ifdef _WIN32
// Don't call this function directly - use COL_THROW_WIN32_ERROR.
void COLthrowWin32Error(
   long ErrorCode, // e.g. COLlastError() or ::GetLastError()
   const char* Description=NULL,
   int   LineNumber=-1,
   const char* FileName=NULL
);
#endif // _WIN32

long COLlastError();  //GetLastError on WIN32 and errno on POSIX

// The next macro is very handy.  It will build a COLerror
// exception and throw it.  Any time that you see a _WIN32 API
// function that tells you to use GetLastError to retrieve the
// error code, simply running this macro is an easy way to
// obtain the error description using FormatMessage and GetLastError
// i.e.
// if ( _WIN32 API Call Returns Failure Code)
// {
//    COL_THROW_WIN32_ERROR("During ServiceCreate call for " << ServiceName);
// }
// The above should throw a COLerror object that says something along the lines
// of
// During ServiceCreate call for XYZ on line 23 of SDMdaemon.cpp windows reported that
// blah blah blah happened.
//
// Note!  It is important that GetLastError() is called immediately after
// the failing Win32 call, as LastError can be easily overwritten by
// seemingly innocuous C++ code.  Therefore, always call COL_THROW_WIN32_ERROR
// immediately after the failing Win32 function.  See #13923.
#define COL_THROW_WIN32_ERROR(Message)\
{\
   long ErrorCode = COLlastError();\
   COLsinkString _ErrorSink;\
   COLostream ColErrorStream(_ErrorSink);\
   ColErrorStream << Message;\
   COLthrowWin32Error(ErrorCode, _ErrorSink.string().c_str());\
}

#define COL_THROW_WIN32_ERROR_PLAIN(Message){\
   long ErrorCode = COLlastError();\
   COLsinkString _ErrorSink;\
   COLostream ColErrorStream(_ErrorSink);\
   ColErrorStream << Message;\
   COLthrowWin32Error(ErrorCode, _ErrorSink.string().c_str());\
}

// This call goes a little further.
// COL_CHECK_WIN32_CALL(::CreateWindowEx(...));
#define COL_CHECK_WIN32_CALL(CALL) if (0 == CALL) { COL_THROW_WIN32_ERROR(#CALL << "  Failed") }

// Calls strerror under the hood for POSIX and the equivalent routine for
COLstring COLerrorString(long ErrorCode);

COLstring COLlastErrorAsString();

class COLerror : public COLrefCounted{
public:
   enum COLerrorCode {
     PreCondition        = 0x80000100,
     PostCondition       = 0x80000101,
     FileError           = 0x80000201,
     SerializationError  = 0x80001000,
     SystemError         = 0x80000500,
     InvalidDateTime     = 0x80002000,
     ParsingError        = 0x80003000,
     Cancel              = 0x80004000,
   };

   COLerror(const COLstring& Message, int Line, const char* File, int Code);
   COLerror(const COLstring& Message="", int Code=0);
   COLerror(const COLerror& Error, int Code);
   COLerror(const COLerror& Error);

   virtual ~COLerror();

   const COLstring& description() const { return m_Description; }
   int code() const { return m_Code; }
   const COLstring& file() const { return m_File; }
   int line() const { return m_Line; }

   void setDescription(const COLstring& Description);
   void setCode(int NewCode);

   virtual COLerror& operator=(const COLerror& Error);
   virtual bool operator==(const COLerror& Rhs) const;

protected:
   COLstring m_Description;
   int m_Code;
   COLstring m_File;
   int m_Line;
};

COLostream& operator<<(COLostream& Stream, const COLerror& Error);

// This is good if you have an ANSI C function that sets the errno variable.
// this macro will generate a COLerror object with a nice error message using COLerrorString
// i.e.
// do some ANSI C call which sets errno
// COL_THROW_STRERROR("socket call failed");
#define COL_THROW_STRERROR(Message) {\
   int ErrorCode = errno;\
   COLsinkString _ErrorSink;\
   COLostream Stream(_ErrorSink);\
   Stream << Message << ' ' << COLerrorString(ErrorCode) << '.'; \
   throw COLerror(_ErrorSink.string(), __LINE__, __FILE__, ErrorCode);\
}

#define COL_THROW_STRERROR_PLAIN(Message) {\
   int ErrorCode = errno;\
   COLsinkString _ErrorSink;\
   COLostream Stream(_ErrorSink);\
   Stream << Message << ' ' << COLerrorString(ErrorCode) << '.';\
   throw COLerror(_ErrorSink.string(), ErrorCode);\
}

#define COL_ERROR_STREAM(_Message, Number)\
{\
   COLsinkString _ErrorSink;\
   COLostream Stream(_ErrorSink);\
   Stream << _Message;\
   throw COLerror(_ErrorSink.string(), __LINE__, __FILE__, (COLerror::COLerrorCode)Number);\
}

// Same as COL_ERROR_STREAM except without the File and Line information.  Use this in
// cases when the __FILE__ and __LINE__ macros do not actually provide good context information
// i.e. when the code is within a commonly used library and the actual problem is not in the library
// but results from application code using the library.
#define COL_ERROR_STREAM_PLAIN(Message, Number){\
   COLsinkString _ErrorSink;\
   COLostream ColErrorStream(_ErrorSink);\
   ColErrorStream << Message;\
   throw COLerror(_ErrorSink.string(), (COLerror::COLerrorCode)Number);\
}

// Aliases for the COL_ERROR_STREAM* macros that don't require you to explicitly
// set an error code.
#define COL_ERR_STREAM(Message) COL_ERROR_STREAM(Message, COLerror::PreCondition)
#define COL_ERR_STREAM_PLAIN(Message) COL_ERROR_STREAM_PLAIN(Message, COLerror::PreCondition)

// #13609 Wrapper around abort() to improve DBG crash dumps on Windows.
// For some reason, crash dumps do not include the stack frame calling abort()
// unless the CRT PDBs are available.  By using COLabort, the desired stack frame
// is captured, and the irrelevant COLabort() frame is lost instead.
// extern "C" for use from python.
extern "C" void COLabort();

void COLabortWithMessage(const COLstring& ErrorString);

typedef void (*COLabortCallback)(const COLstring& ErrorString);

void COLsetAbortCallback(COLabortCallback pAbortCallback);

#undef COLASSERT
#define COL_ASSERTS_ON
#ifdef COL_ASSERTS_ON
#define COLASSERT(Condition) \
   if (!(Condition)){\
      COLsinkString _ErrorSink;\
      COLostream ErrorStringStream_(_ErrorSink);\
      ErrorStringStream_ << __FILE__ << ':' << __LINE__ << " Assertion failed: " << #Condition;\
      COLabortWithMessage(_ErrorSink.string());\
   }
#else
#define COLASSERT(Condition)
#endif

#define COL_ABORT(Message)\
      COLsinkString _ErrorSink;\
      COLostream ErrorStringStream_(_ErrorSink);\
      ErrorStringStream_ << Message << " " << __FILE__ << ':' << __LINE__;\
      COLabortWithMessage(_ErrorSink.string());


// Implementation macros for COLPRECONDITION and COLPOSTCONDITION.

#undef COL_THROW_IMPL
#define COL_THROW_IMPL(Condition, Message, AssertType) \
   if (!(Condition)){\
      COLsinkString _ErrorSink;\
      COLostream ErrorStringStream_(_ErrorSink);\
      ErrorStringStream_ << Message << " " << __FILE__ << ':' << __LINE__ << " Assertion failed: " << #Condition;\
      COLabortWithMessage(_ErrorSink.string());\
   }

#undef COL_THROW_PRECONDITION
#define COL_THROW_PRECONDITION(Condition, ConditionStr) \
   COL_THROW_IMPL(Condition, "Failed precondition: " << ConditionStr, COLerror::PreCondition);

#undef COL_THROW_MSG_PRECONDITION
#define COL_THROW_MSG_PRECONDITION(Condition, ConditionStr, Message) \
   COL_THROW_IMPL(Condition, "Failed precondition: " << ConditionStr << ", " << Message, COLerror::PreCondition);

#undef COL_THROW_POSTCONDITION
#define COL_THROW_POSTCONDITION(Condition, ConditionStr) \
   COL_THROW_IMPL(Condition, "Failed postcondition:" << ConditionStr, COLerror::PostCondition);

#undef COL_THROW_MSG_POSTCONDITION
#define COL_THROW_MSG_POSTCONDITION(Condition, ConditionStr, Message) \
   COL_THROW_IMPL(Condition, "Failed postcondition: " << ConditionStr << ", " << Message, COLerror::PostCondition);

// In theory the next two macros are for compulsory preconditions.
// In practice I don't think that they have been used much throughout
// the code.
#define COLCOMPPRECONDITION(Condition) COL_THROW_PRECONDITION(Condition, #Condition)
#define COLCOMPPOSTCONDITION(Condition) COL_THROW_POSTCONDITION(Condition, #Condition)

#ifdef COL_ASSERTS_ON
   #define COLPRECONDITION(Condition) COL_THROW_PRECONDITION(Condition, #Condition)
   #define COLMSGPRECONDITION(Condition, Message) COL_THROW_MSG_PRECONDITION(Condition, #Condition, Message)
   #define COLPOSTCONDITION(Condition) COL_THROW_POSTCONDITION(Condition, #Condition)
   #define COLMSGPOSTCONDITION(Condition, Message) COL_THROW_MSG_POSTCONDITION(Condition, #Condition, Message)
#else
   #define COLPRECONDITION(Condition)
   #define COLMSGPRECONDITION(Condition, Message)
   #define COLPOSTCONDITION(Condition)
   #define COLMSGPOSTCONDITION(Condition, Message)
#endif

// TODO - upgrade this code and put it into its own header.  We
// could use this code to check that the COLuint32 etc. declarations are
// accurate for a given platform.
// If the assert CONDITION_ is false, then there is no definition of the
// struct STATIC_ASSERTION_FAILURE, so sizeof() gives a compile time error.
// Otherwise if CONDITION_ is true, there is an empty definition for
// STATIC_ASSERTION_FAILURE and sizeof() works normally.
// COL_STATIC_ASSERT( sizeof(A) > 2 );
template <bool IsFalse> struct STATIC_ASSERTION_FAILURE;
template <> struct STATIC_ASSERTION_FAILURE<true>{};
#define COL_STATIC_ASSERT( CONDITION_ ) sizeof( STATIC_ASSERTION_FAILURE< (bool)(CONDITION_) > );

#endif // end of defensive include
