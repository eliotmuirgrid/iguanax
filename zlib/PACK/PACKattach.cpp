//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: PACKattach.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   6/16/23 10:52 AM
//  ---------------------------------------------------------------------------
#include <PACK/PACKattach.h>

#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <FIL/FILfile.h>

#include <GZIP/GZIPutils.h>
#include <SFI/SFImd5.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


// returns the buffer offset where magic is found
bool PACKfind(const COLstring& Buffer, const COLstring& SubStr, int* pPosition) {
   COL_FUNCTION(PACKfind);
   COL_VAR(Buffer.size());
   const char* Start = Buffer.data();
   const char* Position = Start + Buffer.size() - SubStr.size();
   while (Position >= Start) {
      if (0 == memcmp(Position, SubStr.data(), SubStr.size())) {
         *pPosition = Position - Start;
         COL_VAR(*pPosition);
         return true;
      }
      Position--;
   }
   return false;
}

static bool PACKgetMagicPos(FILfile& File, COLuint32& Magic, COLstring* pError, int BufSize = 1048576) {
   COL_FUNCTION(PACKgetMagicPos);
   COLstring Buffer;
   Buffer.setCapacity(BufSize);
   COLint64 Position = File.size() - BufSize;
   if (Position < 0) { Position = 0; }
   File.setPosition(Position);
   File.read(&Buffer);     // throws on Error
   int Offset = 0;
   if (!PACKfind(Buffer, PACK_MAGIC_STRING, &Offset)) {
      *pError = "The magic string not found in the last " + COLintToString(BufSize) + " bytes of the file";
      return false;
   }
   Magic = Position + Offset;
   COL_TRC("Magic string found at position " << Magic);
   COL_DUMP("Magic = ", Buffer.c_str() + Offset, PACK_MAGIC_STRING.size());
   return true;
}

static bool PACKgetMD5hash(FILfile& File, COLuint32& Position, COLstring* pHash, COLstring* pError) {
   COL_FUNCTION(PACKgetMD5hash);
   pHash->clear();
   pHash->setCapacity(32);
   File.setPosition(Position - 32);
   if (File.read(pHash) != 32) {
      *pError = "Failed to extract MD5 hash";
      return false;
   }
   Position -= 32;
   COL_VAR(*pHash);
   return true;
}

static bool PACKgetDataSize(FILfile& File, COLuint32& Position, COLuint32* pSize, COLstring* pError) {
   COL_FUNCTION(PACKgetDataSize);
   File.setPosition(Position - sizeof(COLuint32));
   if (File.read(pSize, sizeof(COLuint32)) != sizeof(COLuint32)) {
      *pError = "Failed to extract data size";
      return false;
   }
   Position -= sizeof(COLuint32);
   COL_VAR(*pSize);
   return true;
}

static bool PACKgetData(FILfile& File, COLuint32& Position, COLstring* pData, COLuint32 DataSize, COLstring* pError) {
   COL_FUNCTION(PACKgetData);
   pData->clear();
   pData->setCapacity(DataSize);
   File.setPosition(Position - DataSize);
   if (File.read(pData) != DataSize) {
      *pError = "Failed to extract data";
      return false;
   }
   Position -= DataSize;
   COL_VAR(*pData);
   return true;
}

static bool PACKvalidateData(COLstring Data, const COLstring& MD5, COLstring* pError) {
   COL_FUNCTION(PACKvalidateData);
   COL_VAR(MD5);
   COLuint32 Size = Data.size();
   Data.write(&Size, sizeof(COLuint32));
   COLstring Hash = SFImd5(Data);
   COL_VAR(Hash);
   if (Hash != MD5) {
      *pError = "Unexpected MD5 hash. File corrupted.";
      return false;
   }
   return true;
}

static bool PACKconvertData(const COLstring& Data, COLvar* pVar, COLstring* pError) {
   COL_FUNCTION(PACKconvertData);
   try {
      int Consumed = 0;
      COLvar::ParseState Result = pVar->parseBinary(Data.data(), Data.size(), &Consumed);
      COLASSERT(Consumed == Data.size());
      COLASSERT(Result   == COLvar::kSUCCESS);
      return true;
   } catch(const COLerror& Error) {
      *pError = "Failed to parse attached data.";
   }
   return false;
}

// NOTE: only supports Binaries less than 4GB
// throws file io error if failed to open or reader Binary
bool PACKextractFromExe(const COLstring& Binary, COLvar* pData, COLstring* pError) {
   COL_FUNCTION(PACKextractFromExe);
   bool      Success = false;
   COLuint32 Magic = 0, DataSize = 0;  // limits file size to 4 GiB
   COLstring Hash, Data;
   FILfile   File(Binary, FILfile::Read);
   do {
      if (!PACKgetMagicPos (File, Magic, pError)) { break; }
      if (!PACKgetMD5hash  (File, Magic, &Hash, pError)) { break; }
      if (!PACKgetDataSize (File, Magic, &DataSize, pError)) { break; }
      if (!PACKgetData     (File, Magic, &Data, DataSize, pError)) { break; }
      if (!PACKvalidateData(Data, Hash,  pError)) { break; }
      if (!PACKconvertData (Data, pData, pError)) { break; }
      Success = true;
   } while (false);
   COL_VAR(*pError);
   return Success;
}

void PACKattachToExe(const COLvar& Result, COLstring IguanaBinary) {
   COL_FUNCTION(PACKattachToExe);
   COLstring Attachment;
   Result.toBinary(&Attachment);

   COLuint32 Size = Attachment.size();
   COL_VAR(Size);
   Attachment.write(&Size, sizeof(COLuint32));

   COLstring MD5 = SFImd5(Attachment);
   COL_VAR(MD5);
   Attachment.append(MD5);

   Attachment.append(PACK_MAGIC_STRING);

   COLstring NewBinary = FILpathName(IguanaBinary);
   FILcopyFile(IguanaBinary, NewBinary);
   FILappendFile(NewBinary, Attachment);
#ifndef _WIN32
   FILsetPermission(NewBinary, FIL_USER_RWX);
#endif

   COLstring Raw, IndexHtml;
   Raw = Result["/web/index.html"];
   GZIPuncompress(Raw, &IndexHtml);
   COL_VAR(IndexHtml);
}