//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: MZIPunzip
//
// Description:
//
// Implementation
//
// Author: Dmitri Klimenko
// Date:   Saturday, June 6th, 2009 @ 11:23:03 AM
//---------------------------------------------------------------------------

#include "MZIPunzip.h"
#include "MZIPzip.h"
#include <MZIP/unzip.h>

#include <COL/COLerror.h>
#include <COL/COLostream.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>

#ifndef _WIN32
# include <unistd.h>
# include <utime.h>
#else
# include <direct.h>
# include <io.h>
#endif

#define CASESENSITIVITY (0)
#define MZIP_UNZIP_WRITE_BUFFER_SIZE (8192)

#ifdef _WIN32
#define USEWIN32IOAPI
#include "iowin32.h"
#endif

#include <COL/COLlog.h>
COL_LOG_MODULE;

/* change_file_date : change the date/time of a file
    filename : the filename of the file where date/time must be modified
    dosdate : the new date at the MSDos format (4 bytes)
    tmu_date : the SAME new date at the tm_unz format */
void MZIPunzipChangeFileDate(const char* filename, uLong dosdate, tm_unz tmu_date){
#ifdef _WIN32
  HANDLE hFile;
  FILETIME ftm,ftLocal,ftCreate,ftLastAcc,ftLastWrite;

  hFile = CreateFile(filename,GENERIC_READ | GENERIC_WRITE,
                      0,NULL,OPEN_EXISTING,0,NULL);
  GetFileTime(hFile,&ftCreate,&ftLastAcc,&ftLastWrite);
  DosDateTimeToFileTime((WORD)(dosdate>>16),(WORD)dosdate,&ftLocal);
  LocalFileTimeToFileTime(&ftLocal,&ftm);
  SetFileTime(hFile,&ftm,&ftLastAcc,&ftm);
  CloseHandle(hFile);
#else
  struct utimbuf ut;
  struct tm newdate;
  newdate.tm_sec = tmu_date.tm_sec;
  newdate.tm_min=tmu_date.tm_min;
  newdate.tm_hour=tmu_date.tm_hour;
  newdate.tm_mday=tmu_date.tm_mday;
  newdate.tm_mon=tmu_date.tm_mon;
  if (tmu_date.tm_year > 1900)
      newdate.tm_year=tmu_date.tm_year - 1900;
  else
      newdate.tm_year=tmu_date.tm_year ;
  newdate.tm_isdst=-1;

  ut.actime=ut.modtime=mktime(&newdate);
  utime(filename,&ut);
#endif
}

void MZIPunzipList(unzFile pUnzipFile){
   COL_FUNCTION(MZIPunzipList);
   // this function is FIXME - prints to stdout
   // it also can be used as an example of zip content listing
   unz_global_info GlobalInfo;
   int InfoErr = unzGetGlobalInfo (pUnzipFile,&GlobalInfo);
   if (InfoErr!=UNZ_OK){
      COL_ERROR_STREAM("Error " << InfoErr << " with zipfile in unzGetGlobalInfo", COLerror::PreCondition);
   }
   printf(" Length     Method   Size  Ratio   Date      Time   CRC-32     Name\n");
   printf(" ---------  ------   ----- ----  ----------  -----  --------   ----\n");
   for (unsigned int EntryIndex=0; EntryIndex < GlobalInfo.number_entry; EntryIndex++){
      // FIXME factor out the corresponding bit from the unzipping function
      char FileNameInZip[256];
      unz_file_info FileInfo;
      uLong Ratio=0;
      const char *StringMethod;
      char CharCrypt=' ';
      int FileInfoErr = unzGetCurrentFileInfo(pUnzipFile,&FileInfo,FileNameInZip,sizeof(FileNameInZip),NULL,0,NULL,0);
      if (FileInfoErr!=UNZ_OK){
         COL_ERROR_STREAM("Error " << FileInfoErr << " with zipfile in unzGetCurrentFileInfo", COLerror::PreCondition);
      }
      if (FileInfo.uncompressed_size>0){
         Ratio = (FileInfo.compressed_size*100)/FileInfo.uncompressed_size;
      }

      /* display a '*' if the file is crypted */
      if ((FileInfo.flag & 1) != 0){
         CharCrypt='*';
      }

      if (FileInfo.compression_method==0){
         StringMethod="Stored";
      } else if (FileInfo.compression_method==Z_DEFLATED){
         uInt iLevel=(uInt)((FileInfo.flag & 0x6)/2);
         if (iLevel==0){
            StringMethod="Defl:N";
         } else if (iLevel==1){
            StringMethod="Defl:X";
         } else if ((iLevel==2) || (iLevel==3)){
           StringMethod="Defl:F"; /* 2:fast , 3 : extra fast*/
         }
      } else {
         StringMethod="Unkn. ";
      }

      printf("%10lu  %6s%c%7lu %3lu%%  %4.4lu-%2.2lu-%2.2lu  %2.2lu:%2.2lu  %8.8lx   %s\n",
             FileInfo.uncompressed_size, StringMethod, CharCrypt,FileInfo.compressed_size, Ratio,
             (uLong)FileInfo.tmu_date.tm_year, (uLong)FileInfo.tmu_date.tm_mon + 1,(uLong)FileInfo.tmu_date.tm_mday,
             (uLong)FileInfo.tmu_date.tm_hour, (uLong)FileInfo.tmu_date.tm_min,
             (uLong)FileInfo.crc,
             FileNameInZip);

      if ( EntryIndex+1 < GlobalInfo.number_entry){
         int NextErr = unzGoToNextFile(pUnzipFile);
         if (NextErr!=UNZ_OK){
            COL_ERROR_STREAM("Error " << NextErr << " with zipfile in unzGoToNextFile", COLerror::PreCondition);
         }
      }
   }
}

