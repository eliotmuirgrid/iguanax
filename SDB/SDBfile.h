#ifndef __SDB_FILE_H__
#define __SDB_FILE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBfile
//
// Description:
//
// Helper routine to create log file if it doesn't exist
//
// Author: Eliot Muir
// Date:   Wednesday 22 February 2023 - 03:22PM
// ---------------------------------------------------------------------------
#include <COL/COLclosure.h>
#include <COL/COLminimumInclude.h>
#include <COL/COLstring.h>

class SCKloop;

class SDBfile {
public:
   COLstring LogFile1;
   COLstring LogFile2;
   bool BothDir { false };

   bool is_null() const;
};

COLostream& operator<<(COLostream& Stream, const SDBfile& LogFile);

int SDBcheckLogDir(const COLstring& FilePath);  // Currently this will throw if it fails.

// This will create the file if it does not exist.
int SDBopenLogFile(const COLstring& FilePath, bool Append = true);

int SDBreplaceLogFile(const COLstring& FilePath);  // This one will truncate the log file

// SDBseekLogFile(FD, 0, true)   - get current file position
// SDBseekLogFile(FD, -10, true) - move back 10 bytes
// SDBseekLogFile(FD, 10, true)  - move forward 10 bytes
// SDBseekLogFile(FD, 100)       - seek to the 100th byte in the file
// SDBseekLogFile(FD, -1)        - seek to the last byte in the file (a subsequent read will return the byte)
// SDBseekLogFile(FD, 0)         - seek to the beginning of the file
COLint64 SDBseekLogFile(int Descriptor, COLint64 Offset, bool Relative = false);
COLint64 SDBseekLogFileEOF(int Descriptor);
int  SDBreadLogFile(int Descriptor, char* pBuffer, int Size);
int  SDBwriteLogFile(int Descriptor, const char* pBuffer, int Size);
int  SDBcommitLogFile(int Descriptor);
void SDBcloseLogFile(int& Descriptor);

void SDBstreamFile(SCKloop* pLoop, int FileDescriptor, COLclosure1<COLstring>* pCallback);

#endif // end of defensive include