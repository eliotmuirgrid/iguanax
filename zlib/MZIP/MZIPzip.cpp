//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: MZIPzip
//
// Description:
//
// Implementation
//
// Author: Ivan Hamer
// Date:   Monday, June 6th, 2011
//---------------------------------------------------------------------------
#pragma hdrstop

#include <sys/stat.h>
#include <time.h>

#include "MZIPzip.h"
#include "MZIPmemory.h"
#include <FIL/FILutils.h>
#include <COL/COLpair.h>
#include <COL/COLlist.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <MZIP/zip.h>

typedef COLpair<COLstring,COLstring> MZIPfileEntry; // path, contents
typedef COLlist<MZIPfileEntry>       MZIPfileList;

#ifdef _WIN32
#define USEWIN32IOAPI
#include <MZIP/iowin32.h>
#endif

#include <stdio.h>

static const int WRITEBUFFERSIZE = 4096;
static const int MAXFILENAME     = 1024;

static bool MZIPhasTrailingSlash(const COLstring& Path)
{
   const int Len = Path.size();
   return Len && (Path[Len-1] == '/' || Path[Len-1] == '\\');
}

static void MZIPpopulateCurrentTimestamp(tm_zip* tmzip)
{
   time_t t = ::time(NULL);
#ifdef _WIN32
   struct tm* pTm = ::localtime(&t); // ok - windows
#else
   struct tm tmStruct;
   struct tm* pTm = ::localtime_r(&t, &tmStruct);
#endif
   tmzip->tm_sec  = pTm->tm_sec;
   tmzip->tm_min  = pTm->tm_min;
   tmzip->tm_hour = pTm->tm_hour;
   tmzip->tm_mday = pTm->tm_mday;
   tmzip->tm_mon  = pTm->tm_mon;
   tmzip->tm_year = pTm->tm_year;
}

//  char*   f;            /* name of file to get info on */
//  tm_zip* tmzip;        /* return value: access, modific. and creation times */
//  uLong*  dt;           /* dostime */
static uLong MZIPfiletime(const char* f, tm_zip* tmzip, uLong* dt)
{
   int ret = 0;
#ifdef _WIN32
   FILETIME ftLocal;
   HANDLE hFind;
   WIN32_FIND_DATA  ff32;
   hFind = FindFirstFile(f,&ff32);
   if (hFind != INVALID_HANDLE_VALUE) {
      FileTimeToLocalFileTime(&(ff32.ftLastWriteTime),&ftLocal);
      FileTimeToDosDateTime(&ftLocal,((LPWORD)dt)+1,((LPWORD)dt)+0);
      FindClose(hFind);
      ret = 1;
   }
#else
   struct stat s;        /* results of stat() */
   time_t tm_t = 0;
   if (strcmp(f,"-")!=0) {
      char name[MAXFILENAME+1];
      int len = strlen(f);
      if (len > MAXFILENAME)
         len = MAXFILENAME;

      strncpy(name, f,MAXFILENAME-1);
      /* strncpy doesnt append the trailing NULL if the string is too long. */
      name[ MAXFILENAME ] = '\0';

      if (name[len - 1] == '/')
         name[len - 1] = '\0';

      /* not all systems allow stat'ing a file with / appended */
      if (stat(name,&s)==0) {
         tm_t = s.st_mtime;
         ret = 1;
      }
   }
   struct tm filedate;
   ::localtime_r(&tm_t, &filedate);
   tmzip->tm_sec  = filedate.tm_sec;
   tmzip->tm_min  = filedate.tm_min;
   tmzip->tm_hour = filedate.tm_hour;
   tmzip->tm_mday = filedate.tm_mday;
   tmzip->tm_mon  = filedate.tm_mon;
   tmzip->tm_year = filedate.tm_year;
#endif
   return ret;
}

#ifdef _WIN32
// This is a complete copy of win32_open_file_func, changed to use CreateFileW, which allows
// Unicode characters in Windows filenames. Wasn't sure about licensing for the zip library,
// which is why it's not in there directly.
typedef struct
{
   HANDLE hf;
   int error;
} WIN32FILE_IOWIN;