char MZIPunzipFileExistsPrompt(const COLstring& Destination, const COLstring& FileNameInZip){
   COL_FUNCTION(MZIPunzipFileExistsPrompt);
   while(true){
     COLcout << "The file \"" << FileNameInZip << "\" exists in \"" << Destination << "\". Overwrite ? [y]es, [n]o, [A]ll: " << newline;
     char ScanBuffer[128];
     int ScanSize = scanf("%1s",ScanBuffer);
     if (ScanSize != 1){ 
        COL_ERROR_STREAM("Invalid response size for overwrite prompt", COLerror::PreCondition);
     }
     char ResponseChar = toupper(ScanBuffer[0]);
     if(ResponseChar == 'Y' || ResponseChar=='N' || ResponseChar=='A'){
        return ResponseChar;
     }
   }
   return 0; // won't happen
}

void MZIPunzipWriteFileData(FILE* pFile, unzFile pUnzipFile){
   COL_FUNCTION(MZIPunzipWriteFileData);
   while(true){
      char Buffer[MZIP_UNZIP_WRITE_BUFFER_SIZE];
      int ReadResult = unzReadCurrentFile(pUnzipFile,Buffer, MZIP_UNZIP_WRITE_BUFFER_SIZE);
      if (ReadResult<0){
         COL_ERROR_STREAM("Error " << ReadResult << " returned by unzReadCurrentFile()", COLerror::PreCondition);
      }
      if(!ReadResult){
         return;
      }
      // inv: ReadResult > 0
      COLPRECONDITION( ReadResult <= MZIP_UNZIP_WRITE_BUFFER_SIZE);
      if (fwrite(Buffer, ReadResult, 1, pFile)!=1){
         COL_ERROR_STREAM("Error while writing file data", COLerror::PreCondition);
      }
   }
   COL_ERROR_STREAM("Shouldn't get here.", COLerror::PreCondition);
}

int MZIPcurrentFileName(unzFile pUnzipFile, unz_file_info* pFileInfo, COLstring* pFileName){
   COL_FUNCTION(MZIPcurrentFileName);
   COLstring& FileName = *pFileName;
   FileName.setCapacity(256);
   int FileInfoErr = unzGetCurrentFileInfo(pUnzipFile, pFileInfo, FileName.get_buffer(), FileName.capacity(), NULL, 0, NULL, 0);
   if (FileInfoErr != UNZ_OK){ 
      return FileInfoErr;
   }
   if(pFileInfo->size_filename + 1 > FileName.capacity()) {      // do it again.
      FileName.setCapacity(pFileInfo->size_filename+1);
      FileInfoErr = unzGetCurrentFileInfo(pUnzipFile, pFileInfo, FileName.get_buffer(), FileName.capacity(), NULL, 0, NULL, 0);
      if (FileInfoErr != UNZ_OK){
         return FileInfoErr;
      }
   }
   int HowBig = pFileInfo->size_filename;
   COLASSERT(HowBig < FileName.capacity());
   FileName.setSize(HowBig);
   COL_VAR(FileName);
   COL_VAR(HowBig);
   return UNZ_OK;
}


