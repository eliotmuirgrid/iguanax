// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITEgithub
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 28 March 2023 - 11:34AM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <DIR/DIRpath.h>
#include <GIT/GITpath.h>
#include <GITU/GITUpermission.h>
#include <GITU/GITUurl.h>
#include <GITE/GITEgithub.h>
#include <GITE/GITEutils.h>
#include <GSV/GSVconfig.h>
#include <GSV/GSVlink.h>
#include <GUID/GUIDid.h>
#include <WEB/WEBrequest.h>
#include <WEB/WEBresponse.h>
COL_LOG_MODULE;

const COLstring GITE_GITHUB_CLOUD_API_URL = "https://api.github.com";
const char* const GITE_GITHUB_TOKEN_SCOPES[] = {"admin:gpg_key", "admin:org", "admin:public_key", "project", "repo", "user"};

static COLstring GITEgithubCreateServerUrl(const GSVconfig& Config, const COLstring& Path){
   COL_FUNCTION(GITEgithubCreateServerUrl);
   COL_TRC("Using server: " << Config.m_ServerUrl);
   COLstring Url;
   GSVconstructServerLink(Config.m_ServerUrl, &Url);
   Url.append("/api/v3" + Path);
   return Url;
}

static bool GITEgithubTokenScopesValid(const COLstring& TokenScopes, COLstring* MissingScopes){
   COL_FUNCTION(GITEgithubTokenScopesValid);
   COL_VAR(TokenScopes);
   for (const auto Scope : GITE_GITHUB_TOKEN_SCOPES) {
      if (TokenScopes.find(Scope) == npos) {
         if(!MissingScopes->is_null()){MissingScopes->append(", ");}
         MissingScopes->append(Scope);
      }
   }
   if(MissingScopes->is_null()){return true;}
   return false;
}

void GITEgithubPrepare(const COLstring& Path, const GSVconfig& Config, WEBrequest* pRequest){
   COL_FUNCTION(GITEgithubPrepare);
   WEBrequest& Request = *pRequest;
   if (!Config.m_ServerUrl.is_null() && Config.m_Local) { Request.Url = GITEgithubCreateServerUrl(Config, Path); }
   else                                                 { Request.Url = GITE_GITHUB_CLOUD_API_URL + Path;        }
   Request.Headers["Authorization"] = "Bearer " + Config.m_Token;
   Request.Headers["Accept"]        = "application/json";
   Request.Headers["User-Agent"]    = "IguanaX";
   Request.Timeout = 5000;
}

COLstring GITEgithubExtractErrorMessage(const WEBresponse& Response){
   COL_FUNCTION(GITEgithubExtractErrorMessage);
   COLvar Result;
   COL_VAR(Result.json(1));
   if (!Result.parse(Response.Body)){ 
      return Response.Body;
   }
   COL_VAR(Result);
   if(Result.exists("message")){
      return Result["message"].asString();
   }
   if(Result.exists("errors") && Result["errors"].exists("message")){
      return  + " Error: " + Result["errors"][0]["message"].asString();
   } else {
      COL_TRC("Could not find error.message so returning the body");
      return Response.Body;
   }
}

void GITEgithubThrowBadResult(const WEBresponse& Response){
   COL_FUNCTION(GITEgithubThrowBadResult);
   if (Response.ReturnCode != 200){
      throw COLerror(GITEgithubExtractErrorMessage(Response), 0);
   }
}

void GITEgithubRepoNewPersonal(const GSVconfig& Config, const COLstring& Repository){
   COL_FUNCTION(GITEgithubRepoNewPersonal);
   WEBrequest Request;
   GITEgithubPrepare("/user/repos", Config, &Request);
   COLvar Body;
   Body["name"] = Repository;
   Body["private"] = true;
   Request.Body = Body.json();
   WEBresponse Response;
   WEBrequestBlockingPost(Request, &Response);
   if (201 == Response.ReturnCode){ return; }
   GITEgithubThrowBadResult(Response);
}

void GITEgithubRepoNewOrganization(const GSVconfig& Config, const COLstring& Owner, const COLstring& Repository){
   COL_FUNCTION(GITEgithubRepoNewOrganization);
   WEBrequest Request;
   GITEgithubPrepare("/orgs/" + Owner + "/repos", Config, &Request);
   COLvar Body;
   Body["name"] = Repository;
   Body["private"] = true;
   Request.Body = Body.json();
   WEBresponse Response;
   WEBrequestBlockingPost(Request, &Response);
   COL_VAR(Response);
   COLvar Result;
   Result.parse(Response.Body);
   COL_VAR(Result.json(1));
   if (201 == Response.ReturnCode){ return; } // 201 is success
   GITEgithubThrowBadResult(Response);
}