voidpf ZCALLBACK win32_open_wide_file_func(voidpf opaque, const char* filename, int mode){
   const char* mode_fopen = NULL;
   DWORD dwDesiredAccess, dwCreationDisposition, dwShareMode, dwFlagsAndAttributes;
   HANDLE hFile = 0;
   voidpf ret=NULL;
   COLstring Path = filename;
   COLstring WinPath;
   WIN32FILE_IOWIN w32fiow;
   dwDesiredAccess = dwShareMode = dwFlagsAndAttributes = 0;

   if ((mode & ZLIB_FILEFUNC_MODE_READWRITEFILTER) == ZLIB_FILEFUNC_MODE_READ)
   {
      dwDesiredAccess = GENERIC_READ;
      dwCreationDisposition = OPEN_EXISTING;
      dwShareMode = FILE_SHARE_READ;
   } else
      if (mode & ZLIB_FILEFUNC_MODE_EXISTING)
      {
         dwDesiredAccess = GENERIC_WRITE | GENERIC_READ;
         dwCreationDisposition = OPEN_EXISTING;
      } else
         if (mode & ZLIB_FILEFUNC_MODE_CREATE)
         {
            dwDesiredAccess = GENERIC_WRITE | GENERIC_READ;
            dwCreationDisposition = CREATE_ALWAYS;
         }

      if ((filename != NULL) && (dwDesiredAccess != 0))

         hFile = CreateFileW(FILmangleFileNameForWindows(Path, &WinPath), dwDesiredAccess, dwShareMode, NULL,
         dwCreationDisposition, dwFlagsAndAttributes, NULL);

      if (hFile == INVALID_HANDLE_VALUE)
         hFile = NULL;

      if (hFile != NULL)
      {

         w32fiow.hf = hFile;
         w32fiow.error = 0;
         ret = malloc(sizeof(WIN32FILE_IOWIN));
         if (ret == NULL)
            CloseHandle(hFile);
         else *((WIN32FILE_IOWIN*)ret) = w32fiow;
      }
      return ret;
}
#endif

int MZIPzipFiles(const COLstring& ZipFileName, const COLarray<COLstring>& FilesToZip, const COLstring& RootDir, COLstring* pErr)
{
   COL_FUNCTION(MZIPzipFiles);
   int Error = 0;
   COLstring ErrorMsg; 
   zipFile ZipFile = 0;
   FILE* InputFILE = NULL;

#ifdef USEWIN32IOAPI
   zlib_filefunc_def Ffunc;
   fill_win32_filefunc(&Ffunc);
   Ffunc.zopen_file = win32_open_wide_file_func;
   ZipFile = zipOpen2(ZipFileName.c_str(), APPEND_STATUS_CREATE, NULL, &Ffunc);
#else
   ZipFile = zipOpen(ZipFileName.c_str(), APPEND_STATUS_CREATE);
#endif

   if (ZipFile) {
      const int BufferSize = WRITEBUFFERSIZE;
      char Buffer[BufferSize];

      // for each input file to zip...
      for (int i = 0; i < FilesToZip.size(); i++) {
         COLstring FileName = RootDir + FilesToZip[i];
         COL_VAR(FileName);

         int SizeRead = 0;
         const char* FilenameInZip = FileName.c_str();
         unsigned long CrcFile=0;

         // get the input file's timestamp
         zip_fileinfo ZipFileInfo;
         ::memset(&ZipFileInfo, 0, sizeof(ZipFileInfo));
         if (MZIPhasTrailingSlash(FilesToZip[i])) {
            // Name with trailing slash is a directory
            ZipFileInfo.external_fa = 48; // empty zip directory verified in Windows XP explorer
         }
         MZIPfiletime(FilenameInZip, &ZipFileInfo.tmz_date, &ZipFileInfo.dosDate);

         Error = zipOpenNewFileInZip3(ZipFile, FilesToZip[i].c_str(), &ZipFileInfo,
            NULL,0,NULL,0,NULL /* comment*/,
            Z_DEFLATED,
            5/*opt_compress_level*/,0,
            -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
            NULL/*password*/,CrcFile);

         if (Error != ZIP_OK) {
            ErrorMsg = COLstring("Cannot open ") + FilenameInZip + " in zipfile.";
            break;
         }

         // open a zip input file to be zipped

#ifdef _WIN32
         COLstring WinPath;
         InputFILE = _wfopen(FILmangleFileNameForWindows(FileName, &WinPath), L"rb");
#else
         InputFILE = fopen(FilenameInZip,"rb");
#endif

         if (InputFILE == NULL) {
            Error = ZIP_ERRNO;
            ErrorMsg = COLstring("Cannot open file ") + FilenameInZip + " for reading.";
            break;
         }

         // read and zip an input file in BufferSize chunks...
         do {
            Error = ZIP_OK;
            SizeRead = (int)fread(Buffer, 1, BufferSize, InputFILE);
            if (SizeRead < BufferSize) {
               if (feof(InputFILE) == 0) {
                  Error = ZIP_ERRNO;
                  ErrorMsg = COLstring("Cannot read file: ") + FilenameInZip;
                  break;
               }
            }

            if (SizeRead > 0) {
               Error = zipWriteInFileInZip(ZipFile, Buffer, SizeRead);
               if (Error < 0) {
                  ErrorMsg = COLstring("Cannot write ") + FilenameInZip + " to the zipfile.";
                  break;
               }
            }
         } while ((Error == ZIP_OK) && (SizeRead > 0));

         // close zip input file
         if (InputFILE) { fclose(InputFILE); InputFILE = NULL; }

         if (Error != ZIP_OK) {
            break;
         }

         Error = zipCloseFileInZip(ZipFile);
         if (Error != ZIP_OK) {
            ErrorMsg = COLstring("Cannot close ") + FilenameInZip + " in zipfile.";
            break;
         }
      }

      if (zipClose(ZipFile,NULL) != ZIP_OK) {
         if (ErrorMsg.is_null()) {
            // only override the error message if it was previously empty.
            ErrorMsg = COLstring("Cannot close zipfile ") + ZipFileName;
         }
      }
   }
   else {
      ErrorMsg = "Cannot open or create " + ZipFileName;
   }

   // close any open zip input file after an error condition
   if (InputFILE) { fclose(InputFILE); InputFILE = NULL; }

   if (pErr) {
      // set error message whether or not there was an error
      *pErr = ErrorMsg;
   }
   if (ErrorMsg.size()) {
      // if error message is not null, it must be an error
      COL_ERR(ErrorMsg);

      // Remove the bad zip file.
      // Groan... Why does FILremove throw an exception?
      // We want to ignore it in any event.
      try { FILremoveWithThrow(ZipFileName); } catch (COLerror&) {}

      return MZIP_FAILURE;
   }
   return MZIP_SUCCESS;
}

