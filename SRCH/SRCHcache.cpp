//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SRCHcache.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   30/01/25 12:26 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLstack.h>
#include <COL/COLvar.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <SRCH/SRCHcache.h>
COL_LOG_MODULE;

const int SRCH_MAX_FILE_SIZE = 1048576; // 100mb // VISMAY look at this number

static void SRCHcacheInit(COLmap<COLstring, COLstring>* pCache, const COLstring& Path, COLvar* pResult) {
   COL_FUNCTION(SRCHcacheInit);
   if(!FILisDirectory(Path)) {
      COL_TRC("Given file to read " << Path);
      return SRCHreadIntoCache(pCache, Path);
   }
   COLstack<COLstring> DirStack;
   DirStack.push(Path);
   while(!DirStack.empty()) {
      COLstring CurrentPath = DirStack.top();
      DirStack.pop();
      FILdirEnumerator Enum(FILpathAppend(CurrentPath, FIL_ANYFILE));
      COLstring        File;
      while(Enum.getFile(File)) {
         COL_VAR3(File, Enum.isDirectory(), Enum.isHidden());
         if (Enum.isDirectory() && ".git" == File) {
            COL_TRC("Skipping Git folder: " << CurrentPath << File);
            continue;
         }
         if(Enum.isHidden()) {
            COL_TRC("Hidden file: " << File << " skipping.");
            continue;
         }
         const COLstring FullPath = FILpathAppend(CurrentPath, File);
         if(Enum.isDirectory()) {
            DirStack.push(FullPath);
         } else {
            if(FILfileSize(FullPath) >= SRCH_MAX_FILE_SIZE) {
               COL_TRC(FullPath << " greater than 100mb");
               (*pResult)["too_large"].push_back(File);
            } else {
               SRCHreadIntoCache(pCache, FullPath);
            }
         }
      }
   }
}

void SRCHreadIntoCache(COLmap<COLstring, COLstring>* pCache, const COLstring& Path) {
   COL_FUNCTION(SRCHreadIntoCache);
   if(FILisDirectory(Path)) {
      COL_TRC(Path << " is a directory");
      return;
   }
   try {
      COLstring Data;
      FILreadFile(Path, &Data);
      Data.replace("\r\n", "\n");
      COL_VAR2(Path, Data);
      pCache->add(Path, Data);
   } catch(const COLerror& E) { COL_TRC(E.description()); }
}

void SRCHcache(COLmap<COLstring, COLstring>* pCache, const COLstring& Path, COLvar* pResult) {
   COL_FUNCTION(SRCHcache);
   COL_VAR(Path);
   if(!FILfileExists(Path)) {
      COL_TRC(Path << " not found");
      return;
   }
   SRCHcacheInit(pCache, Path, pResult);
}