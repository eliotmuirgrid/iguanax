//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SSOmicrosoft.cpp
//
//  Description: Implementation for Microsoft SSO role management with caching
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <SSO/SSOjwt.h>
#include <SSO/SSOmicrosoft.h>
#include <USR/USRuser.h>
#include <WEB/WEBrequest.h>
#include <WEB/WEBresponse.h>
#include <time.h>
COL_LOG_MODULE;

static COLmap<COLstring, COLstring> g_MicrosoftRoleMap;
static COLmap<COLstring, COLstring> g_MicrosoftGroupMap;
static time_t                       g_LastRoleRefresh   = 0;
static const time_t                 ROLE_CACHE_DURATION = 24 * 60 * 60;

static COLstring SSOmicrosoftFetchGroupName(const COLstring& Token, const COLstring& GroupId) {
   COL_FUNCTION(SSOmicrosoftFetchGroupName);
   WEBrequest Req;
   Req.Url                      = "https://graph.microsoft.com/v1.0/groups/" + GroupId + "?$select=displayName";
   Req.Headers["Accept"]        = "application/json";
   Req.Headers["Content-Type"]  = "application/json";
   Req.Headers["User-Agent"]    = "IguanaX-SSO";
   Req.Headers["Authorization"] = "Bearer " + Token;
   Req.Timeout                  = 10000;

   WEBresponse Resp;
   WEBrequestBlockingGet(Req, &Resp);
   COL_VAR2(Resp.ReturnCode, Resp.Body);
   if(Resp.ReturnCode != 200) {
      COL_TRC("Failed to fetch group " + GroupId + ". HTTP " + COLint32ToString(Resp.ReturnCode));
      return GroupId;
   }

   COLvar Group;
   if(!Group.parse(Resp.Body)) {
      COL_TRC("Failed to parse group response for " + GroupId);
      return GroupId;
   }

   const COLstring DisplayName = Group["displayName"].asString();
   if(DisplayName.is_null()) {
      COL_TRC("Group " + GroupId + " has no displayName");
      return GroupId;
   }

   COL_TRC("Resolved group " + GroupId + " to " + DisplayName);
   return DisplayName;
}

static bool SSOmicrosoftRolesImpl(const COLstring& Token) {
   COL_FUNCTION(SSOmicrosoftRolesImpl);
   WEBrequest Req;
   Req.Url                      = "https://graph.microsoft.com/v1.0/roleManagement/directory/roleDefinitions";
   Req.Headers["Accept"]        = "application/json";
   Req.Headers["Content-Type"]  = "application/json";
   Req.Headers["User-Agent"]    = "IguanaX-SSO";
   Req.Headers["Authorization"] = "Bearer " + Token;
   Req.Timeout                  = 10000;

   WEBresponse Resp;
   WEBrequestBlockingGet(Req, &Resp);
   if (Resp.ReturnCode != 200) {
      COL_TRC("Failed to fetch Microsoft roles. HTTP " + COLint32ToString(Resp.ReturnCode));
      return false;
   }

   COLvar Roles;
   if (!Roles.parse(Resp.Body)) {
      COL_TRC("Failed to parse Microsoft roles response");
      return false;
   }

   if (!Roles.exists("value") || !Roles["value"].isArray()) {
      COL_TRC("Invalid roles response format - missing or invalid 'value' array");
      return false;
   }

   g_MicrosoftRoleMap.clear();

   for(const auto& role : Roles["value"].array()) {
      if(role.exists("id") && role.exists("displayName")) {
         const COLstring Id          = role["id"].asString();
         const COLstring DisplayName = role["displayName"].asString();
         if(!Id.is_null() && !DisplayName.is_null()) { g_MicrosoftRoleMap[Id] = DisplayName; }
      }
   }
   g_LastRoleRefresh = time(NULL);
   for(const auto& it : g_MicrosoftRoleMap) { COL_VAR2(it.first, it.second); }
   COL_TRC("Successfully loaded " + COLint32ToString(g_MicrosoftRoleMap.size()) + " Microsoft roles");
   return true;
}