extern "C" void fill_memory_filefunc(zlib_filefunc_def* pzlib_filefunc_def);

zipFile MZIPzipOpenMem(MZIPmemory_t* pMemory)
{
   COL_FUNCTION(MZIPunzipOpenMem);
   zlib_filefunc_def ffunc;
   fill_memory_filefunc(&ffunc);
   ffunc.opaque = pMemory;
   zipFile ZipFile = zipOpen2(":memory.zip:", APPEND_STATUS_CREATE, NULL, &ffunc);
   return ZipFile;
}

int MZIPzipFilesToMemory(const COLarray<COLstring>& FilesToZip, const COLstring& RootDir, COLstring* pMemoryZip, COLstring* pErr)
{
   COL_FUNCTION(MZIPzipFilesToMemory);
   int Error = 0;
   COLstring ErrorMsg; 
   FILE* InputFILE = NULL;
   MZIPmemory_t MemoryZip = { NULL, 0 };

   // Always create zip from scratch
   pMemoryZip->clear();

   if (zipFile ZipFile = MZIPzipOpenMem(&MemoryZip)) {
      const int BufferSize = 4096;
      char Buffer[BufferSize];

      // for each input file to zip...
      for (int i = 0; i < FilesToZip.size(); i++) {
         COLstring FileName = RootDir + FilesToZip[i];
         COL_VAR(FileName);

         int SizeRead = 0;
         const char* FilenameInZip = FileName.c_str();
         unsigned long CrcFile=0;

         // get the input file's timestamp
         zip_fileinfo ZipFileInfo;
         ::memset(&ZipFileInfo, 0, sizeof(ZipFileInfo));
         if (MZIPhasTrailingSlash(FilesToZip[i])) {
            // Name with trailing slash is a directory
            ZipFileInfo.external_fa = 48; // empty zip directory verified in Windows XP explorer
         }
         MZIPfiletime(FilenameInZip, &ZipFileInfo.tmz_date, &ZipFileInfo.dosDate);

         Error = zipOpenNewFileInZip3(ZipFile, FilesToZip[i].c_str(), &ZipFileInfo,
            NULL,0,NULL,0,NULL /* comment*/,
            Z_DEFLATED,
            5/*opt_compress_level*/,0,
            -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
            NULL/*password*/,CrcFile);

         if (Error != ZIP_OK) {
            ErrorMsg = COLstring("Cannot open ") + FilenameInZip + " in zipfile.";
            break;
         }

         // open a zip input file to be zipped
#ifdef _WIN32
         COLstring WinPath;
         InputFILE = _wfopen(FILmangleFileNameForWindows(FileName, &WinPath), L"rb");
#else
         InputFILE = fopen(FilenameInZip,"rb");
#endif
         if (InputFILE == NULL) {
            Error = ZIP_ERRNO;
            ErrorMsg = COLstring("Cannot open file ") + FilenameInZip + " for reading.";
            break;
         }

         // read and zip an input file in BufferSize chunks...
         do {
            Error = ZIP_OK;
            SizeRead = (int)fread(Buffer, 1, BufferSize, InputFILE);
            if (SizeRead < BufferSize) {
               if (feof(InputFILE) == 0) {
                  Error = ZIP_ERRNO;
                  ErrorMsg = COLstring("Cannot read file: ") + FilenameInZip;
                  break;
               }
            }

            if (SizeRead > 0) {
               Error = zipWriteInFileInZip(ZipFile, Buffer, SizeRead);
               if (Error < 0) {
                  ErrorMsg = COLstring("Cannot write ") + FilenameInZip + " to the zipfile.";
                  break;
               }
            }
         } while ((Error == ZIP_OK) && (SizeRead > 0));

         // close zip input file
         if (InputFILE) { fclose(InputFILE); InputFILE = NULL; }

         if (Error != ZIP_OK) {
            break;
         }

         Error = zipCloseFileInZip(ZipFile);
         if (Error != ZIP_OK) {
            ErrorMsg = COLstring("Cannot close ") + FilenameInZip + " in zipfile.";
            break;
         }
      }

      if (zipClose(ZipFile,NULL) != ZIP_OK) {
         if (ErrorMsg.is_null()) {
            // only override the error message if it was previously empty.
            ErrorMsg = "Cannot close memory zip file.";
         }
      }
   }
   else {
      ErrorMsg = "Cannot create memory zip file.";
   }

   // close any open zip input file after an error condition
   if (InputFILE) { fclose(InputFILE); InputFILE = NULL; }

   if (pErr) {
      // set error message whether or not there was an error
      *pErr = ErrorMsg;
   }
   if (ErrorMsg.size()) {
      // if error message is not null, it must be an error
      COL_ERR(ErrorMsg);
      free(MemoryZip.Memory);
      return MZIP_FAILURE;
   }

   pMemoryZip->set(MemoryZip.Memory, MemoryZip.Size);
   free(MemoryZip.Memory);
   return MZIP_SUCCESS;
}

