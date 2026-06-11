//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLerror
//
// Description: Implementation
//
// Error class used in project
//
// Author: Eliot Muir
// Date:   Wed 09/09/1998
//---------------------------------------------------------------------------
#include "COLerror.h"
#include "COLvar.h"
#include "COLlog.h"
COL_LOG_MODULE;

#ifdef _WIN32
   #include "COLwindows.h"
   #include <dbghelp.h>
#else // POSIX
   #include <errno.h>
   #include "COLmutex.h"
   #include "COLlocker.h"
   #include <execinfo.h>
   #include <stdio.h>
   #include <stdlib.h>
   #include <unistd.h>
#endif

#include <COL/COLdateTime.h>

COLstring COLerrorString(long ErrorCode){
#ifdef _WIN32
   char* MessageBuffer;
   DWORD dwFormatFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
                         FORMAT_MESSAGE_IGNORE_INSERTS |
                         FORMAT_MESSAGE_FROM_SYSTEM;

   ::FormatMessage(dwFormatFlags,
      NULL, // module to get message from (NULL == system)
      ErrorCode,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // default language
      (char*) &MessageBuffer,
      0,
      NULL);
   COLstring Result = MessageBuffer;
   LocalFree(MessageBuffer);
   return Result;
#else
   COLstring Result;
   Result.setCapacity(255);
   strerror_r(ErrorCode, Result.get_buffer(), Result.capacity()-1);
   Result.setSize(strlen(Result.c_str()));
   return Result;
#endif
}

#ifdef _WIN32
void COLthrowWin32Error(long ErrorCode,const char* Description, int LineNumber,const char* FileName){
   COLsinkString ErrorSink;
   COLostream MessageStream(ErrorSink);
   if (Description){
      MessageStream << Description;
   }
   if (LineNumber != -1 && (FileName != NULL)){
      MessageStream << " on line " << LineNumber << " in " << FileName << " - ";
   }
   MessageStream << COLerrorString(ErrorCode);
   throw COLerror(ErrorSink.string(), (COLuint32)ErrorCode);
}
#endif // _WIN32

long COLlastError(){
 #ifdef _WIN32
   return ::GetLastError();
#else
   return errno;
#endif
}

COLstring COLlastErrorAsString(){
   int LastError = COLlastError();
   COL_FUNCTION(COLlastError);
   return COLerrorString(LastError);
}

static COLstring COLerrorAsString(int ErrorCode){
   switch ((COLerror::COLerrorCode)ErrorCode) {
   case COLerror::PreCondition:       return "PreCondition";
   case COLerror::PostCondition:      return "PostCondition";
   case COLerror::FileError:          return "FileError";
   case COLerror::SerializationError: return "SerializationError";
   case COLerror::SystemError:        return "SystemError";
   case COLerror::InvalidDateTime:    return "InvalidDateTime";
   case COLerror::ParsingError:       return "ParsingError";
   case COLerror::Cancel:             return "Cancel";
   default:
      break;
   }
   return COLintToString(ErrorCode);
}

COLerror::COLerror(const COLstring& Message, int Line, const char* File, int Code) :
   m_Description(Message),
   m_Code(Code),
   m_File(File),
   m_Line(Line)
{
   if (Code && Message.size()) {
      COL_ERR("Message: `" << Message << "`, Code: " << COLerrorAsString(Code)
         << ", File: " << File << ", Line " << Line << COLbacktrace());
   }
}

COLerror::COLerror(const COLstring& Message, int Code) : m_Description(Message),  m_Code(Code),  m_Line(0){
   if (Code && Message.size()) {
      COL_ERR("Message: `" << Message << "`, Code: " << COLerrorAsString(Code) << COLbacktrace());
   }
}

COLerror::COLerror(const COLerror& Error, int NewCode) :
   m_Description(Error.m_Description),
   m_File(Error.m_File),
   m_Line(Error.m_Line),
   m_Code(NewCode) // assign new code
{}

COLerror::COLerror(const COLerror& Error) :
   m_Description(Error.m_Description),
   m_Code(Error.m_Code),
   m_File(Error.m_File),
   m_Line(Error.m_Line)
{}

COLerror::~COLerror() {
}

void COLerror::setDescription(const COLstring& Description){
   m_Description = Description;
}

void COLerror::setCode(int NewCode){
   m_Code = NewCode;
}

COLerror& COLerror::operator=(const COLerror& Error){
   m_Description = Error.m_Description;
   m_Code        = Error.m_Code;
   m_File        = Error.m_File;
   m_Line        = Error.m_Line;

   return *this;
}

bool COLerror::operator==(const COLerror& Rhs) const{
   return m_Code == Rhs.m_Code && m_Description == Rhs.m_Description;
}

COLostream& operator<<(COLostream& Stream, const COLerror& Error){
   Stream << "Error Code: 0x" << hex << Error.code() << decimal << newline
          << "Description: " << Error.description() << newline;
   if (Error.file().size() > 0){
      Stream << "Line: " << Error.line() << newline
             << "File: " << Error.file() << newline;
   }
   return Stream;
}

void COLabort(){
   abort();
}

void COLdefaultAbortCallback(const COLstring& ErrorString){
   COL_FUNCTION(COLdefaultAbortCallback);
   COL_ERR(ErrorString << COLbacktrace());
   COLcout << "Fatal Error. Abort with message: " << ErrorString << COLbacktrace() << flush;
}

COLabortCallback s_pAbortCallback = &COLdefaultAbortCallback;

void COLsetAbortCallback(COLabortCallback pAbortCallback){
   COL_FUNCTION(COLsetAbortCallback);
   COL_VAR(pAbortCallback);
   s_pAbortCallback = pAbortCallback;
}

void COLabortWithMessage(const COLstring& ErrorString){
   COL_FUNCTION(COLabortWithMessage);
   (*s_pAbortCallback)(ErrorString);
   abort();
}

COLstring s_COLdebugSearchPath;

void COLsetDebugSearchPath(const COLstring& Path){
   COL_FUNCTION(COLsetDebugSearchPath);
   COL_VAR(Path);
   s_COLdebugSearchPath = Path;
}

COLstring COLdebugSearchPath(){
   COL_FUNCTION(COLdebugSearchPath);
   return s_COLdebugSearchPath;
}