void SSOmicrosoftFetchRoles(const COLstring& Token) {
   COL_FUNCTION(SSOmicrosoftFetchRoles);
   const time_t Now          = time(NULL);
   const bool   CacheExpired = Now - g_LastRoleRefresh > ROLE_CACHE_DURATION;
   const bool   MapEmpty     = g_MicrosoftRoleMap.empty();
   COL_VAR2(MapEmpty, CacheExpired);
   if(!MapEmpty && !CacheExpired) {
      COL_TRC("Using cached Microsoft roles.");
      return;
   }
   if(!SSOmicrosoftRolesImpl(Token)) { COL_TRC("Failed to refresh Microsoft roles"); }
}

COLstring SSOmicrosoftGetRoleName(const COLstring& RoleId) {
   COL_FUNCTION(SSOmicrosoftGetRoleName);
   if(g_MicrosoftRoleMap.count(RoleId)) { return g_MicrosoftRoleMap[RoleId]; }
   COL_TRC("Unknown Microsoft role ID: " + RoleId);
   return RoleId;
}

void SSOmicrosoftResolveRoleIds(const COLstring& Token, const COLvar& Wids, COLarray<COLstring>& Roles) {
   COL_FUNCTION(SSOmicrosoftResolveRoleIds);
   SSOmicrosoftFetchRoles(Token);
   COL_VAR(g_MicrosoftRoleMap.size());
   if(!Wids.isArray()) {
      COL_TRC("Invalid wids format: " << Wids);
      return;
   }
   for(const auto& it : Wids.array()) {
      const COLstring ResolvedName = SSOmicrosoftGetRoleName(it);
      COL_VAR2(ResolvedName, it);
      if(ResolvedName != it) { Roles.push_back("#" + COLremoveAllWhiteSpace(ResolvedName)); }
   }
}

COLstring SSOmicrosoftGetGroupName(const COLstring& Token, const COLstring& GroupId) {
   COL_FUNCTION(SSOmicrosoftGetGroupName);
   if(g_MicrosoftGroupMap.count(GroupId)) {
      COL_TRC("Using cached group name for " + GroupId);
      return g_MicrosoftGroupMap[GroupId];
   }
   const COLstring GroupName = SSOmicrosoftFetchGroupName(Token, GroupId);
   g_MicrosoftGroupMap[GroupId] = GroupName;
   return GroupName;
}

void SSOmicrosoftResolveGroupIds(const COLstring& Token, const COLvar& Groups, COLarray<COLstring>& Roles) {
   COL_FUNCTION(SSOmicrosoftResolveGroupIds);
   if(!Groups.isArray()) {
      COL_TRC("Invalid groups format: " << Groups);
      return;
   }
   for(const auto& it : Groups.array()) {
      const COLstring GroupId      = it.asString();
      const COLstring ResolvedName = SSOmicrosoftGetGroupName(Token, GroupId);
      COL_VAR2(GroupId, ResolvedName);
      if(ResolvedName == GroupId) { Roles.push_back(GroupId); }
      else                        { Roles.push_back("#" + COLremoveAllWhiteSpace(ResolvedName)); }
   }
}

bool SSOmicrosoftExtractUserInfo(const COLstring& Credential, const COLstring& Token, USRuser& User, COLstring& Error) {
   COL_FUNCTION(SSOmicrosoftExtractUserInfo);
   COLvar UserInfo;
   if(!SSOverifyToken(Credential, SSOgetConfig().m_ClientConfig.m_ClientId, &UserInfo, Error, MICROSOFT)) {
      Error = "Failed to verify Microsoft token. " + Error;
      return false;
   }
   COL_VAR(UserInfo.json(1));
   COLarray<COLstring> Roles;
   SSOmicrosoftResolveRoleIds(Token, UserInfo["wids"], Roles);
   SSOmicrosoftResolveGroupIds(Token, UserInfo["groups"], Roles);
   User = USRuser(COLvarGetString(UserInfo, "iguana_unique_id", ""), "", Roles, false);
   User.setProvider(SSOproviderToString(MICROSOFT));
   User.setDisplayName(COLvarGetString(UserInfo, "preferred_username", ""));
   User.setExternal();
   COL_VAR(User);
   if(User.displayName().is_null() || User.username().is_null()) {
      COL_TRC("Microsoft token missing required user information");
      Error = "Invalid user information from Microsoft";
      return false;
   }
   return true;
}
