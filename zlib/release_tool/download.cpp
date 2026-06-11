// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: checkHash.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   Thursday August 17, 2023
// ---------------------------------------------------------------------------
#include "download.h"
#include "constants.h"
#include "utils.h"

#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>

#include <WEB/WEBrequest.h>
#include <WEB/WEBresponse.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static COLstring resolveBinDir(const COLstring& Location, const COLstring& Url) {
   COL_FUNCTION(resolveBinDir);
   COL_VAR2(Location, Url);
   if(Url == MAC_URL)     { return FILpathAppend(Location, "macosx/"); }
   if(Url == LIN_URL)     { return FILpathAppend(Location, "linux/"); }
   if(Url == WIN_URL)     { return FILpathAppend(Location, "windows/"); }
   if(Url == LIN_ARM_URL) { return FILpathAppend(Location, "linux/"); }
   COLabortWithMessage("No map of binary directory set for URL = " + Url);
   return "";
}

static COLstring addArm(const COLstring& File){
   COL_FUNCTION(addArm);
   COLstring Modified = File;
   COLstring Extension = FILpathExtLast(Modified, true); 
   Modified = Modified.substr(0, Modified.size()-Extension.size()) + "-Arm" + Extension;
   COL_VAR2(File, Modified);
   return Modified;
}

bool downloadHash(const char* Url, const COLstring& Hash, const COLstring& File, const COLstring& Location) {
   COL_FUNCTION(downloadHash);
   COL_VAR4(Url, Hash, File, Location);
   WEBrequest HashExistsRequest;
   HashExistsRequest.Url = Url + Hash + "/" + File;
   COL_TRC("Making request to: " << HashExistsRequest.Url);
   WEBresponse RepoResponse;
   WEBrequestBlockingGet(HashExistsRequest, &RepoResponse);
   if(RepoResponse.ReturnCode != 200) {
      reportMessage("Error: " + File + " not found", false);
      return reportMessage("Could not find hash: " + Hash + " on " + Url + Hash + "\nCode=" + COLint32ToString(RepoResponse.ReturnCode), false);
   }
   COLstring BinDir = resolveBinDir(Location, Url);
   COL_VAR(BinDir);
   if(!FILfileExists(BinDir)) {
      COL_TRC("Making " << BinDir);
      FILmakeFullDir(BinDir, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
   }
   COLstring FinalFile = File;
   if (strcmp(Url,LIN_ARM_URL) == 0){
      COL_TRC("Add -Arm since it's an ARM build.");
      FinalFile = addArm(FinalFile);
   }
   COL_TRC("Writing binary to " << BinDir + FinalFile);
   FILwriteFile(BinDir + FinalFile, RepoResponse.Body);
   return true;
}

static void filterOutTxt(COLvar* pData){
   COL_FUNCTION(filterOutTxt);
   COLvar& Data = *pData;
   for (int i = Data.size() - 1; i >= 0; i--){
      if (FILpathExtLast(Data[i].asString(), true) == ".txt") {
         COL_TRC("Removing " << Data[i] << " as " << i);
         Data.remove(i);
      }
   }
}

static bool listHashFiles(const char* Url, const COLstring& Hash, COLvar* pOut){
   COL_FUNCTION(listHashFiles);
   WEBrequest FileList;
   FileList.Url = Url + Hash + "/?json=true";
   WEBresponse Response;
   WEBrequestBlockingGet(FileList, &Response);
   if(Response.ReturnCode != 200) { return reportMessage("Could not find hash: " + Hash + " on " + Url + Hash, false); }
   COL_VAR(Response.Body);
   if (!pOut->parse(Response.Body)){ return reportMessage("Failed to parse json from " + FileList.Url, false); }
   COL_VAR(pOut->json(1));
   return true;
}

bool requestOs(const char* OSurl, const COLstring& Hash, COLvar* pFileList) {
   COL_FUNCTION(requestOs);
   if (!listHashFiles(OSurl, Hash, pFileList)) { return false; }
   filterOutTxt(pFileList);
   if (pFileList->size() == 0) {
      reportMessage("No files found for commit: " + Hash + " on " + OSurl, false);
      return false;
   }
//   if (!downloadFiles(OSurl, Hash, Files, Location)) { return false; }
//   COLcout << "Got all required files from " << OSurl << newline;
   return true;
}