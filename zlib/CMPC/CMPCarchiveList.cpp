//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: CMPCarchiveList.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   10/04/25 1:28 PM
//  ---------------------------------------------------------------------------
#include <CMPC/CMPCarchiveList.h>
#include <COL/COLlog.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <SCK/SCKworker.h>
#include <SDB/SDBsize.h>
COL_LOG_MODULE;

#ifndef _WIN32
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <cstdint>
#endif

// posix FILdirenumerator skips hidden files which gives inaccurate size of archive dirs due to .git
#ifndef _WIN32
uint64_t CMPCgetDirectorySizePosix(const COLstring& Path) {
   uint64_t size = 0;
   DIR* dir = opendir(Path.c_str());

   if (!dir) {
      return 0; // Couldn't open directory
   }

   struct dirent* entry;
   struct stat statbuf;

   while ((entry = readdir(dir)) != nullptr) {
      // Skip "." and ".."
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
         continue;
      }

      COLstring fullPath = Path + "/" + entry->d_name;

      if (lstat(fullPath.c_str(), &statbuf) == 0) {
         if (S_ISDIR(statbuf.st_mode)) {
            // Recursively get size of subdirectory
            size += CMPCgetDirectorySizePosix(fullPath);
         } else {
            size += statbuf.st_size;
         }
      }
   }

   closedir(dir);
   return size;
}
#endif

uint64_t CMPCgetDirectorySize(const COLstring& Path) {
#ifdef _WIN32
   return SDBsize(Path);
#else
   return CMPCgetDirectorySizePosix(Path);
#endif
}

static void CMPCarchiveListCallback(COLvar Result, COLaddress Address) {
   COL_FUNCTION(CMPCarchiveListCallback);
   COL_VAR(Result.json(1));
   COLrespondSuccess(Address, Result);
}

static void CMPCarchiveListImpl(const COLvar& In, COLvar* pOut) {
   COL_FUNCTION(CMPCarchiveListImpl);
   const COLstring Archive     = DIRarchiveDir();
   const COLstring CompArchive = FILpathAppend(Archive, "comps/");
   (*pOut)["dir"]              = Archive;
   (*pOut)["components"].setArrayType();
   (*pOut)["size"] = "0";
   if(FILfileExists(CompArchive)) {
      COLstring        Component;
      FILdirEnumerator Enum(CompArchive + FIL_ANYFILE, true, true);
      while(Enum.getFile(Component)) { (*pOut)["components"].push_back(Component); }
      (*pOut)["size"] = COLuint64ToString(CMPCgetDirectorySize(Archive.c_str()));
   }
}

// /component/archive_list
void CMPCarchiveList(const COLwebRequest& Request, SCKloop* pLoop) {
   COL_FUNCTION(CMPCarchiveList);
   const COLvar Data;
   SCKinvokeWorker(pLoop, CMPCarchiveListImpl, Data, COLnewClosure1(CMPCarchiveListCallback, Request.Address));
}