void MZIPunzipExtractCurrentFile
(
   unzFile pUnzipFile,
   const COLstring& DestPath,
   bool ExtractWithoutPath,
   bool* pOverwrite, 
   // ^ the Overwrite flag can be set to true upon an "A" answer to "Overwrite? Y/N/A",
   // which is supposed to affect the subsequent invocations,
   // that's why it's passed as a pointer to a variable stored by the caller
   const char* pPassword,
   char (*PromptFunc)(const COLstring& Destination, const COLstring& WriteFileName),
   COLostream* pDisplayStream,
   COLarray <COLstring> *pFileList = NULL // Will add the filename if not NULL
)
{
   COL_FUNCTION(MZIPunzipExtractCurrentFile);
   unz_file_info FileInfo;
   COLstring FileNameInZip;
   if (UNZ_OK != MZIPcurrentFileName(pUnzipFile, &FileInfo, &FileNameInZip)){
      COL_ERROR_STREAM_PLAIN("MZIPcurrentFileName failed to extract file name.", COLerror::PreCondition);
   }
   COL_TRC("Processing " << FileNameInZip);
   if (pFileList){
      pFileList->push_back (FileNameInZip);
   }

   COLstring FileNameWithoutPath = FILpathName(FileNameInZip);
   if (FileNameWithoutPath.is_null()){
      // a directory - ends in slash
      if (!ExtractWithoutPath){
         COLstring RelPath = FILpathSepRemove(FileNameInZip);
         COLstring FullPath = FILpathAppend(DestPath, RelPath);
         COL_DBG("Creating directory: " << FullPath);
         FILmakeFullDir(FullPath, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
         if(pDisplayStream){
            (*pDisplayStream) << "DIR  " << FileNameInZip << newline;
         }
      }
      return;
   }
   // inv: a file

   COLstring WriteFileName = FileNameInZip;
   if (ExtractWithoutPath){
     WriteFileName = FileNameWithoutPath;
   }
   COL_TRC("WriteFileName: " << WriteFileName);

   COLstring WritePathFileName = FILpathAppend(DestPath, WriteFileName);
   COL_TRC("WritePathFileName: " << WritePathFileName);

   if (FILfileExists(WritePathFileName)){
      COL_DBG("File exists." << newline);
      if(*pOverwrite){// remove the file first - failure to open may result from odd attributes
         FILremoveWithThrow(WritePathFileName);// if this fails, we're toast - so we let the exception fly
      } else if(!PromptFunc){
         // null prompt func and file exists - we can't deal with this
         COL_ERROR_STREAM("File \"" << WriteFileName << "\" exists in \"" << DestPath << "\" - not overwriting.", COLerror::PreCondition);
      } else {
         // need to prompt
         char PromptResponse = PromptFunc(DestPath,  WriteFileName);
         // note that we always expect a PromptFunc to be non-null and usable
         // it's easy to supply custom functions that have the desired effect, including:
         // - always overwrite = return 'Y'
         // - always skip = return 'N'
         // - throw an exception mentioning the filename
         COLPRECONDITION( PromptResponse == 'Y' || PromptResponse == 'N' || PromptResponse == 'A');
         if(PromptResponse == 'N'){
            return;
         }
         if (PromptResponse == 'A'){
            *pOverwrite = true;
         }
      }
   }

#ifdef _WIN32
   COLstring WinPath;
   wchar_t* pWinPath = FILmangleFileNameForWindows(WritePathFileName, &WinPath);
   FILE* pFile = _wfopen(pWinPath, L"wb");
#else
   FILE* pFile = fopen(WritePathFileName.c_str(),"wb");
#endif
   if(!pFile){
      COL_DBG("Failed to open the file: " << WritePathFileName);
      if(!ExtractWithoutPath && FileNameWithoutPath != FileNameInZip){
         /* some zipfile don't contain directory alone before file */
         COLstring ZipPath = FILpathDir(FileNameInZip);
         COL_DBG("ZipPath=" << ZipPath);
         COLstring DestZipPath = FILpathSepRemove(FILpathAppend(DestPath, ZipPath));
         COL_DBG("DestZipPath=" << DestZipPath);
         if(!FILfileExists(DestZipPath) || !FILisDirectory(DestZipPath.c_str())){
            COL_DBG("Path doesn't exist - creating: " << DestZipPath);
            FILmakeFullDir(DestZipPath, FIL_USER_RWX|FIL_GROUP_RX|FIL_OTHER_RX);
            COL_DBG("Retrying to open file " << WritePathFileName);
#ifdef _WIN32
            pFile = _wfopen(pWinPath, L"wb");
#else
            pFile = fopen(WritePathFileName.c_str(),"wb");
#endif
            if(pFile){
               COL_DBG("Now file is opened." << newline);
            }
         }
      }
   }

   if(!pFile){
      COL_ERROR_STREAM("Failed to open output file for writing: " << WritePathFileName, COLerror::PreCondition);
   }
   COL_TRC("File opened for writing: " << WritePathFileName);
   int OpenErr = unzOpenCurrentFilePassword(pUnzipFile,pPassword);
   if (OpenErr!=UNZ_OK){
      fclose(pFile);
      COL_ERROR_STREAM("Error " << OpenErr << " with zipfile in unzOpenCurrentFilePassword", COLerror::PreCondition);
   }
   COL_DBG("Current file opened in zip, writing data...");
   try{
      MZIPunzipWriteFileData(pFile, pUnzipFile);
   } catch(const COLerror& Error){
      fclose(pFile);
      unzCloseCurrentFile(pUnzipFile);
      COL_ERROR_STREAM(
         "Error while writing file data for file: " << FileNameInZip << " to " << WriteFileName << ":" << newline
         << Error, COLerror::PreCondition);
   }
   // file written ok - close file
   fclose(pFile);
   pFile=0;
   // close current file in zip
   int CloseErr = unzCloseCurrentFile(pUnzipFile);
   if (CloseErr!=UNZ_OK){
      COL_ERROR_STREAM("Error " << CloseErr << "  with zipfile in unzCloseCurrentFile", COLerror::PreCondition);
   }
   MZIPunzipChangeFileDate(WritePathFileName.c_str(),FileInfo.dosDate, FileInfo.tmu_date);
   COL_TRC("OK - file " << WriteFileName);
   if(pDisplayStream){
      (*pDisplayStream) << "FILE " << FileNameInZip << newline;
   }
}

void MZIPunzipExtract
(
   unzFile pUnzipFile,
   const COLstring& DestPath,
   bool ExtractWithoutPath,
   bool Overwrite,
   const char* pPassword,
   char (*PromptFunc)(const COLstring& Destination, const COLstring& WriteFileName),
   COLostream* pDisplayStream,
   COLarray <COLstring> *pFileList
)
{
   COL_FUNCTION(MZIPunzipExtract);
   COL_VAR4(DestPath, ExtractWithoutPath, Overwrite, pPassword);
   unz_global_info GlobalInfo;
   int GlobalInfoErr = unzGetGlobalInfo (pUnzipFile,&GlobalInfo);
   if (GlobalInfoErr!=UNZ_OK){
      COL_ERROR_STREAM("Error " << GlobalInfoErr << " with zipfile in unzGetGlobalInfo()", COLerror::PreCondition);
   }
   if(!DestPath.is_null()){
      COLPRECONDITION( FILfileExists(DestPath) && FILisDirectory(DestPath.c_str()) );
   }
   int FirstErr = unzGoToFirstFile(pUnzipFile);
   if (FirstErr!=UNZ_OK){
      COL_ERROR_STREAM("Error " << FirstErr << " with zipfile in unzGoToFirstFile", COLerror::PreCondition);
   }
   for(int EntryIndex=0; EntryIndex < GlobalInfo.number_entry; EntryIndex++){
      try{
         MZIPunzipExtractCurrentFile(pUnzipFile, DestPath, ExtractWithoutPath, &Overwrite, pPassword, PromptFunc, pDisplayStream, pFileList);
      } catch(const COLerror& Error){
         COL_ERROR_STREAM("Failed to extract current file " << EntryIndex << ":" << newline << Error << newline, COLerror::PreCondition);
      }

      if (EntryIndex+1 < GlobalInfo.number_entry) {
         int NextErr = unzGoToNextFile(pUnzipFile);
         if (NextErr!=UNZ_OK){
            COL_ERROR_STREAM("Error " << NextErr << " with zipfile in unzGoToNextFile", COLerror::PreCondition);
         }
      }
   }
}

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
)
{
    int LocateErr = unzLocateFile(pUnzipFile,FileName,CASESENSITIVITY);
    if(LocateErr!=UNZ_OK){
       COL_ERROR_STREAM("Error " << LocateErr << " - failed to locate file " << FileName << " in the zipfile", COLerror::PreCondition);
    }
    try{
      MZIPunzipExtractCurrentFile(pUnzipFile, DestPath, ExtractWithoutPath, &Overwrite, pPassword, PromptFunc, pDisplayStream);
    }catch(const COLerror& Error){
       COL_ERROR_STREAM("Failed to extract file " << FileName << ":" << newline << Error, COLerror::PreCondition);
    };
}

