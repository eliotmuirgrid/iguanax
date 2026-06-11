// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBfile
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday 22 February 2023 - 03:22PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <FIL/FILosDefs.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <SCK/SCKloop.h>
#include <SDB/SDBfile.h>
COL_LOG_MODULE;

bool SDBfile::is_null() const { return BothDir ? LogFile1.is_null() && LogFile2.is_null() : LogFile1.is_null(); }

COLostream& operator<<(COLostream& Stream, const SDBfile& LogFile) {
   Stream << LogFile.LogFile1;
   if (LogFile.BothDir) { Stream << " | " << LogFile.LogFile2; }
   return Stream;
}

int SDBcheckLogDir(const COLstring& FilePath) {
   COL_FUNCTION(SDBcheckLogDir);
   COLstring Dir = FILparentDir(FilePath);
   COL_VAR2(Dir, FilePath);
   if (Dir.is_null()) { return 0; }
   if (!FILfileExists(Dir)) {
      FILmakeFullDir(Dir, FIL_USER_RWX);
   }
   return FILisDirectory(Dir) ? 0 : EEXIST;
}

// This will create the file if it does not exist.
int SDBopenLogFile(const COLstring& FilePath, bool Append) {
   // COL_FUNCTION(SDBopenLogFile);
   int Flags = O_CREAT | O_RDWR | O_BINARY | O_NOINHERIT;
   if (Append) { Flags |= O_APPEND; }
   int FD = FILopen(FilePath.c_str(), Flags, S_IREAD | S_IWRITE);
   if (-1 == FD) { return -errno; }
   return FD;
}

// This will replace the file even it it exists
int SDBreplaceLogFile(const COLstring& FilePath) {
   COL_FUNCTION(SDBreplaceLogFile);
   int FD = FILopen(FilePath.c_str(), O_CREAT | O_RDWR | O_TRUNC | O_BINARY | O_NOINHERIT, S_IREAD | S_IWRITE);
   if (-1 == FD) { return -errno; }
   return FD;
}

COLint64 SDBseekLogFile(int Descriptor, COLint64 Offset, bool Relative) {
   COL_FUNCTION(SDBseekLogFile);
   int Whence = Relative ? SEEK_CUR : (Offset >= 0 ? SEEK_SET : SEEK_END);
   // Windows FILlseek returns a 32-bit int, limits seeking to 2^31
   COLint64 Position = FILlseek64(Descriptor, Offset, Whence);
   if (-1 == Position) { return -errno; }
   return Position;
}

COLint64 SDBseekLogFileEOF(int Descriptor) {
   COL_FUNCTION(SDBseekLogFileEOF);
   COLint64 Position = FILlseek64(Descriptor, 0, SEEK_END);
   if (-1 == Position) { return -errno; }
   return Position;
}

int SDBreadLogFile(int Descriptor, char* pBuffer, int Size) {
   COL_FUNCTION(SDBreadLogFile);
   int Amount = FILread(Descriptor, pBuffer, Size);
   if (Amount == -1) { return -errno; }
   return Amount;
}

int SDBwriteLogFile(int Descriptor, const char* pBuffer, int Size) {
   // COL_FUNCTION(SDBwriteLogFile);
   int Amount = FILwrite(Descriptor, pBuffer, Size);
   if (Amount == -1) { return -errno; }
   return Amount;
}

int SDBcommitLogFile(int Descriptor) {
   int Result = FILcommit(Descriptor);
   if (Result == -1) { return -errno; }
   return Result;
}

void SDBcloseLogFile(int& Descriptor) {
   COL_FUNCTION(SDBcloseLogFile);
   if (FILclose(Descriptor) == -1) {
      COL_WRN("Failed to close file descriptor " << Descriptor << ": " << strerror(errno));
   }
   Descriptor = -1;
}

void SDBstreamWorker(int FileDescriptor, COLclosure1<COLstring>* pCallback){
   COL_FUNCTION(SDBstreamWorker);
   COLstring Buffer;
   Buffer.setCapacity(1024*1024);
   do {
      int AmountRead = SDBreadLogFile(FileDescriptor, Buffer.get_buffer(), Buffer.capacity()-1);
      if (AmountRead <= 0 ){
         // we are done
        pCallback->run("");
         delete pCallback;
         return;
      }
      Buffer.setSize(AmountRead);
      pCallback->run(Buffer);
   } while(true);
}

void SDBstreamFile(SCKloop* pLoop, int FileDescriptor, COLclosure1<COLstring>* pCallback){
   COL_FUNCTION(SDBstreamFile);
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&SDBstreamWorker, FileDescriptor, pCallback));
}