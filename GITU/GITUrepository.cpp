// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUrepository
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Thursday 23 February 2023 - 03:31PM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <COL/COLmap.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>
#include <GIT/GITbranch.h>
#include <GITU/GITUconfig.h>
#include <GITU/GITUlist.h>
#include <GITU/GITUrepository.h>
#include <GITU/GITUurl.h>
#include <INI/INIdata.h>
COL_LOG_MODULE;

static bool GITUrepositoryUrlValid(const COLstring& Url) {
   COL_FUNCTION(GITUrepositoryUrlValid);
   if(Url.is_null()) { return false; }
   GITUurl ParsedUrl;
   COLstring Error;
   const bool Valid = GITUparseUri(Url, &ParsedUrl, &Error);
   COL_VAR3(Valid, ParsedUrl.Host, ParsedUrl.Repository);
   if(!Valid) {
      COL_TRC("Ignoring broken repository url: " << Url);
      COL_TRC(Error);
   }
   return Valid;
}

// This is a cleaner API to use
COLstring GITUrepositoryRemoteUrl(const COLstring& Dir){
   COL_FUNCTION(GITUrepositoryRemoteUrl);
   INIdata Config;
   COLstring Error;
   if(!GITUconfigLoad(Dir, &Config, &Error)){
      COL_TRC(Error);
      return "";
   };
   const COLstring Url = GITUconfigRemoteOriginUrl(Config);
   return GITUrepositoryUrlValid(Url) ? Url : "";
}

bool GITUrepository(const COLstring& Dir, const COLstring& Tag, COLstring* pContent){
   COL_FUNCTION(GITUrepository);
   COL_VAR(Dir);
   INIdata Config;
   *pContent = "";
   COLstring Error;
   if(!GITUconfigLoad(Dir, &Config, &Error)){
      COL_TRC("GITUrepository config load failed: " << Error);
      return false;
   };
   COLstring RawUrl = INIdataKeyGet(Config, "remote", Tag, "url");
   RawUrl.stripAll('\n');
   if(!GITUrepositoryUrlValid(RawUrl)) {
      *pContent = "";
      return false;
   }
   *pContent = RawUrl;
   return !pContent->is_null();
}

// TODO Come back to UPSTREAM Conversation
bool GITUgetUpstreamList(const COLstring& Folder, GITUlist* pList, COLstring* pError){
   COL_FUNCTION(GITUgetUpstreamList);
   FILdirEnumerator DirEnumerator(Folder + "*", true, true);
   COLstring Dir;
   bool UpstreamExists = false;
   while (DirEnumerator.getFile(Dir)){
      COL_VAR(Dir);
      COLstring ComponentPath = FILpathAppend(Folder, Dir);
      ComponentPath = FILpathAppend(ComponentPath, "dev");
      COLstring UpstreamRepo, HttpUpstreamRepo;
      if(GITUrepository(ComponentPath, "origin", &UpstreamRepo)){
         COL_VAR(UpstreamRepo);
         GITUhttpxUrl(UpstreamRepo, &HttpUpstreamRepo);
         COL_VAR(HttpUpstreamRepo);
         COLstring Branch = GITbranchCurrentDev(ComponentPath);
         COL_VAR(Branch);
         // Component information is not important, we are just interested in the upstream repository so that we can cache it for git operations (commits, diff's etc)
         // TODO IX-4309 query branch for new component branch implementation
         pList->List.push_back(GITUlistComponent("", "", HttpUpstreamRepo, "", Branch));
         UpstreamExists = true;
      }
   }
   return UpstreamExists;
}
