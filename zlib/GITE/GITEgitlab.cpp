//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GITEgitlab.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   28/03/24 11:41 AM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <DIR/DIRpath.h>
#include <GIT/GITpath.h>
#include <GITU/GITUpermission.h>
#include <GITU/GITUurl.h>
#include <GITE/GITEgitlab.h>
#include <GITE/GITEutils.h>
#include <GSV/GSVconfig.h>
#include <GSV/GSVlink.h>
#include <GUID/GUIDid.h>
#include <SFI/SFIuriEscapeFilter.h>
#include <WEB/WEBrequest.h>
#include <WEB/WEBresponse.h>
COL_LOG_MODULE;

const COLstring GITE_GITLAB_CLOUD_API_URL = "https://gitlab.com/api/v4";
const char* const GITE_GITLAB_TOKEN_SCOPES[] = {"api","read_user","read_repository","read_api","write_repository"};

static COLstring GITEgitlabCreateServerUrl(const GSVconfig& Config, const COLstring& Path){
   COL_FUNCTION(GITEgitlabCreateServerUrl);
   COL_TRC("Using server: " << Config.m_ServerUrl);
   COLstring Url;
   GSVconstructServerLink(Config.m_ServerUrl, &Url);
   Url.append("/api/v4" + Path);
   return Url;
}

static void GITEgitlabPrepare(const COLstring& Path, const GSVconfig& Config, WEBrequest* pRequest) {
   COL_FUNCTION(GITEgitlabPrepare);
   WEBrequest& Request = *pRequest;
   if (!Config.m_ServerUrl.is_null() && Config.m_Local) { Request.Url = GITEgitlabCreateServerUrl(Config, Path); }
   else                                                 { Request.Url = GITE_GITLAB_CLOUD_API_URL + Path;        }
   Request.Headers["PRIVATE-TOKEN"] = Config.m_Token;
   Request.Headers["Accept"]        = "application/json";
   Request.Headers["Content-Type"]  = "application/json";
   Request.Timeout = 15000;
}

static bool GITEgitlabTokenScopesValid(const GSVconfig& Auth, COLstring* MissingScopes){
   COL_FUNCTION(GITEgitlabTokenScopesValid);
   WEBrequest Request;
   GITEgitlabPrepare("/personal_access_tokens/self", Auth, &Request);
   WEBresponse Response;
   WEBrequestBlockingGet(Request, &Response);
   if( Response.ReturnCode != 200){
      COL_TRC("Failed to get scopes");
      COL_VAR(Response);
      return true;
   }
   COLvar Data;
   if (!Data.parse(Response.Body)) {
      COL_TRC("Failed to parse response body");
      return true;
   }
   COLstring TokenScopes = Data["scopes"].json();
   COL_VAR(TokenScopes);
   for (const auto Scope : GITE_GITLAB_TOKEN_SCOPES) {
      if (TokenScopes.find(Scope) == npos) {
         if(!MissingScopes->is_null()){MissingScopes->append(", ");}
         MissingScopes->append(Scope);
      }
   }
   if(MissingScopes->is_null()){return true;}
   return false;
}

bool GITEgitlabCheckToken(const COLstring& Token, const GSVconfig& Auth, int& ReturnCode, COLstring* pError) {
   COL_FUNCTION(GITEgitlabCheckToken);
   WEBrequest Request;
   GITEgitlabPrepare("/user", Auth, &Request);
   Request.Headers["PRIVATE-TOKEN"] = Token; // overwrite token with the one we are checking
   COL_VAR(Request);
   WEBresponse Response;
   WEBrequestBlockingGet(Request, &Response);
   COL_VAR(Response);
   ReturnCode = Response.ReturnCode;
   if (Response.ReturnCode == 401){ // GitLab's error message is just 'Unauthorized' so return something nicer.
      COL_TRC("Invalid user and access token combination.");
      *pError = "Failed to authenticate with GitLab using this access token";
      return false;
   }
   if (Response.ReturnCode != 200){
      COL_TRC("Failed to authenticate the Gitlab access token. HTTP Code=" + COLintToString(Response.ReturnCode) + " " + Response.Body);
      *pError = Response.Body;
      return false;
   }
    COLstring MissingScopes;
   if(!GITEgitlabTokenScopesValid(Auth, &MissingScopes)){
      COL_TRC("Token is missing scopes " + MissingScopes);
      *pError = MissingScopes;
   }
   return true;
}