bool GITEgithubCheckToken(const COLstring& Token, const GSVconfig& Config, int& ReturnCode, COLstring* pError){
   COL_FUNCTION(GITEgithubCheckToken);
   WEBrequest Request;
   GITEgithubPrepare("/user", Config, &Request);
   Request.Headers["Authorization"] = "Bearer " + Token; // overwrite token so we check the new one
   COL_VAR(Request);
   WEBresponse Response;
   WEBrequestBlockingGet(Request, &Response);
   COL_VAR(Response);
   ReturnCode = Response.ReturnCode;
   if (Response.ReturnCode != 200){
      *pError = GITEgithubExtractErrorMessage(Response);
      return false;
   }
   COLstring MissingScopes;
   if(!GITEgithubTokenScopesValid(Response.Headers["x-oauth-scopes"], &MissingScopes)){
      COL_TRC("Token is missing scopes " + MissingScopes);
      *pError = MissingScopes;
   }
   return true;
}

bool GITEgithubRegisterSshKey(const COLstring& PublicKey, const GSVconfig& Config, COLstring* pError){
   COL_FUNCTION(GITEgithubRegisterSshKey);
   WEBrequest Request;
   GITEgithubPrepare("/user/keys", Config, &Request);
   COLvar Body;
   Body["title"]= "Generated by IguanaX";
   Body["key"]  = PublicKey;
   Request.Body = Body.json();
   COL_VAR(Request);
   WEBresponse Response;
   COL_VAR2(Response.ReturnCode, Response.Body);
   WEBrequestBlockingPost(Request, &Response);
   if (Response.ReturnCode == 201){ return true; } // Github's api documentation says it will return 201 on created
   switch (Response.ReturnCode){                                                   
      case 401: *pError = "Key or authorization token invalid.";                        return false;
      case 403: *pError = "User does not have the correct permissions to set the key."; return false;
      case 422: *pError = "Operation tried too frequently or validation failed. Please try again later."; return false; 
      default:  *pError = GITEgithubExtractErrorMessage(Response);                      return false;
   }
}

static bool GITEgithubGroupExtract(const COLvar& Data, COLvar* pResult, COLstring* pError){
   COL_FUNCTION(GITEgithubGroupExtract);
   const COLvar& Values = Data;
   if (Values.size() == 0) { return true; }
   if(!Values.isArray()) { 
      *pError = "Failed to parse Github output"; 
      return false; 
   }
   for (const auto& Group : Values.array()){
      pResult->push_back(Group["login"]);
   }
   return true;
}

static bool GITEgithubGetGroups(const GSVconfig& Config, COLvar* pGroups, COLstring* pError){
   COL_FUNCTION(GITEgithubGetGroups);
   COL_TRC("Fetching the groups where the current user has membership (limited to first 100 results)");
   WEBrequest Request;
   GITEgithubPrepare("/user/orgs", Config, &Request);
   Request.GetVariables.add("page", "1");
   Request.GetVariables.add("per_page", "100");
   WEBresponse Response;
   WEBrequestBlockingGet(Request, &Response);
   COLvar GroupsData;
   if (Response.ReturnCode == 200) {
      if (!GroupsData.parse(Response.Body)) { 
         *pError = "Failed to parse response from Github."; 
         return false;
      }
      COL_VAR(GroupsData.json(1));
      *pGroups = GroupsData;
      return true;
   }
   switch (Response.ReturnCode){
      case 401: *pError = "Key or authorization token invalid."; return false;
      case 403: *pError = "User does not have the correct permissions to list organizations."; return false;
      default:  *pError = "Failed to get organization information"; return false;
   }
}

static bool GITEgithubGetUsername(const GSVconfig& Config, COLstring* pUsername, COLstring* pError){
   COL_FUNCTION(GITEgithubGetUsername);
   COL_TRC("Fetching the current user's username");
   WEBrequest Request;
   GITEgithubPrepare("/user", Config, &Request);
   WEBresponse Response;
   WEBrequestBlockingGet(Request, &Response);
   COLvar UserData;
   if (!UserData.parse(Response.Body) || !UserData.exists("login")) { 
      *pError = "Failed to get username from GitHub.";
      return false;
   } 
   COL_VAR("Got username"); 
   *pUsername = UserData["login"]; 
   return true;
}

