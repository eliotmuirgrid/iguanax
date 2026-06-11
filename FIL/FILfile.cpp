//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: FILfile
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Sun 21 Mar 2021 21:52:19 EDT
//---------------------------------------------------------------------------

#include <FIL/FILfile.h>
#include <FIL/FILpathUtils.h>

#include <COL/COLioCheck.h>
#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <sys/stat.h>
#include <fcntl.h>

#ifndef _WIN32
#   include <unistd.h>
#else
#   include <io.h>
#   include <stdio.h>
#endif

#include <errno.h>
#include <string.h>

#ifndef O_BINARY
#  define O_BINARY 0
#endif

FILfile::FILfile(const COLstring& FileName, FILmode Mode) : m_FileNo(-1) {
   COL_METHOD(FILfile::FILfile);
   open(FileName, Mode);
}

FILfile::FILfile() : m_FileNo(-1) {
   COL_METHOD(FILfile::FILfile);
}

FILfile::~FILfile(){
   COL_METHOD(FILfile::~FILfile);
   close();
}

#ifdef _WIN32
#  define FILopen ::_open
#  define FILseek _lseeki64
#  define FILfsync _commit
#else
#   define FILopen ::open
#   define FILseek lseek
#   define FILfsync fsync
#endif

void FILfile::open(const COLstring& FileName, FILmode Mode){
   COL_METHOD(FILfile::open);
   m_FileName = FILpathResolve(FileName);
   int Options=0;
   switch(Mode) {
      case FILfile::Read:    Options = O_BINARY | O_RDONLY; break;
      case FILfile::Write:   Options = O_BINARY | O_RDWR;   break;
      case FILfile::Append:  Options = O_BINARY | O_RDWR | O_CREAT | O_APPEND; break;
      case FILfile::Rewrite: Options = O_BINARY | O_RDWR | O_CREAT | O_TRUNC;  break;
      default: throw COLerror("Unknown file parameter", 0); break;
   }
#ifdef _WIN32
   Options |= O_NOINHERIT; // prevents permission issues for FILsaveAtomic()
#endif
   if (Mode == FILfile::Read){
      m_FileNo = FILopen(FileName.c_str(), Options);
   } else {
      m_FileNo = FILopen(FileName.c_str(), Options, S_IREAD | S_IWRITE);
   }
   if (m_FileNo == -1){
      int LastError = errno;
      throw COLerror("Failed to open '" +  m_FileName + "'. " + COLerrorString(LastError), LastError);
   }
   COL_VAR(m_FileNo);
}

void FILfile::close(){
   COL_METHOD(FILfile::close);
   COL_VAR(m_FileNo);
   if (m_FileNo != -1){
      ::close(m_FileNo);
      m_FileNo = -1;
   }
}

bool FILfile::isOpen() const{
   COL_METHOD(FILfile::isOpen);
   return m_FileNo != -1;
}

int FILfile::position() const{
   COL_METHOD(FILfile::position);
   int Result = FILseek(m_FileNo, 0, SEEK_CUR);
   if (Result == -1){
      int LastError = errno;
      throw COLerror("Position failed on file '" + m_FileName + "' " + COLerrorString(LastError), LastError);
   }
   return Result;
}

void FILfile::setPosition(int NewPosition){
   COL_METHOD(FILfile::setPosition);
   int Result = FILseek(m_FileNo, NewPosition, SEEK_SET);
   if (Result == -1){
      int LastError = errno;
      throw COLerror("setPosition failed on file '" + m_FileName + "' " + COLerrorString(LastError), LastError);
   }
}

int FILfile::read(void* pBuffer, int SizeOfBuffer){
   COL_METHOD(FILfile::read);
   COLassertIfMainThread();
   int AmountRead = ::read(m_FileNo, pBuffer, SizeOfBuffer);
   if (AmountRead == -1){
      int LastError = errno;
      throw COLerror("Read failed on file '" + m_FileName + "' " + COLerrorString(LastError), LastError);
   }
   COL_VAR(AmountRead);
   return AmountRead;
}

int FILfile::read(COLstring* pString){
   COL_METHOD(FILfile::read);
   int AmountRead = FILfile::read(pString->get_buffer(), pString->capacity()-1);
   pString->setSize(AmountRead);
   return AmountRead;
}

COLuint32 FILfile::write(const void* pBuffer, COLuint32 SizeOfBuffer){
   COL_METHOD(FILfile::write);
   COLassertIfMainThread();
   int AmountWritten = ::write(m_FileNo, pBuffer, SizeOfBuffer);
   if (AmountWritten == -1){
      int LastError = errno;
      throw COLerror("Write failed on file " + m_FileName + "' " + COLerrorString(LastError), LastError);
   }
   return AmountWritten;
}

int FILfile::write(const COLstring& Value){
   COL_METHOD(FILfile::write);
   return write(Value.c_str(), Value.size());
}

int FILfile::size() const{
   COL_METHOD(FILfile::size);
   COLPRECONDITION(m_FileNo != -1);
   int OldPosition = position();
   int Result = lseek(m_FileNo, 0, SEEK_END);
   lseek(m_FileNo, OldPosition, SEEK_SET);
   return Result;
}

void FILfile::flush(){
   COL_METHOD(FILfile::flush);
   int Result = FILfsync(m_FileNo);
   if (Result == -1){
      int LastError = errno;
      throw COLerror("Flush failed on file '" + m_FileName + "' " + COLerrorString(LastError), LastError);
   }
}
