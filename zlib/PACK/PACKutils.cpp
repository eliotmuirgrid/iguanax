//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: PACKutils.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   23/06/25 10:17 AM
//  ---------------------------------------------------------------------------
#include <PACK/PACKutils.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <WEBI/WEBIindexFile.h>
#include <SFI/SFImd5.h>
#include <FIL/FILpathUtils.h>
#include <WEBI/WEBIindex.h>
COL_LOG_MODULE;

COLstring PACKfindRoot(const COLarray<COLstring>& DirList) {
   COL_FUNCTION(PACKfindRoot);
   if(DirList.size() == 0) { return ""; }
   COLstring RootDir;
   int ShortestPathLength = DirList[0].size();
   for(const auto& Dir : DirList) { if(Dir.size() < ShortestPathLength) { ShortestPathLength = Dir.size(); } }
   for(int i = 0; i < ShortestPathLength; i++) {
      char CurrentChar = DirList[0][i];
      for(const auto& Dir : DirList) { if(Dir[i] != CurrentChar) { return RootDir; } } // diverged, return path
      RootDir.append(1, CurrentChar);
   }
   return RootDir;
}

void PACKconcatFiles(const COLstring& RootDir, COLmap<COLstring, COLstring>* pFileMap) {
   COL_FUNCTION(PACKconcatFiles);
   COLmap<COLstring, COLstring> ConcatMap;
   for(const auto& it : *pFileMap) {
      COLstring FileName = it->first;
      if(WEBIresolveFile(&FileName)) { COL_TRC("Resolved " << it->first << " to " << FileName); }
      FileName               = FILpathSepToPosix(FileName);
      const size_t RootFound = FileName.find(RootDir);
      if(RootFound != npos) { FileName = FileName.substr(RootFound + RootDir.size() - 1); }
      if(ConcatMap.count(FileName) == 0) {
         ConcatMap.add(FileName, it->second + "\n");
      } else {
         ConcatMap[FileName] = ConcatMap[FileName] + it->second + "\n"; // newline is necessary
      }
   }
   *pFileMap = ConcatMap;
}

static void PACKhashSource(COLmap<COLstring, COLstring>* pFileMap, COLstring* pJsFileHash, COLstring* pCssFileHash) {
   COL_FUNCTION(PACKhashSource);
   const COLstring WebJs  = "/web/app.js";
   const COLstring WebCss = "/web/app/app.css";
   *pJsFileHash           = "app/" + SFImd5((*pFileMap)[WebJs]) + ".js";
   *pCssFileHash          = "app/" + SFImd5((*pFileMap)[WebCss]) + ".css";
   pFileMap->add("/web/" + *pJsFileHash, (*pFileMap)[WebJs]);
   pFileMap->add("/web/" + *pCssFileHash, (*pFileMap)[WebCss]);
   pFileMap->erase(WebJs);
   pFileMap->erase(WebCss);
   COL_VAR2(*pJsFileHash, *pCssFileHash);
}

static void PACkappendHash(const COLstring& JsFileHash, const COLstring& IndexFile,
                           COLmap<COLstring, COLstring>* pFileMap) {
   COL_FUNCTION(PACkappendHash);
   pFileMap->add("hash", SFImd5((*pFileMap)[IndexFile]));
   const COLstring JsKey  = "/web/" + JsFileHash;
   const COLstring JsHash = "APP_HASH = '" + (*pFileMap)["hash"] + "';";
   (*pFileMap)[JsKey]     = (*pFileMap)[JsKey] + "\n" + JsHash;
}

void PACKgenerateIndex(const COLstring& IndexFile, COLmap<COLstring, COLstring>* pFileMap) {
   COL_FUNCTION(PACKgenerateIndex);
   if(pFileMap->count(IndexFile) == 0) { return; }
   COLstring IndexContent = (*pFileMap)[IndexFile];
   COLstring JsFileHash, CssFileHash;
   PACKhashSource(pFileMap, &JsFileHash, &CssFileHash);
   WEBIindexPack(JsFileHash, CssFileHash, &IndexContent);
   (*pFileMap)[IndexFile] = IndexContent;
   PACkappendHash(JsFileHash, IndexFile, pFileMap);
}

bool PACKisGzippedFile(const COLstring& File) {
   COL_FUNCTION(PACKisGzippedFile);
   return File != "hash" && File != "help_hash" && FILpathExtLast(File, false) != "vmd" &&
          File.find("deprecated.txt") == npos;
}

COLvar PACKmapToVar(const COLmap<COLstring, COLstring>& FileMap) {
   COL_FUNCTION(PACKmapToVar);
   COLvar MapVar;
   MapVar.setMapType();
   for(const auto& file : FileMap) { MapVar[file.first] = file.second; }
   return MapVar;
}