static COLstring GITEgitlabParseSshKeyError(const COLstring& Response){
   COL_FUNCTION(GITEgitlabParseSshKeyError);
   COLvar ErrorResponse;
   ErrorResponse.parse(Response);
   if (COLvarPathExists(ErrorResponse, "message/key")){
      COL_TRC("Returning error message from message.key[0]");
      return ErrorResponse["message"]["key"][0];
   } else if (COLvarPathExists(ErrorResponse, "message/fingerprint")){
      COL_TRC("Returning error message from message.fingerprint[0]");
      return ErrorResponse["message"]["fingerprint"][0];
   } else if (COLvarPathExists(ErrorResponse, "message/fingerprint_sha256")){ // from testing, the key is actually fingerprint_sha256 not just fingerprint, despite what the documentation says.
      COL_TRC("Returning error message from message.fingerprint[0]");
      return ErrorResponse["message"]["fingerprint_sha256"][0];
   } else {
      COL_TRC("Could not find expected message format so returning the body");
      return Response;
   }
}

bool GITEgitlabRegisterSshKey(const COLstring& PublicKey, const GSVconfig& Config, COLstring* pError) {
   COL_FUNCTION(GITEgitlabRegisterSshKey);
   WEBrequest Request;
   GITEgitlabPrepare("/user/keys", Config, &Request);
   COLvar Body;
   Body["title"]= "Generated by IguanaX";
   Body["key"]  = PublicKey;
   Request.Body = Body.json();
   COL_VAR(Request);
   WEBresponse Response;
   COL_VAR2(Response.ReturnCode, Response.Body);
   WEBrequestBlockingPost(Request, &Response);
   if (Response.ReturnCode == 201){ return true; } // Gitlab's api documentation says it will return 201 on success
   switch (Response.ReturnCode){                                                   
      case 400: *pError = "Key or authorization token invalid.";                        return false;
      case 401: *pError = "Key or authorization token invalid";                         return false;
      case 403: *pError = "User does not have the correct permissions to set the key."; return false;
      default:  *pError = GITEgitlabParseSshKeyError(Response.Body);                    return false;
   }
}

static bool GITEgitlabGroupExtract(const COLvar& Data, COLvar* pResult, COLstring* pError){
   COL_FUNCTION(GITEgitlabGroupExtract);
   const COLvar& Values = Data;
   if (Values.size() == 0) { return true; }
   if(!Values.isArray()) { 
      *pError = "Failed to parse Gitlab output"; 
      return false; 
   }
   for (const auto& Group : Values.array()){
      pResult->push_back(Group["path"]);
   }
   return true;
}

static bool GITEgitlabGetGroups(const GSVconfig& Auth, COLvar* pGroups, COLstring* pError) {
   COL_FUNCTION(GITEgitlabGetGroups);
   COL_TRC("Fetching the groups where the current user has membership and is a developer (access level 30)");
   WEBrequest Request;
   GITEgitlabPrepare("/groups?owned=false&min_access_level=30", Auth, &Request);
   WEBresponse Response;
   WEBrequestBlockingGet(Request, &Response);
   COLvar GroupsData;
   if (!GroupsData.parse(Response.Body)) { 
      *pError = "Failed to parse response from Gitlab."; 
      return false;
   }
   COL_VAR(GroupsData.json(1));
   *pGroups = GroupsData;
   return true;
}

