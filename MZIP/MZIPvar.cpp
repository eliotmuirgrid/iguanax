//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: MZIPunzip
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thu 13 Mar 2014 14:52:59 EDT
//---------------------------------------------------------------------------
#include <FIL/FILpathUtils.h>
#include <COL/COLsplit.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include "MZIPvar.h"
#include "MZIPunzip.h"

#include "MZIPzip.h"
#include "MZIPmemory.h"

#include <stdio.h>

#define MZIP_META_PREFIX "::"

static COLstring MZIPmodeAsString(const unz_file_info& Info) {
   char ModeBuffer[32];
   snprintf(ModeBuffer, 32, "%05lo", Info.external_fa >> 16);
   return ModeBuffer;
}

static COLstring MZIPpermissionsAsString(const unz_file_info& Info) {
   static const char* s_rwx[8] = { "---", "--x", "-w-", "-wx", "r--", "r-x", "rw-", "rwx" };
   const uLong fa = Info.external_fa;
   char buf[32];
   const bool isDir =  ((fa >> (16 + 12)) & 7) == 4;
   const unsigned usr = (fa >> (16 +  6)) & 7;
   const unsigned grp = (fa >> (16 +  3)) & 7;
   const unsigned oth = (fa >> (16 +  0)) & 7;
   snprintf(buf, 32, "%c%s%s%s", (isDir? 'd' : '-'), s_rwx[usr], s_rwx[grp], s_rwx[oth]);
   return buf;
}

