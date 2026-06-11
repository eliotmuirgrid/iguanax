//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSpackage
//
// Description:
//
// See header file.
//
// Author: John Qi
// Date:   Mon 24 Jun 2019 10:07:56 EDT
//
//---------------------------------------------------------------------------

#include "INSpackage.h"

#include <MZIP/MZIPzip.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILfile.h>

#include <COL/COLendian.h>
#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


static void INSiterateDir(const COLstring& Directory, COLarray<COLstring>* pList, const char* pPathSeperator){
   COL_FUNCTION(INSiterateDir);
   COL_VAR(Directory);

   FILdirEnumerator Enumerator(Directory + pPathSeperator + FIL_ANYFILE);
   COLstring FileName;
   COLarray<COLstring>& List = *pList;

   COLarray<COLstring> DirectoryList;
   while (Enumerator.getFile(FileName)){
      if (Enumerator.isDirectory()){
         COL_DBG("Adding directory: " << FileName);
         DirectoryList.push_back(FileName);
      } else {
         COL_DBG("FileName=" << FileName);
         List.push_back(Directory + pPathSeperator + FileName);
      }
   }
   for (COLarray<COLstring>::iterator i = DirectoryList.begin(); i != DirectoryList.end(); i++){
      INSiterateDir(Directory + pPathSeperator + *i, pList, pPathSeperator);
   }
}


void INSpack(const COLstring& Transport, const COLstring& Payload, bool isDirectory) {
   COLstring PayloadData;
   if ( ! isDirectory ) {
      FILreadFile(Payload, &PayloadData);
   }
   else {
      COLstring Err;
      COLarray<COLstring> FileList;
      INSiterateDir(Payload, &FileList, FIL_PATH_SEP_POSIX);

      int rc = MZIPzipFilesToMemory(FileList, FILworkingDir(), &PayloadData, &Err);
      if ( MZIP_FAILURE == rc ) {
         throw COLerror(Err);
      }
   }
   
   // NOTE: assuming the payload is less than 4GB in size, so a uint32 is used here
   COLuint32 BinaryCacheSize = PayloadData.size();
   COL_VAR(BinaryCacheSize);
   // FILcopyFile(SourceExe, DestinationExe, true);

   // append binary cache data and its size to the end of the binary
   FILfile BundledExe(Transport, FILfile::Write);
   BundledExe.setPosition(BundledExe.size());
   BundledExe.write(PayloadData.c_str(), BinaryCacheSize);
   COL_TRC("Appended data size: " << BinaryCacheSize << " (" << BinaryCacheSize / 1024 / 1024.0 << " MiB)");
   BinaryCacheSize = COLsystemToBigEndian(BinaryCacheSize);
   BundledExe.write(&BinaryCacheSize, 4);

   // append a magic number to indicate the binary contains
   COLuint32 CacheFlag = 1415926535;
   CacheFlag = COLsystemToBigEndian(CacheFlag);
   BundledExe.write(&CacheFlag, 4);
   BundledExe.close();
}

void INSunpack(const COLstring& Carrier, const COLstring& Output) {
   COL_FUNCTION(INSunpack); 
   FILfile File(Carrier, FILfile::Read);
   // seek to payload size and read
   // 4 byte magic number + 4 byte appended data size = 8 byte
   File.setPosition(File.size() - 8);
   COLuint32 PayloadSize = 0;
   int AmountRead = File.read(&PayloadSize, 4);
   PayloadSize = COLbigEndianToSystem(PayloadSize);
   COL_VAR(PayloadSize);

   // seek to payload 
   File.setPosition(File.size() - PayloadSize - 8);
   
   COLstring Buffer;
   Buffer.setCapacity(PayloadSize);
   File.read(Buffer.get_buffer(), PayloadSize);
   File.close();

   COL_TRC("Outputing file " << Output);
   FILfile BundledExe(Output, FILfile::Rewrite);
   BundledExe.write(Buffer.c_str(), PayloadSize);
   BundledExe.close();
}