bool GITEgitlabUpdateOwners(const COLstring& Username, bool Force, const GSVconfig& Auth, COLvar* pResult, COLstring* pError) {
   COL_FUNCTION(GITEgitlabUpdateOwners);
   COLstring FileName = DIRconfigUserOwnersFile(Username);
   GITEremoveFileIfForce(Force, FileName);
   // Lookup the Gitlab username and add it as an owner
   WEBrequest UsernameRequest;
   WEBresponse UsernameResponse;
   GITEgitlabPrepare("/user", Auth, &UsernameRequest);
   WEBrequestBlockingGet(UsernameRequest, &UsernameResponse);
   COLvar UserData;
   pResult->setArrayType();
   if (!UserData.parse(UsernameResponse.Body) || !UserData.exists("username")) { 
      *pError = "Failed to get username from GitLab.";
      return false;
   } else { 
      COL_VAR("pushing username"); 
      pResult->push_back(UserData["username"]); 
   }
   // Then get all the groups
   COLvar Groups;
   if (!GITEgitlabGetGroups(Auth, &Groups, pError)) { return false; }
   COL_TRC("Parsing Gitlab response");
   if (!GITEgitlabGroupExtract(Groups, pResult, pError)) {
      return false; 
   }
   COL_VAR2(UserData.json(1), pResult->size());
   COL_TRC("Now updating the file on disk");
   return GITEupdateOwnersFile(Username, pResult, pError);
}

void GITEgitlabGeneralApiCall(const COLstring& Endpoint, const GSVconfig& Auth, COLvar* pResult, COLstring* pError) {
   COL_FUNCTION(GITEgitlabGeneralApiCall);
   WEBrequest Request;
   GITEgitlabPrepare(Endpoint, Auth, &Request);
   WEBresponse Response;
   WEBrequestBlockingGet(Request, &Response);
   COLvar Data;
   if (!Data.parse(Response.Body)) { 
      COL_TRC("Failed to parse response as json.");
      *pError = "Failed to parse response from Gitlab.";
      *pResult = Response.Body;
   } else {
      COL_VAR(Data.json(1));
      *pResult = Data;
   }
}

static bool GITEgitlabParseCheckRepoResponse(const WEBresponse& Response, COLvar* pResult, COLstring* pError) {
   COL_FUNCTION(GITEgitlabParseCheckRepoResponse);
   COLvar Data, ResponseJson;
   ResponseJson.parse(Response.Body);
   COL_VAR(ResponseJson);
   int Code = Response.ReturnCode;
   if(Code == 401)                { *pError = "Invalid credentials";                       return false; }
   if(Code == 200)                { 
      Data["slug_exists"] = true;
      Data["repo_link"] = ResponseJson["web_url"];
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
   } else {
      *pError = ResponseJson;
   }
   return false;
}

bool GITEgitlabCheckRepo(const COLstring& Owner, const COLstring& Slug, const GSVconfig& Credentials, COLvar* pResult, COLstring* pError) {
   COL_FUNCTION(GITEgitlabCheckRepo);
   COLstring UrlEncoded = Owner + "/" + Slug;
   UrlEncoded = SFIuriEscape(UrlEncoded);
   WEBrequest Request;
   GITEgitlabPrepare("/projects/" + UrlEncoded, Credentials, &Request);
   WEBresponse Response;
   WEBrequestBlockingGet(Request, &Response);
   return GITEgitlabParseCheckRepoResponse(Response, pResult, pError);
}

static bool GITEgitlabGroupAndIdExtract(const COLvar& Data, COLvar* pResult, COLstring* pError){
   COL_FUNCTION(GITEgitlabGroupAndIdExtract);
   pResult->setMapType();
   const COLvar& Values = Data;
   if (Values.size() == 0) { return true; }
   if(!Values.isArray()) { 
      *pError = "Failed to parse Gitlab output"; 
      return false; 
   }
   for (const auto& Group : Values.array()){
      if (!Group.exists("full_path") || !Group.exists("id")) { continue; }
      COL_VAR2(Group["full_path"], Group["id"]);
      (*pResult)[Group["full_path"].asString()] = Group["id"];
   }
   return true;
}

