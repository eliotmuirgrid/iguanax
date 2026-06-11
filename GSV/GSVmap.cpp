//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GSVmap.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   30/07/24 11:38 AM
//  ---------------------------------------------------------------------------
#include <COL/COLhashmap.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <GSV/GSVmap.h>
COL_LOG_MODULE;

static COLhashmap<COLstring, GSVitem> GSVmap;

static void GSVcreateDefault() {
   COL_FUNCTION(GSVcreateDefault);
   GSVmap.add("github.com", GSVitem(true, GIT_github_cloud, 0, 0));
   GSVmap.add("gitlab.com", GSVitem(true, GIT_gitlab_cloud, 0, 0));
   GSVmap.add("bitbucket.org", GSVitem(true, GIT_bitbucket_cloud, 0, 0));
   GSVmap.add("dev.azure.com", GSVitem(true, GIT_azuredev_cloud, 0, 0));
}

void GSVmapInit() {
   COL_FUNCTION(GSVmapInit);
   try {
      const COLstring File = DIRfileConfigServers();
      if(!FILfileExists(File)) { return GSVcreateDefault(); }
      COLstring RawData;
      FILreadFile(File, &RawData);
      COLvar MapData;
      MapData.parse(RawData);
      GSVmapSet(MapData);
   } catch(const COLerror& Error) { COL_ERR(Error.description()); }
}

void GSVmapAdd(const COLstring& Host, const GITUserver& Server, bool Secure, int HttpPort, int SshPort) {
   COL_FUNCTION(GSVmapAdd);
   COL_VAR3(Host, GSVserviceName(Server), Secure);
   COL_VAR2(HttpPort, SshPort);
   GSVmap.add(Host, GSVitem{Secure, Server, HttpPort, SshPort});
}

bool GSVmapExists(const COLstring& Host) {
   COL_FUNCTION(GSVmapExists);
   COL_VAR(Host);
   return GSVmap.count(Host) != 0;
}

bool GSVoldFormat(const COLvar& Input) {
   COL_FUNCTION(GSVoldFormat);
   return !Input.exists("http_port") && !Input.exists("ssh_port");
}

COLstring GSVoldDefaultType(const COLstring& Type) {
   COL_FUNCTION(GSVoldDefaultType);
   if(Type == "Azure Devops") return GSVserviceName(GIT_azuredev_cloud);
   if(Type == "Bitbucket") return GSVserviceName(GIT_bitbucket_cloud);
   if(Type == "GitHub") return GSVserviceName(GIT_github_cloud);
   if(Type == "GitLab") return GSVserviceName(GIT_gitlab_cloud);
   return GSVserviceName(GIT_bitbucket_cloud);
}

COLstring GSVreadType(const COLvar& Input, const COLstring& Type) {
   COL_FUNCTION(GSVoldDefaultType);
   if(GSVoldFormat(Input)) return GSVoldDefaultType(Type);
   return Type;
}

bool GSVmapSet(const COLvar& Data) {
   COL_FUNCTION(GSVmapSet);
   if(!Data.isMap()) {
      COL_ERR("Invalid server data");
      return false;
   }
   GSVmap.clear();
   for(const auto& it : Data.map()) {
      if(!it.second.isMap()) {
         COL_ERR("Error loading item from server data -- " << it.second);
         continue;
      }
      const COLstring& Host     = it.first;
      const COLstring& InType   = it.second.exists("type") ? it.second["type"] : "";
      const bool       Secure   = it.second.exists("secure") && it.second["secure"].asBool();
      int              HttpPort = it.second.exists("http_port") ? it.second["http_port"].asInt() : 0;
      int              SshPort  = it.second.exists("ssh_port") ? it.second["ssh_port"].asInt() : 0;
      if(HttpPort < 0) HttpPort = 0;
      if(SshPort < 0) SshPort = 0;
      const COLstring& Type = GSVreadType(it.second, InType);
      COL_VAR3(Host, Secure, Type);
      GSVmapAdd(Host, GSVserviceEnumVal(Type), Secure, HttpPort, SshPort);
   }
   return true;
}

bool GSVmapSave(COLstring* pErr) {
   COL_FUNCTION(GSVmapSave);
   try {
      const COLstring File = DIRfileConfigServers();
      FILwriteFile(File, GSVmaptoVar().json(1));
      return true;
   } catch(const COLerror& Error) {
      *pErr = Error.description();
      return false;
   }
}

GSVitem GSVmapItem(const COLstring& Host) {
   COL_FUNCTION(GSVmapItem);
   if(GSVmap.count(Host) == 0) { return GSVitem(); }
   return GSVmap[Host];
}

COLvar GSVmaptoVar() {
   COL_FUNCTION(GSVmaptoVar);
   COLvar Var;
   for(const auto& it : GSVmap) {
      COLvar Item;
      Item["secure"]    = it.second.m_Secure;
      Item["type"]      = GSVserviceName(it.second.m_Type);
      Item["http_port"] = it.second.m_HttpPort;
      Item["ssh_port"]  = it.second.m_SshPort;
      Var[it.first]     = Item;
   }
   return Var;
}

COLvar GSVitemToVar(const GSVitem& Item) {
   COL_FUNCTION(GSVitemToVar);
   COLvar Var;
   Var["secure"]    = Item.m_Secure;
   Var["service"]   = GSVserviceName(Item.m_Type);
   Var["http_port"] = Item.m_HttpPort;
   Var["ssh_port"]  = Item.m_SshPort;
   return Var;
}

COLstring GSVserviceName(const GITUserver& Host) {
   switch(Host) {
      case GIT_azuredev_cloud: return "Cloud - Azure Devops";
      case GIT_azuredev_server: return "Server - Azure Devops";
      case GIT_bitbucket_cloud: return "Cloud - Bitbucket";
      case GIT_bitbucket_server: return "Server - Bitbucket";
      case GIT_github_cloud: return "Cloud - GitHub";
      case GIT_github_server: return "Server - GitHub";
      case GIT_gitlab_cloud: return "Cloud - GitLab";
      case GIT_gitlab_server: return "Server - GitLab";
      default: return "Cloud - Bitbucket";
   }
}

GITUserver GSVserviceEnumVal(const COLstring& Host) {
   if(Host == "Cloud - Azure Devops") return GIT_azuredev_cloud;
   if(Host == "Server - Azure Devops") return GIT_azuredev_server;
   if(Host == "Cloud - Bitbucket") return GIT_bitbucket_cloud;
   if(Host == "Server - Bitbucket") return GIT_bitbucket_server;
   if(Host == "Cloud - GitHub") return GIT_github_cloud;
   if(Host == "Server - GitHub") return GIT_github_server;
   if(Host == "Cloud - GitLab") return GIT_gitlab_cloud;
   if(Host == "Server - GitLab") return GIT_gitlab_server;
   return GIT_invalid;
}
COLstring GSVdefaultHost(const GITUserver& Host) {
   COL_FUNCTION(GSVdefaultHost);
   switch(Host) {
      case GIT_bitbucket_cloud: return "bitbucket.org";
      case GIT_github_cloud: return "github.com";
      case GIT_gitlab_cloud: return "gitlab.com";
      case GIT_azuredev_cloud: return "dev.azure.com";
      default: return "bitbucket.org";
   }
}

bool GSVserviceIsCloud(const GITUserver& Host) {
   COL_FUNCTION(GSVserviceIsCloud);
   switch(Host) {
      case GIT_bitbucket_cloud:
      case GIT_github_cloud:
      case GIT_gitlab_cloud:
      case GIT_azuredev_cloud: return true;
      default: return false;
   }
}