static COLstring MZIPdatetimeAsString(const unz_file_info& Info) {
   const tm_unz& t = Info.tmu_date;
   char buf[32];
   snprintf(buf, 32, "%04u-%02u-%02u %02u:%02u:%02u",
      t.tm_year, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
   return buf;
}

static int MZIPextractFileContent(const COLstring& FileName, unzFile pUnzipFile,
      const unz_file_info& Info, const char* Password, COLvar* pContent) {
   COL_FUNCTION(MZIPextractFileContent);
   COL_VAR3(FileName, Info.uncompressed_size, Password);

   int OpenErr = unzOpenCurrentFilePassword(pUnzipFile,Password);
   if (OpenErr != UNZ_OK){
      COL_ERR("unzOpenCurrentFilePassword failed " << OpenErr);
      return OpenErr;
   }

   // read and store the uncompressed file into *pContent
   COLstring Buffer;
   Buffer.setCapacity(Info.uncompressed_size);
   Buffer.setSize(Info.uncompressed_size);
   int ReadSize = unzReadCurrentFile(pUnzipFile, Buffer.get_buffer(), Buffer.size());
   COL_VAR2(Buffer.size(), ReadSize);
   if (ReadSize == Buffer.size()){
      // file contents
      pContent->swap(&Buffer);
   } else {
      COL_ERR("Error uncompressing file: " << FileName);
      return UNZ_BADZIPFILE;
   }

   int CloseErr = unzCloseCurrentFile(pUnzipFile);
   if (CloseErr != UNZ_OK){
      // TODO - error handling!
      COL_ERR("unzCloseCurrentFile failed " << CloseErr);
      return UNZ_BADZIPFILE;
   }
   return UNZ_OK;
}

COLostream& operator<<(COLostream& Stream, const unz_file_info& FileInfo) {
   Stream << hex
          << "{version: 0x"             << FileInfo.version
          << ", version_needed: 0x"     << FileInfo.version_needed
          << ", flag: 0x"               << FileInfo.flag
          << ", compression_method: 0x" << FileInfo.compression_method
          << ", dosDate: 0x"            << FileInfo.dosDate
          << ", internal_fa: 0x"        << FileInfo.internal_fa
          << ", external_fa: 0x"        << FileInfo.external_fa
          << decimal
          << ", crc: "                << FileInfo.crc
          << ", compressed_size: "    << FileInfo.compressed_size
          << ", uncompressed_size: "  << FileInfo.uncompressed_size
          << ", size_filename: "      << FileInfo.size_filename
          << ", size_file_extra: "    << FileInfo.size_file_extra
          << ", size_file_comment: "  << FileInfo.size_file_comment
          << ", disk_num_start: "     << FileInfo.disk_num_start
          << ", tmu_date: "  << MZIPdatetimeAsString(FileInfo)
          << ", unix_mode: " << MZIPmodeAsString(FileInfo)
          << ", unix_perm: " << MZIPpermissionsAsString(FileInfo)
          << "}";
   return Stream;
}

static int MZIPunzipVarExtractCurrentFile(unzFile pUnzipFile, const char* Password, bool WantMetaData, COLvar* pResult, COLstring* pError) {
   COL_FUNCTION(MZIPunzipVarExtractCurrentFile);
   unz_file_info FileInfo;
   COLstring FileName;
   int ReturnCode = MZIPcurrentFileName(pUnzipFile, &FileInfo, &FileName);
   if (ReturnCode != UNZ_OK){
      return ReturnCode;
   }
   COL_VAR2(FileName, FileInfo);

   FileName.replace('\\', '/'); // normalize path with forward slashes

   COLarray<COLstring> PartList;
   COLsplit(&PartList, FileName, "/");

   // TODO - what if PartList.size() == 0 ...
   // TODO - what about empty directories?

   // create the subdirectory map nodes in each path
   COLstring Name;
   COLvar* pNode         = pResult;
   COLvar* pParentOfNode = pResult;
   bool IsDir = false;

   for (COLarray<COLstring>::iterator i = PartList.begin(); i != PartList.end(); ++i){
      COL_VAR(*i);
      if (i->size() == 0) {
         // directory name detected
         COL_TRC(FileName << " is a directory.");
         IsDir = true;
         break;
      }
      Name = *i;
      pParentOfNode = pNode;
      pNode = &(*pNode)[Name];
   }

   int Err = UNZ_OK;
   if (Name.size()) {
      if (IsDir) {
         // directory - set contents to empty map
         (*pParentOfNode)[Name].setMapType();
      } else {
         // get file contents
         Err = MZIPextractFileContent(FileName, pUnzipFile, FileInfo, Password, pNode);
      }
      if (WantMetaData) {
         // get timestamp
         (*pParentOfNode)[Name + MZIP_META_PREFIX "date"].set(MZIPdatetimeAsString(FileInfo));

         // get optional file/directory permissions.
         if (((FileInfo.version & 0xff00) >> 8) == 3 && FileInfo.external_fa) {
            // FilInfo.version high byte is OS. 3 is UNIX.
            //(*pParentOfNode)[Name + MZIP_META_PREFIX "perm"].set(MZIPpermissionsAsString(FileInfo));
            (*pParentOfNode)[Name + MZIP_META_PREFIX "mode"].set(MZIPmodeAsString(FileInfo));
         }
      }
   }

   return Err;
}

static int MZIPunzipVarExtract(unzFile pUnzipFile, const char* Password, bool WantMetaData, COLvar* pResult, COLstring* pError){
   COL_FUNCTION(MZIPunzipVarExtract);
   COL_VAR(Password);
   unz_global_info GlobalInfo;
   int GlobalInfoErr = unzGetGlobalInfo (pUnzipFile,&GlobalInfo);
   if (GlobalInfoErr!=UNZ_OK){
      *pError += "Error " + COLintToString(GlobalInfoErr) + " with zipfile in unzGetGlobalInfo()";
      return (MZIPresult)GlobalInfoErr;
   }
   int FirstErr = unzGoToFirstFile(pUnzipFile);
   if (FirstErr!=UNZ_OK){
      *pError += "Error " + COLintToString(FirstErr) + " with zipfile in unzGoToFirstFile()";
      return FirstErr;
   }
   COL_VAR(GlobalInfo.number_entry);
   for(int i=0; i < GlobalInfo.number_entry; i++){
      int ResultCode = MZIPunzipVarExtractCurrentFile(pUnzipFile, Password, WantMetaData, pResult, pError);
      if (ResultCode != UNZ_OK){
         return ResultCode;
      }

      if (i+1 < GlobalInfo.number_entry) {
         int NextErr = unzGoToNextFile(pUnzipFile);
         if (NextErr!=UNZ_OK){
            // TODO - is there an API that gives a human translation of the error message?
            *pError += "Error " + COLintToString(NextErr) + " with zipfile in unzGoToFirstFile()";
            return (MZIPresult)NextErr;
         }
      }
   }
   return MZIP_SUCCESS;
}

int MZIPunzipToVar(const void* pData, int Size, const char* Password, bool WantMetaData, COLvar* pResult, COLstring* pError){
   COL_FUNCTION(MZIPunzipToVar);
   pResult->reset();
   pResult->setMapType();
   unzFile pUnzipFile = MZIPunzipOpenMem(pData,Size);
   if(!pUnzipFile){
      COL_ERR("Corrupt zip data.");
      *pError = "ZIP file data is not valid.";
      return MZIP_CORRUPT;
   }
   int Result = MZIPunzipVarExtract(pUnzipFile, Password, WantMetaData, pResult, pError);
   MZIPunzipClose(pUnzipFile);
   return Result;
}