static COLstring MZIPnormalizePath(COLstring Path)
{
   Path.replace('\\', '/');
   const int OriginalPathLength = Path.size();
   if (OriginalPathLength && Path[OriginalPathLength-1] == '/') {
      Path = Path.substr(0, OriginalPathLength-1);
   }
   return Path;
}

static int MZIPflattenVarTree(const COLvar& In, MZIPfileList* Out, const COLstring& Path="")
{
   COL_FUNCTION(MZIPflattenVarTree);
   int EntriesEmittedAtThisLevel = 0;
   if (In.isMap()) {
      COL_VAR2(Path, In);
      COLvar::COLvarMap::const_iterator it = In.map().cbegin(), E = In.map().cend();
      for (; it != E; ++it) {
         const COLstring& Key = it->first;
         const COLvar&    Val = it->second;

         // compute the full normalized path + file name for this entry
         COLstring FilePath = Path;
         if (FilePath.size()) {
            FilePath += "/";
         }
         FilePath += Key;
         FilePath = MZIPnormalizePath(FilePath);

         if (Val.isString()) {
            // This entry is either a file or an attribute.
            if (::strstr(Key.c_str(),"::")) {
               // A file attribute was detected - skip it.
               COL_TRC("skipping attribute: " << FilePath);
            }
            else {
               // Add this file or empty directory.
               ++EntriesEmittedAtThisLevel;
               MZIPfileEntry& Entry = Out->push_back();
               Entry->first = FilePath;
               // add this "file" name and payload
               Entry->second = Val; // "file" contents as binary string
               COL_TRC("Adding zip entry: '" << Entry->first
                  << "' with payload size " << Entry->second.size());
            }
         }
         else if (Val.isMap()) {
            // process this directory
            int rc = MZIPflattenVarTree(Val, Out, FilePath);
            if (rc == 0) {
               // Add this empty Map as a directory.
               // It could also be a Map with ignored child attributes.
               ++EntriesEmittedAtThisLevel;
               MZIPfileEntry& Entry = Out->push_back();
               Entry->first = FilePath + "/"; // dirs require trailing slash in zip
               COL_TRC("Adding empty dir: '" << Entry->first << "'");
            }
         }
         else {
            COL_WRN("ignoring entry '" << Key << "' with value type: " << Val.type());
         }
      }
   } else {
      COL_ERR("MZIPflattenVarTree: Input tree is not a map: " << In);
   }
   return EntriesEmittedAtThisLevel;
}

