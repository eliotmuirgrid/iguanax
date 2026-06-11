//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: PACKwebPack.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   6/5/23 1:56 PM
//  ---------------------------------------------------------------------------
#include <COL/COLclosure6.h>
#include <COL/COLlist.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GZIP/GZIPutils.h>
#include <PACK/PACKhelp.h>
#include <PACK/PACKutils.h>
#include <PACK/PACKwebPack.h>
#include <SCK/SCKloop.h>

#include <memory>
COL_LOG_MODULE;

struct PACKtracker {
   explicit PACKtracker(const int Pending, const COLstring& Root, const COLstring& Index)
       : m_Pending(Pending), m_Root(Root), m_Index(Index) {};
   int                          m_Pending;
   COLstring                    m_Root;
   COLstring                    m_Index;
   COLmap<COLstring, COLstring> m_FileMap;
};

struct PACKfileInfo {
   explicit PACKfileInfo(const COLstring& File, const bool IsDir) : m_FileName(File), m_IsDir(IsDir) {};
   COLstring          m_FileName;
   COLstring          m_Content;
   bool               m_IsDir;
   COLlist<COLstring> m_FileList;
};

static void PACKprocessFile(COLstring File, std::shared_ptr<PACKtracker> pTracker, SCKloop* pLoop,
                            COLclosure1<const COLvar&>* pOnComplete);

static void PACKtracePreGzip(const COLmap<COLstring, COLstring>& FileMap) {
   COL_FUNCTION(PACKtracePreGzip);
   for(const auto& file : FileMap) { COL_VAR2(file.first, file.second); }
}

// main thread
static void PACKgzipComplete(const COLstring File, std::shared_ptr<COLstring> pContent, std::shared_ptr<PACKtracker> pTracker,
                             SCKloop* pLoop, COLclosure1<const COLvar&>* pOnComplete) {
   COL_FUNCTION(PACKgzipComplete);
   pTracker->m_Pending--;
   if(!pContent->is_null()) { pTracker->m_FileMap[File] = *pContent; }
   if(pTracker->m_Pending == 0) {
      pOnComplete->runAndDelete(PACKmapToVar(pTracker->m_FileMap));
      pLoop->shutdown();
   }
}

// worker thread
static void PACKgzipContent(const COLstring File, std::shared_ptr<PACKtracker> pTracker, SCKloop* pLoop,
                            COLclosure1<const COLvar&>* pOnComplete) {
   COL_FUNCTION(PACKgzipContent);
   auto pContent = std::make_shared<COLstring>();
   if(PACKisGzippedFile(File)) { GZIPcompress(pTracker->m_FileMap[File], pContent.get(), 9); }
   pLoop->post(COLnewClosure0(&PACKgzipComplete, File, pContent, pTracker, pLoop, pOnComplete));
}

static void PACKgzipFiles(std::shared_ptr<PACKtracker> pTracker, SCKloop* pLoop,
                          COLclosure1<const COLvar&>* pOnComplete) {
   COL_FUNCTION(PACKgzipFiles);
   pTracker->m_Pending = pTracker->m_FileMap.size();  // set count to size of map for gzip iteration
   for(const auto it : pTracker->m_FileMap) {
      pLoop->threadPool()->scheduleTask(COLnewClosure0(&PACKgzipContent, it.first, pTracker, pLoop, pOnComplete));
   }
}

static void PACKprocessFileMap(std::shared_ptr<PACKtracker> pTracker, SCKloop* pLoop,
                               COLclosure1<const COLvar&>* pOnComplete) {
   COL_FUNCTION(PACKprocessFileMap);
   PACKconcatFiles(pTracker->m_Root, &pTracker->m_FileMap);
   PACKgenerateIndex(pTracker->m_Index, &pTracker->m_FileMap);
   PACKaddHelpHash(&pTracker->m_FileMap);
   PACKtracePreGzip(pTracker->m_FileMap);
   PACKgzipFiles(pTracker, pLoop, pOnComplete);
}

// main
static void PACKaddToFileVarMain(std::shared_ptr<PACKfileInfo> pFileInfo, std::shared_ptr<PACKtracker> pTracker,
                                 SCKloop* pLoop, COLclosure1<const COLvar&>* pOnComplete) {
   COL_FUNCTION(PACKaddToFileVarMain);
   COL_VAR3(pFileInfo->m_FileName, pFileInfo->m_IsDir, pFileInfo->m_FileList.size());
   pTracker->m_Pending--;
   if(pFileInfo->m_IsDir) {
      pTracker->m_Pending += static_cast<int>(pFileInfo->m_FileList.size());
      for(const auto& file : pFileInfo->m_FileList) {
         pLoop->threadPool()->scheduleTask(COLnewClosure0(&PACKprocessFile, file, pTracker, pLoop, pOnComplete));
      }
   } else {
      pTracker->m_FileMap.add(PACKreplaceHelpPrefix(pTracker->m_Root, pFileInfo->m_FileName), pFileInfo->m_Content);
   }
   if(pTracker->m_Pending == 0) PACKprocessFileMap(pTracker, pLoop, pOnComplete);
}

// worker
static void PACKprocessFile(const COLstring File, std::shared_ptr<PACKtracker> pTracker, SCKloop* pLoop,
                            COLclosure1<const COLvar&>* pOnComplete) {
   COL_FUNCTION(PACKprocessFile);
   auto pFileInfo = std::make_shared<PACKfileInfo>(File, FILisDirectory(File));
   if(pFileInfo->m_IsDir) {
      FILdirEnumerator Enumerator(FILpathAppend(File, FIL_ANYFILE));
      COLstring FileName;
      while(Enumerator.getFile(FileName)) { pFileInfo->m_FileList.add(FILpathAppend(File, FileName)); }
   } else {
      FILreadFile(File,&pFileInfo->m_Content);
   }
   pLoop->post(COLnewClosure0(&PACKaddToFileVarMain, pFileInfo, pTracker, pLoop, pOnComplete));
}

void PACKwebPack(const COLarray<COLstring>& DirList, COLclosure1<const COLvar&>* pOnComplete) {
   COL_FUNCTION(PACKwebPack);
   COLthreadPool Pool(1, 100);
   Pool.start();
   SCKloop Loop(&Pool);
   auto    pTracker = std::make_shared<PACKtracker>(DirList.size(), PACKfindRoot(DirList), "/web/index.html");
   for(const auto& Dir : DirList) {
      if(FILfileExists(Dir)) {
         Loop.threadPool()->scheduleTask(COLnewClosure0(&PACKprocessFile, Dir, pTracker, &Loop, pOnComplete));
      } else {
         COL_ERR(Dir + " not found");
         pTracker->m_Pending--;
      }
   }
   Loop.start();
}