unzFile MZIPunzipOpen(const char* FileName){
   COL_FUNCTION(MZIPunzipOpen);
   COL_VAR(FileName);
#ifdef USEWIN32IOAPI
   COL_TRC("Using WIN32IOAPI");
   zlib_filefunc_def ffunc;
   fill_win32_filefunc(&ffunc);
   ffunc.zopen_file = win32_open_wide_file_func;
   return unzOpen2(FileName,&ffunc);
#else
   COL_TRC("Not using WIN32IOAPI");
   return unzOpen(FileName);
#endif
}

extern "C" void fill_memory_filefunc (zlib_filefunc_def* pzlib_filefunc_def);

unzFile MZIPunzipOpenMem(const void* pBuffer, int Size) {
   COL_FUNCTION(MZIPunzipOpenMem);
   COL_VAR(Size);
   zlib_filefunc_def ffunc;
   fill_memory_filefunc(&ffunc);
   COLstring EncodedMemoryInfo;
   COLostream Stream(EncodedMemoryInfo);
   Stream << pBuffer << "+" << Size; // <hex address>+<decimal file length>
   COL_VAR(EncodedMemoryInfo);
   return unzOpen2(EncodedMemoryInfo.c_str(), &ffunc);
}

void MZIPunzipClose(unzFile pUnzipFile){
   COL_FUNCTION(MZIPunzipClose);
   int CloseErr = unzClose(pUnzipFile);
   if(CloseErr != UNZ_OK){
      COL_ERROR_STREAM("Error " << CloseErr << " from unzClose()", COLerror::PreCondition);
   }
}

// a simple function that does it all, guarding against file handle leakage

void MZIPunzipUnpackZipFile
(
   const COLstring& ArchiveFile,
   const COLstring& Destination,
   bool Overwrite,
   char (*PromptFunc)(const COLstring&, const COLstring&),
   COLostream* pLogStream,
   COLarray <COLstring> *pFileList
)
{
   COL_FUNCTION(MZIPunzipUnpackZipFile);
   unzFile pUnzipFile = MZIPunzipOpen(ArchiveFile.c_str());
   if(!pUnzipFile){
      COL_ERROR_STREAM("Failed to open file for unzipping: " << ArchiveFile << ". The file might not be a valid zip archive", COLerror::PreCondition);
   }
   try{
      MZIPunzipExtract(pUnzipFile, Destination, false, Overwrite, 0, PromptFunc, pLogStream, pFileList);
   } catch(const COLerror& Error){
      try{
         MZIPunzipClose(pUnzipFile);
      } catch(...) {
         // ignore the close exception - throw the original error
      }
      throw;
   }
   MZIPunzipClose(pUnzipFile);  
}