int MZIPzipVar(const COLvar& InMemoryFileTree, COLstring* pMemoryZip, COLstring* pErr)
{
   COL_FUNCTION(MZIPzipVar);
   int Error = 0;
   COLstring ErrorMsg; 
   MZIPmemory_t MemoryZip = { NULL, 0 };

   // create zip from scratch
   pMemoryZip->clear();

   if (zipFile ZipFile = MZIPzipOpenMem(&MemoryZip)) {
      MZIPfileList FileList;
      MZIPflattenVarTree(InMemoryFileTree, &FileList);

      tm_zip TimestampNow;
      MZIPpopulateCurrentTimestamp(&TimestampNow);

      // for each input file to zip...
      MZIPfileList::const_iterator it = FileList.cbegin(), E = FileList.cend();
      for (; it != E; ++it) {
         unsigned long CrcFile=0;
         const MZIPfileEntry& Entry = *it;
         const COLstring& Name = Entry.first;
         const COLstring& Data = Entry.second;
         COL_TRC("adding: '" << Name << "' with payload size " << Data.size());

         // timestamp and file permissions
         zip_fileinfo ZipFileInfo;
         ::memset(&ZipFileInfo, 0, sizeof(ZipFileInfo));
         if (MZIPhasTrailingSlash(Name)) {
            // Name with trailing slash is a directory
            ZipFileInfo.external_fa = 48; // empty zip directory verified in Windows XP explorer
         }
         ZipFileInfo.tmz_date = TimestampNow;

         Error = zipOpenNewFileInZip3(ZipFile, Name.c_str(), &ZipFileInfo,
            NULL, 0, NULL, 0, NULL /*comment*/,
            Z_DEFLATED,
            5/*opt_compress_level*/, 0,
            -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
            NULL/*password*/, CrcFile);

         if (Error != ZIP_OK) {
            ErrorMsg = "Cannot open " + Name + " in im-memory zip file.";
            break;
         }

         Error = zipWriteInFileInZip(ZipFile, Data.c_str(), Data.size());
         if (Error < 0) {
            ErrorMsg = "Cannot write '" + Name + "' to the in-memory zip file.";
            break;
         }

         if (Error != ZIP_OK) {
            break;
         }

         Error = zipCloseFileInZip(ZipFile);
         if (Error != ZIP_OK) {
            ErrorMsg = "Cannot close '" + Name + "' within in-memory zip file.";
            break;
         }
      }

      if (zipClose(ZipFile,NULL) != ZIP_OK) {
         if (ErrorMsg.is_null()) {
            // only override the error message if it was previously empty.
            ErrorMsg = "Cannot close in-memory zip file.";
         }
      }
   }
   else {
      ErrorMsg = "Cannot create in-memory zip file.";
   }

   if (pErr) {
      // set error message whether or not there was an error
      *pErr = ErrorMsg;
   }
   if (ErrorMsg.size()) {
      // if error message is not null, it must be an error
      COL_ERR(ErrorMsg);
      free(MemoryZip.Memory);
      return MZIP_FAILURE;
   }

   pMemoryZip->set(MemoryZip.Memory, MemoryZip.Size);
   free(MemoryZip.Memory);
   return MZIP_SUCCESS;
}

