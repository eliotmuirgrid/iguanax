//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GSVlink.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   05/07/24 12:19 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <GSV/GSVlink.h>
#include <GSV/GSVmap.h>
COL_LOG_MODULE;

static bool GSVitemIsDefaultHttpPort(const GSVitem& Item) {
   COL_FUNCTION(GSVitemIsDefaultHttpPort);
   if(Item.m_Secure) {
      return (Item.m_HttpPort == 0 || Item.m_HttpPort == 443);
   } else {
      return (Item.m_HttpPort == 0 || Item.m_HttpPort == 80);
   }
}

COLstring GSVconstructBranchLink(const GITUurl& Url, const COLstring& WebResult, const COLstring& Branch){
   COL_FUNCTION(GSVconstructBranchLink);
   GSVitem Item = GSVmapItem(Url.Host);
   bool IsBranchNull = Branch.is_null();
   switch (Item.m_Type){
      case GIT_github_cloud:
      case GIT_github_server:
         return IsBranchNull ? WebResult : WebResult + "/tree/" + Branch;
         break;
      case GIT_azuredev_cloud:
      case GIT_azuredev_server:
         return IsBranchNull ? WebResult : WebResult + "/?version=GB" + Branch;
         break;
      case GIT_gitlab_cloud:
      case GIT_gitlab_server:
         return IsBranchNull ? WebResult : WebResult + "/-/tree/" + Branch;
         break;
      default:
         return IsBranchNull ? WebResult : WebResult + "/src/" + Branch;
         break;
   }
}

// link to the website
bool GSVconstructServerLink(const COLstring& ServerUrl, COLstring* pServerLink) {
   COL_FUNCTION(GSVconstructServerLink);
   COLstring& ServerLink = *pServerLink;
   if(ServerUrl.is_null()) { return false; }
   GSVitem   Item   = GSVmapItem(ServerUrl);
   COLstring Scheme = Item.m_Secure ? ServerLink.append("https://") : ServerLink.append("http://");
   ServerLink.append(ServerUrl);
   COLstring Port;
   if(!GSVitemIsDefaultHttpPort(Item)) {
      Port = COLintToString(Item.m_HttpPort);
      ServerLink.append(":" + Port);
   }
   return true;
}

bool GSVconstructLink(const COLstring& Repo, GSVlink* pLink, const COLstring& Branch) {
   COL_FUNCTION(GSVconstructLink);
   COL_VAR(Repo);
   COLstring WebResult, HttpxResult, SshResult;
   if(Repo.is_null()) { return false; }
   if(!GITUwebUrl(Repo, &WebResult))     { return false; }
   if(!GITUhttpxUrl(Repo, &HttpxResult)) { return false; }
   if(!GITUsshUrl(Repo, &SshResult))     { return false; }
   GITUurl Url;
   COLstring Error;
    if(!GITUparseUri(Repo, &Url, &Error)) {
      COL_VAR("Could not parse provided git url." + Error);
      return false;
   }
   pLink->m_Web        = Branch.is_null() ? WebResult : WebResult + "/branches";
   pLink->m_Web_Branch = GSVconstructBranchLink(Url, WebResult, Branch);
   pLink->m_Ssh   = SshResult;
   pLink->m_Httpx = HttpxResult;
   COL_VAR3(pLink->m_Web, pLink->m_Ssh, pLink->m_Httpx);
   return true;
}

COLvar GSVlinkToVar(const GSVlink& Link) {
   COL_FUNCTION(GSVlinkToVar);
   COLvar Var;
   Var["web"] = Link.m_Web;
   Var["web_branch"] = Link.m_Web_Branch;
   Var["url"] = Link.m_Httpx;
   Var["ssh"] = Link.m_Ssh;
   return Var;
}