bool GITEgithubUpdateOwners(const COLstring& Username, bool Force, const GSVconfig& Config, COLvar* pResult, COLstring* pError){
   COL_FUNCTION(GITEgithubUpdateOwners);
   COLstring FileName = DIRconfigUserOwnersFile(Username);
   GITEremoveFileIfForce(Force, FileName);
   // Lookup the Github username and add it as an owner
   COLstring GithubUsername;
   if (!GITEgithubGetUsername(Config, &GithubUsername, pError)) { return false; }
   pResult->setArrayType();
   pResult->push_back(GithubUsername); 
   // Then get all the groups
   COLvar Groups;
   if (!GITEgithubGetGroups(Config, &Groups, pError)) { return false; }
   COL_TRC("Parsing Github response");
   if (!GITEgithubGroupExtract(Groups, pResult, pError)) {
      return false; 
   }
   COL_VAR(*pResult);
   COL_TRC("Now updating the file on disk");
   return GITEupdateOwnersFile(Username, pResult, pError);
}

static bool GITEgithubParseCheckRepoResponse(const WEBresponse& Response, COLvar* pResult, COLstring* pError) {
   COL_FUNCTION(GITEgithubParseCheckRepoResponse);
   COLvar Data, ResponseJson;
   ResponseJson.parse(Response.Body);
   COL_VAR(ResponseJson);
   int Code = Response.ReturnCode;
   if(Code == 401)                { *pError = "Invalid credentials";                       return false; }
   if(Code == 200)                { 
      Data["slug_exists"] = true;
      Data["repo_link"] = ResponseJson["html_url"];
      *pResult = Data;
      return true;
   }
   if(Code == 404)                { 
      Data["slug_exists"] = false;
      *pResult = Data;
      return true;
   }
   if(ResponseJson.exists("message")){
      *pError = ResponseJson["message"];
   }else{
      *pError = ResponseJson;
   }
   return false;
}

bool GITEgithubCheckRepo(const COLstring& Owner, const COLstring& Slug, const GSVconfig& Config, COLvar* pResult, COLstring* pError){
   COL_FUNCTION(GITEgithubCheckRepo);
   WEBrequest Request;
   GITEgithubPrepare("/repos/" + Owner + "/" + Slug, Config, &Request);
   WEBresponse Response;
   WEBrequestBlockingGet(Request, &Response);
   COL_VAR(Response.ReturnCode);
   return GITEgithubParseCheckRepoResponse(Response, pResult, pError);
}

void GITEgithubRepoNew(const GSVconfig& Config, const COLstring& Owner, const COLstring& Repository){
   COL_FUNCTION(GITEgithubRepoNew);
   COLvar GroupsData, Groups;
   COLstring Error;
   if (!GITEgithubGetGroups(Config, &GroupsData, &Error)) {
      throw COLerror("Failed to get organizations from Github");
   }
   COL_TRC("Parsing Github organizations response");
   if (!GITEgithubGroupExtract(GroupsData, &Groups, &Error)) {
      throw COLerror("Failed to get organizations from Github");
   }
   bool IsOrganization = false;
   for (int i = 0; i < Groups.size(); i++){
      COL_VAR(Groups[i]);
      if (Groups[i].asString() == Owner) { 
         IsOrganization = true; 
         break;
      }
   }
   if (IsOrganization){ return GITEgithubRepoNewOrganization(Config, Owner, Repository); } 
   COL_TRC("Are we creating a personal repo?");
   COLstring Username;
   if (!GITEgithubGetUsername(Config, &Username, &Error)) { 
      throw COLerror("Failed to verify username with Github");
   }
   if (Username != Owner) {
      throw COLerror("Cannot create a repository in " + Owner + ": you are not a member.");
   } 
   return GITEgithubRepoNewPersonal(Config, Repository);
}

bool GITEgithubCheckSshAccess(const COLstring& IguanaUser, const GSVconfig& Auth, COLstring* pError){
   COL_FUNCTION(GITEgithubCheckSshAccess);
   COLstring Guid = GUIDnewTinyId();
   COLstring SshUrl;
   GITUmakeSshUrl(Auth.m_ServerUrl, "sshTest" + Guid, "IguanaXsshTest", &SshUrl);
   COLstring Out;
   COL_TRC("Will try to do a dry-run push to " + SshUrl);
   int ExitCode = -1;
   GITUpermissionWrite(IguanaUser, SshUrl, GITuserScratchDir(IguanaUser, Guid), &ExitCode, &Out);
   COL_VAR2(ExitCode, Out);
   if (ExitCode == 128 ||  ExitCode == 0){
      if (Out.find("Permission denied") != npos){
         COL_TRC("Public key authentication error - no ssh access to Github.");
         *pError = "Public key authentication failed - no ssh access to Github";
         return false;
      }
      COL_TRC("Did not see public key error so ssh access is good");
      return true;
   }
   return false;
}
