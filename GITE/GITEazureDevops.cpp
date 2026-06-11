// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITEazureDevops
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Wednesday 15 May 2024 - 10:38AM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <DIR/DIRpath.h>
#include <GIT/GITpath.h>
#include <GITU/GITUpermission.h>
#include <GITU/GITUurl.h>
#include <GITE/GITEazureDevops.h>
#include <GITE/GITEazureEndpoints.h>
#include <GITE/GITEazureUtils.h>
#include <GITE/GITEutils.h>
#include <GSV/GSVconfig.h>
#include <GUID/GUIDid.h>
#include <SDB/SDBcapi.h>
#include <WEB/WEBrequest.h>
#include <WEB/WEBresponse.h>
COL_LOG_MODULE;

bool GITEazureCheckToken(const GSVconfig& Config, bool Secure, int& ReturnCode, COLstring* pError) {
   COL_FUNCTION(GITEazureCheckToken);
   WEBrequest Request;
	GITEazureSetupRequest(GITEazureTokenEndpoint(Config, Secure), Config, &Request);
   Request.Headers["Authorization"] = "Basic " + GITEazureEncodeToken(Config.m_Token);
   COL_VAR(Request);
   WEBresponse Response;
   WEBrequestBlockingGet(Request, &Response);
   COL_VAR2(Response.ReturnCode, Response.Body.size());
   COLvar Out;
   Out.parse(Response.Body);
   COL_VAR(Out.json(1));
   ReturnCode = Response.ReturnCode;
   if (Response.ReturnCode == 401){
      COL_TRC("Invalid user and access token combination.");
      *pError = "Failed to authenticate with Azure Devops using this access token";
      return false;
   }
   if (Response.ReturnCode != 200){
      COL_TRC("Failed to authenticate the Azure Devops access token. HTTP Code=" + COLintToString(Response.ReturnCode) + " " + Response.Body);
      *pError = Response.Body;
      return false;
   }
   return true;
}

static bool GITEazureGetUserId(const GSVconfig& Config, bool Secure, COLstring* pUserId, COLstring* pError) {
   COL_FUNCTION(GITEazureGetUserId);
   WEBrequest Request;
   WEBresponse UsernameResponse;
   GITEazureSetupRequest(GITEazureUserEndpoint(Config, Secure), Config, &Request);
	COL_VAR(Request);
   WEBrequestBlockingGet(Request, &UsernameResponse);
   bool ExtraLogging = GITEazureExtraLogging();
   if (ExtraLogging) { SDBIlogService("Get user id - Code:" + COLintToString(UsernameResponse.ReturnCode) + "\n Raw response: " + UsernameResponse.Body); }
   COL_VAR(UsernameResponse);
   COLvar UserData;
   if (!UserData.parse(UsernameResponse.Body) || UsernameResponse.ReturnCode != 200) {
      *pError = "Failed to get user id information from Azure";
      if (ExtraLogging) { SDBIlogService("Failed to get user id information from Azure"); }
      return false;
   }
   COL_VAR(UserData.json(1));
   *pUserId = UserData["authenticatedUser"]["id"];
   if (ExtraLogging) { SDBIlogService("Got user id: " + UserData["authenticatedUser"]["id"].asString()); }
   return true;
}

static bool GITEazureGetOrganizations(const COLstring& UserId, const GSVconfig& Config, bool Secure,
												  COLarray<COLstring>* pOut, COLstring* pError){
   COL_FUNCTION(GITEazureGetOrganizations);
   WEBrequest Request;
   WEBresponse EntitlementsResponse;
	GITEazureSetupRequest(GITEazureOrgEndpoint(UserId, Config, Secure), Config, &Request);
   COL_VAR(Request);
   WEBrequestBlockingGet(Request, &EntitlementsResponse);
   COL_VAR(EntitlementsResponse);
	if (!GITEazureParseOrganizations(EntitlementsResponse, pOut, pError)) { return false; }
	return true;
}

static bool GITEazureAddProjectsToOwners(const COLstring& Organization, const GSVconfig& Config, bool Secure,
													  COLvar* pResult, COLstring* pError) {
	COL_FUNCTION(GITEazureAddProjectsToOwners);
	WEBrequest Request;
	WEBresponse ProjectsResponse;
	GITEazureSetupRequest(GITEazureProjToOwnerEndpoint(Organization, Config, Secure), Config, &Request);
	COL_VAR(Request);
	// Use the user Id to get all entitlements for that user
	WEBrequestBlockingGet(Request, &ProjectsResponse);
	COL_VAR(ProjectsResponse);
	if (!GITEazureParseProjects(ProjectsResponse, Organization, pResult, pError)) { return false; }
	return true;
}

