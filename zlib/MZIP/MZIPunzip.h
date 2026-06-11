#ifndef __MZIP_UNZIP_H__
#define __MZIP_UNZIP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: MZIPunzip
//
// Description:
//
// MZIPunzip class.
//
// Author: Dmitri Klimenko
// Date:   Saturday, June 6th, 2009 @ 11:23:03 AM
//---------------------------------------------------------------------------

#include <COL/COLarray.h>

#include <MZIP/unzip.h> // need to plug this in for the obscure handles FIXME

class COLstring;
class COLostream;


void MZIPunzipList(unzFile pUnzipFile);

void MZIPunzipExtract
(
   unzFile pUnzipFile,
   const COLstring& DestPath,
   bool ExtractWithoutPath,
   bool Overwrite,
   const char* pPassword,
   char (*PromptFunc)(const COLstring& Destination, const COLstring& WriteFileName),
   COLostream* pDisplayStream,
   COLarray <COLstring> *pFileList = NULL
);

void MZIPunzipExtractOneFile
(
   unzFile pUnzipFile,
   const COLstring& DestPath,
   const char* FileName,
   bool ExtractWithoutPath,
   bool Overwrite,
   const char* pPassword,
   char (*PromptFunc)(const COLstring& Destination, const COLstring& WriteFileName),
   COLostream* pDisplayStream
);

unzFile MZIPunzipOpen(const char* FileName);

void MZIPunzipClose(unzFile pUnzipFile);

char MZIPunzipFileExistsPrompt(const COLstring& Destination, const COLstring& WriteFileName);

// a simple function that does it all for you -
//  * opens the zip file
//  * extracts it to specified directory
//  * closes the handle, even if an exception gets thrown
void MZIPunzipUnpackZipFile
(
   const COLstring& ArchiveFile,
   const COLstring& Destination,
   bool Overwrite,
   char (*PromptFunc)(const COLstring&, const COLstring&),
   COLostream* pLogStream,
   COLarray <COLstring> *pFileList = NULL
);

unzFile MZIPunzipOpenMem(const void* pBuffer, int Size);

int MZIPcurrentFileName(unzFile pUnzipFile, unz_file_info* pFileInfo, COLstring* pFileName);

#endif // end of defensive include