static bool GITEgitlabSplitOutRepo(const COLstring& Repository, COLstring* pRepoName, COLstring* pRest){
   COL_FUNCTION(GITEgitlabSplitOutRepo);
   return Repository.rsplit(*pRest, *pRepoName, "/");
}

static int GITEgitlabDetermineNamespace(const COLvar& WebResult, const COLstring& Owner, const COLstring& Repository){
   COL_FUNCTION(GITEgitlabDetermineNamespace);
   int NameSpaceId = 0;
   COLstring Lookup = Owner;
   if (!Repository.is_null()){ Lookup = Lookup + "/" + Repository; }
   COL_TRC("Looking for " << Lookup);
   if (WebResult.exists(Lookup)){ 
      NameSpaceId = WebResult[Lookup]; 
   }
   return NameSpaceId;
}

void GITEgitlabRepoNew(const GSVconfig& Auth, const COLstring& Owner, const COLstring& Repository) {
   COL_FUNCTION(GITEgitlabRepoNew);
   COLvar Groups, GroupMap;
   COLstring Error;
   if (!GITEgitlabGetGroups(Auth, &Groups, &Error)) {
      throw COLerror("Failed to get groups from Gitlab");
   }
   if (!GITEgitlabGroupAndIdExtract(Groups, &GroupMap, &Error)){
      throw COLerror("Failed to parse groups from Gitlab");
   }
   COLstring RepoName, RestOfPath;
   GITEgitlabSplitOutRepo(Repository, &RepoName, &RestOfPath);
   int NameSpaceId = GITEgitlabDetermineNamespace(GroupMap, Owner, RestOfPath);
   if (!RestOfPath.is_null() && NameSpaceId == 0) {
      throw COLerror("Cannot create a repo using this path. Please make sure that any groups and subgroups in this path exist and you have permission to create a new project in them.");
   }
   COL_VAR2(Owner, NameSpaceId);
   WEBrequest Request;
   GITEgitlabPrepare("/projects", Auth, &Request);
   COLvar Body;
   Body["path"] = RepoName;
   if (NameSpaceId != 0) {
      Body["namespace_id"] = NameSpaceId; // This allows for creating the repository in a specific group, subgroup, etc.
   }
   Body["visibility"] = "private";
   Request.Body = Body.json();
   WEBresponse Response;
   WEBrequestBlockingPost(Request, &Response);
   COL_VAR(Response);
   if(Response.ReturnCode != 201) { 
      throw COLerror("Failed to create new repository: " + Response.Body);
   }
}

bool GITEgitlabCheckSshAccess(const COLstring& IguanaUser, const GSVconfig& Auth, COLstring* pError){
   COL_FUNCTION(GITEgitlabCheckSshAccess);
   COLstring Guid = GUIDnewTinyId();
   COLstring SshUrl;
   GITUmakeSshUrl(Auth.m_ServerUrl, "testSshConnection" + Guid, "IguanaXsshTest", &SshUrl);
   COLstring Out;
   COL_TRC("Will try to do a dry-run push to " + SshUrl);
   int ExitCode = -1;
   GITUpermissionWrite(IguanaUser, SshUrl, GITuserScratchDir(IguanaUser, Guid), &ExitCode, &Out);
   COL_VAR2(ExitCode, Out);
   if (ExitCode == 128 || ExitCode == 0){
      if (Out.find("Permission denied") != npos){
         COL_TRC("Public key authentication error - no ssh access to Gitlab.");
         *pError = "Public key authentication failed - no ssh access to Gitlab";
         return false;
      }
      COL_TRC("Did not see public key error so ssh access is good");
      return true;
   }
   return false;
}