bool GITEazureUpdateOwners(const COLstring& Username, bool Force, const GSVconfig& Auth, bool Secure, COLvar* pResult, COLstring* pError){
   COL_FUNCTION(GITEazureUpdateOwners);
   const COLstring FileName = DIRconfigUserOwnersFile(Username);
   bool ExtraLogging = GITEazureExtraLogging();
   if (ExtraLogging) {
      COLstring isForce = (Force ? "true" : "false");
      SDBIlogService("Update owners called with force set to: " + isForce);
   }
   GITEremoveFileIfForce(Force, FileName);
   // Lookup the Azure username and find the user id
   COLstring UserId;
   if (!GITEazureGetUserId(Auth, Secure, &UserId, pError)){ return false; }
   COL_VAR(UserId);
   COLarray<COLstring> Organizations;
   if (!GITEazureGetOrganizations(UserId, Auth, Secure, &Organizations, pError)){
      COL_TRC("Failed to get all organizations");
      return false;
   }
   // Now get all the projects for each organization and add <organization>/<project> as owners
	for (const auto& Organization : Organizations) {
		if (!GITEazureAddProjectsToOwners(Organization, Auth, Secure, pResult, pError)) { continue; }
   }
   COL_TRC("Now updating the file on disk");
   return GITEupdateOwnersFile(Username, pResult, pError);
}

static bool GITEazureParseCheckRepo(const WEBresponse& Response, COLvar* pResult, COLstring* pError) {
   COL_FUNCTION(GITEazureParseCheckRepo);
   COLvar Data, ResponseJson;
   ResponseJson.parse(Response.Body);
   int Code = Response.ReturnCode;
   if(Code == 401)                { *pError = "Invalid credentials";                            return false; }
   if(Code != 200 && Code != 404) { *pError = "Could not parse the check repository response."; return false; }
   bool SlugExists = Code == 200;
   Data["slug_exists"] = SlugExists;
   if(SlugExists && ResponseJson.exists("webUrl")) { Data["repo_link"] = ResponseJson["webUrl"]; }
   *pResult = Data;
   return true;
}

bool GITEazureCheckRepo(const COLstring& Owner, const COLstring& Slug, const GSVconfig& Config, bool Secure,
								COLvar* pResult, COLstring* pError){
   COL_FUNCTION(GITEazureCheckRepo);
   WEBrequest Request;
   WEBresponse Response;
	GITEazureSetupRequest(GITEazureRepoEndpoint(Owner, Slug, Config, Secure), Config, &Request);
	COL_VAR(Request);
   WEBrequestBlockingGet(Request, &Response);
	COL_VAR(Response);
   return GITEazureParseCheckRepo(Response, pResult, pError);
}

bool GITEazureCheckSshAccess(const COLstring& IguanaUser, const COLstring& Owner, const GSVconfig& Config, COLstring* pError){
   COL_FUNCTION(GITEazureCheckSshAccess);
	const COLstring Guid = GUIDnewTinyId();
   COLstring SshUrl;
   if (!Config.m_Local){ GITUmakeSshUrl(Config.m_ServerUrl, "v3/" + Owner, "IguanaXsshTest/testRepo", &SshUrl); }
   else                { GITUmakeSshUrl(Config.m_ServerUrl, "DefaultCollection", "IguanaXsshTest/testRepo", &SshUrl); } // TODO - we probably want a modal to configure the "DefaultCollection". Do it if we see complaints come in.
	COLstring Out;
   COL_TRC("Will try to do a dry-run push to " + SshUrl);
   int ExitCode = -1;
   GITUpermissionWrite(IguanaUser, SshUrl, GITuserScratchDir(IguanaUser, Guid), &ExitCode, &Out);
   COL_VAR2(ExitCode, Out);
   if (ExitCode == 128){
      if (Out.find("Public key authentication failed") != npos || Out.find("Permission denied") != npos){
         COL_TRC("Public key authentication error - no ssh access to azure.");
         *pError = "Public key authentication failed - no ssh access to Azure Devops";
         return false;
      }
      COL_TRC("Did not see public key error so ssh access is good");
      return true;
   } if ((ExitCode == 0 || ExitCode == -20) && Out.find("TF200016") != npos){
      // Azure case on windows server - its fine for the project to not exist, this happens after we auth via ssh key
      return true;
   }
   return false;
}

void GITEazureRepoNew(const GSVconfig& Config, bool Secure, const COLstring& Owner, const COLstring& Repository, COLstring* pGitUrl){
   COL_FUNCTION(GITEazureRepoNew);
   WEBrequest Request;
   WEBresponse Response;
	GITEazureSetupRequest(GITEazureNewRepoEndpoint(Owner, Config, Secure), Config, &Request);
   COLvar Body;
   Body["name"] = Repository;
   Request.Body = Body.json(1);
	COL_VAR(Request);
   WEBrequestBlockingPost(Request, &Response);
   COLvar Out;
   Out.parse(Response.Body);
   COL_VAR(Out);
   if (Response.ReturnCode != 201) {
      throw COLerror("Failed to create new repository: " + Out["message"].asString());
   } else {
      *pGitUrl = Out["sshUrl"];
   }
}