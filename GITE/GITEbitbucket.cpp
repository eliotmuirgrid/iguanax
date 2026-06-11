// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITEbitbucket
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
#include <COL/COLvarUtils.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <GIT/GITpath.h>
#include <GITU/GITUpermission.h>
#include <GITC/GITCutils.h>
#include <GITE/GITEbitbucket.h>
#include <GITE/GITEutils.h>
#include <GSV/GSVconfig.h>
#include <GSV/GSVlink.h>
#include <GSV/GSVmap.h>
#include <GUID/GUIDid.h>
#include <PASS/PASSword.h>
#include <WEB/WEBrequest.h>
#include <WEB/WEBresponse.h>
#include <SFI/SFIbase64EncodeFilter.h>
COL_LOG_MODULE;

const COLstring GITE_BITBUCKET_CLOUD_API_URL = "https://api.bitbucket.org/2.0";
const char* const GITE_BITBUCKET_TOKEN_SCOPES[] = {"write:ssh-key:bitbucket", "read:user:bitbucket", "read:ssh-key:bitbucket", "read:repository:bitbucket", "read:workspace:bitbucket", "admin:repository:bitbucket"};

static COLstring GITEbitbucketCreateServerUrl(const GSVconfig& Config, const COLstring& Path){
   COL_FUNCTION(GITEbitbucketCreateServerUrl);
   COL_TRC("Using server: " << Config.m_ServerUrl);
   COLstring Url;
   GSVconstructServerLink(Config.m_ServerUrl, &Url);
   Url.append("/rest" + Path);
   return Url;
}

static void GITEbitbucketPasswordAuth(const GSVconfig& Config, WEBrequest* pRequest) {
   COL_FUNCTION(GITEbitbucketPasswordAuth);
   pRequest->UserName = Config.m_Username;
   pRequest->Password = Config.m_Token;
}

static void GITEbitbucketTokenAuth(const GSVconfig& Config, WEBrequest* pRequest) {
   COL_FUNCTION(GITEbitbucketTokenAuth);
   COLstring Encoded;
   SFIbase64EncodeOneLine(Config.m_Email + ":" + Config.m_Token, &Encoded);
   pRequest->Headers["Authorization"] = "Basic " + Encoded;
}

static bool GITEbitbucketTokenScopesValid(const COLstring& TokenScopes, COLstring* MissingScopes){
   COL_FUNCTION(GITEbitbucketTokenScopesValid);
   COL_VAR(TokenScopes);
   for (const auto Scope : GITE_BITBUCKET_TOKEN_SCOPES) {
      if (TokenScopes.find(Scope) == npos) {
         if(!MissingScopes->is_null()){MissingScopes->append(", ");}
         MissingScopes->append(Scope);
      }
   }
   if(MissingScopes->is_null()){return true;}
   return false;
}

static void GITEbitbucketPrepare(const COLstring& Path, const GSVconfig& Config, WEBrequest* pRequest) {
   COL_FUNCTION(GITEbitbucketPrepare);
   pRequest->Url = Config.m_Local && !Config.m_ServerUrl.is_null() ? GITEbitbucketCreateServerUrl(Config, Path)
                                                                   : GITE_BITBUCKET_CLOUD_API_URL + Path;
   pRequest->Headers["Accept"]       = "application/json";
   pRequest->Headers["Content-Type"] = "application/json";
   pRequest->Timeout                 = 5000;
   if (Config.m_Local) { // For self hosted bitbucket, we rely on username:password 
      GITEbitbucketPasswordAuth(Config, pRequest); 
   } else { // app passwords  have fixed size of 36 -- api tokens vary around 200
      GITEbitbucketIsDeprecatedToken(Config.m_Token) ? GITEbitbucketPasswordAuth(Config, pRequest)
                                                     : GITEbitbucketTokenAuth(Config, pRequest);
   }
}

COLstring GITEbitbucketGetProjectKeyFromOwner(const COLstring& Owner){
   COL_FUNCTION(GITEbitbucketGetProjectKeyFromOwner);
   COLstring ProjectKey, Temp;
   Owner.split(Temp, ProjectKey, "/");
   return ProjectKey;
}

COLstring GITEbitbucketExtractErrorMessage(const WEBresponse& Response){
   COL_FUNCTION(GITEbitbucketExtractErrorMessage);
   COLvar Result;
   COL_VAR(Result.json(1));
   if (Result.parse(Response.Body)){
      if (COLvarPathExists(Result, "error/fields/key")){
         // TODO - we're assuming that this key is an array - so more perfection exists....
         COL_TRC("Returning error message from error.fields.key[0]");
         return Result["error"]["fields"]["key"][0];
      } else if (COLvarPathExists(Result, "error/message")){
         COL_TRC("Returning error message from error.message");
         return Result["error"]["message"];
      } else {
         COL_TRC("Could not find error.message so returning the body");
         return Response.Body;
      }
   } else {
      return Response.Body;
   }
}

void GITEbitbucketThrowBadResult(const WEBresponse& Response){
   COL_FUNCTION(GITEbitbucketThrowBadResult);
   if (Response.ReturnCode != 200 && Response.ReturnCode != 201){
      throw COLerror(GITEbitbucketExtractErrorMessage(Response), 0);
   }
}

void GITEbitbucketUser(const GSVconfig& Config){
   COL_FUNCTION(GITEbitbucketUser);
   WEBrequest Request;
   if (!Config.m_Local) {
      GITEbitbucketPrepare("/user", Config, &Request);
   } else {
      GITEbitbucketPrepare("/api/latest/users", Config, &Request);
   }
   WEBresponse Response;
   WEBrequestBlockingGet(Request, &Response);
   GITEbitbucketThrowBadResult(Response);
   COLvar Result;
   Result.parse(Response.Body);
   COL_VAR(Result.json(1));
}

bool GITEbitbucketCheckToken(const COLstring& Token, const GSVconfig& Credentials, int& ReturnCode, COLstring* pError){
   COL_FUNCTION(GITEbitbucketCheckToken);
   WEBrequest Request;
   if (!Credentials.m_Local) {
      GITEbitbucketPrepare("/user", Credentials, &Request);
   } else {
      // For a local server, the 'token' is just the account password
      GITEbitbucketPrepare("/api/latest/users", Credentials, &Request);
   }
   Request.Timeout = 15000;
   Request.Password = Token; // overwrite whatever's stored with the new value we want to check
   COL_VAR(Request);
   WEBresponse Response;
   WEBrequestBlockingGet(Request, &Response);
   COL_VAR(Response);
   ReturnCode = Response.ReturnCode;
   if (Response.ReturnCode == 401){ // Bitbucket's error message is blank so we set one to display
      COL_TRC("Invalid user and app password combination.");
      *pError = "Failed to authenticate with Bitbucket using this app password";
      return false;
   }
   if (Response.ReturnCode != 200){
      COL_TRC("Failed to authenticate the Bitbucket password. HTTP Code=" + COLintToString(Response.ReturnCode) + " " + Response.Body);
      *pError = Response.Body;
      return false;
   }
   COLstring MissingScopes;
   if(!GITEbitbucketTokenScopesValid(Response.Headers["x-oauth-scopes"], &MissingScopes)){
      COL_TRC("Token is missing scopes " + MissingScopes);
      *pError = MissingScopes;
   }
   return true;
}

static void GITEbitbucketHandleSshKeyResponse(const GSVconfig& Config, const WEBresponse& Response, COLstring* pError){
   if (Config.m_Local) {
      switch (Response.ReturnCode){                                                   
         case 400: *pError = "Invalid SSH key. Please try again after regenerating the key."; return;
         case 401: *pError = "Unauthorized. Please make sure that the Bitbucket username and password entered are correct."; return;
         case 404: *pError = "User does not exist.";                                       return;
         case 409: *pError = "This key has already been registered.";                      return;
         default : *pError = "Encounted an unexpected error, Error " + COLintToString(Response.ReturnCode) + ". Please contact iNTERFACEWARE for help."; return; 
      }    
   } else {
      switch (Response.ReturnCode){                                                   
         case 400: *pError = "Key or authorization token invalid.";                        return;
         case 401: *pError = "Unauthorized error: 401";                                    return;
         case 403: *pError = "User does not have the correct permissions to set the key."; return;
         case 404: *pError = "User does not exist.";                                       return;
         // Otherwise, there's some other error.
         default : *pError = GITEbitbucketExtractErrorMessage(Response);                   return; 
      }
   }
}

bool GITEbitbucketRegisterSshKey(const COLstring& PublicKey, const GSVconfig& Credentials, COLstring* pError){
   COL_FUNCTION(GITEbitbucketRegisterSshKey);
   WEBrequest Request;
   if (!Credentials.m_Local){
      GITEbitbucketPrepare("/users/" + Credentials.m_Username + "/ssh-keys", Credentials, &Request);
      COLvar Body;
      Body["key"]  = PublicKey;
      Request.Body = Body.json();   
   } else {
      GITEbitbucketPrepare("/ssh/latest/keys", Credentials, &Request);
      COLvar Body;
      Body["text"]  = PublicKey;
      Body["label"] = "From IguanaX";
      Request.Body  = Body.json();
   }
   WEBresponse Response;
   WEBrequestBlockingPost(Request, &Response);
   if (Response.ReturnCode == 201){ return true; } // Bitbucket's api documentation says it will return 201 on success
   GITEbitbucketHandleSshKeyResponse(Credentials, Response, pError);
   return false;
}

void GITEbitbucketRepoNew(const GSVconfig& Config, const COLstring& Owner, const COLstring& Repository){
   COL_FUNCTION(GITEbitbucketRepoNew); 
   WEBrequest Request;
   if (!Config.m_Local){
      GITEbitbucketPrepare("/repositories/" + GITEencodeString(Owner) + "/" + Repository, Config, &Request);
      COLvar Body;
      Body["scm"]        = "git";
      Body["is_private"] = true;
      Request.Body = Body.json();
   } else {
      COLstring ProjectKey = GITEbitbucketGetProjectKeyFromOwner(Owner);
      GITEbitbucketPrepare("/api/latest/projects/" + ProjectKey + "/repos", Config, &Request);
      COLvar Body;
      Body["scmId"]      = "git";
      Body["name"]       = Repository;
      Request.Body = Body.json();
   }
   WEBresponse Response;
   WEBrequestBlockingPost(Request, &Response);
   COL_VAR(Response);
   COLvar Result;
   Result.parse(Response.Body);
   COL_VAR(Result.json(1));
   GITEbitbucketThrowBadResult(Response);
}

void GITEbitbucketRepoDelete(const GSVconfig& Config, const COLstring& Owner, const COLstring& Repository){
   COL_FUNCTION(GITEbitbucketRepoDelete);
   WEBrequest Request;
   // Hopefully slugs and projects don't have / characters
   if (!Config.m_Local){
      GITEbitbucketPrepare("/repositories/" + GITEencodeString(Owner) + "/" + Repository, Config, &Request);
   } else {
      COLstring ProjectKey = GITEbitbucketGetProjectKeyFromOwner(Owner);
      GITEbitbucketPrepare("/api/latest/projects/" + ProjectKey + "/repos/" + Repository, Config, &Request);
   }
   WEBresponse Response;
   WEBrequestBlockingDelete(Request, &Response);
   COL_VAR(Response);
   GITEbitbucketThrowBadResult(Response);
}

void GITEbitbucketRepoList(const GSVconfig& Config, const COLstring& Owner, COLmap<COLstring, COLstring>* pList){
   COL_FUNCTION(GITEbitbucketRepoList);
   pList->clear();
   WEBrequest Request;
   Request.GetVariables["pagelen"] = "100";
   Request.GetVariables["fields"]  = "values.slug";
   if (!Config.m_Local){
      GITEbitbucketPrepare("/repositories/" + GITEencodeString(Owner), Config, &Request);
   } else {
      COLstring ProjectKey = GITEbitbucketGetProjectKeyFromOwner(Owner);
      GITEbitbucketPrepare("/api/latest/repos", Config, &Request);
   }
   WEBresponse Response;
   WEBrequestBlockingGet(Request, &Response);
   COL_VAR(Response);
   GITEbitbucketThrowBadResult(Response);
   COLvar Result;
   Result.parse(Response.Body);
   COL_VAR(Result.json(1));
   COLvar& List = Result["values"];
   if(!List.isArray()) { return; }
   for (auto i = List.array().begin(); i !=List.array().end(); i++){
      COL_VAR((*i)["slug"]);
      pList->add((*i)["slug"], "");
   }
}

// For web API - can change with IguanaX
COLvar GITEconfigToWebVar(const GITEbitbucketAuth& Input){
   COL_FUNCTION(GITEconfigToWebVar);
   COLvar Config;
   Config["username"]  = Input.Username;
   return Config;
}

// For serialization to disc.
COLvar GITEconfigToFileVar(const GITEbitbucketAuth& Input){
   COL_FUNCTION(GITEconfigToFileVar);
   COLvar Config;
   COLvar& Data = Config["providers"][0];
   COLstring EncryptedToken;
   PASSencrypt(Input.Token, &EncryptedToken);
   Data["token"]     = EncryptedToken;
   Data["username"]  = Input.Username;
   Data["platform"]  = "bitbucket";
   return Config;
}

// For serialization to disc.
COLvar GITEconfigToUserFileVar(const GITEbitbucketAuth& Input){
   COL_FUNCTION(GITEconfigToUserFileVar);
   COLvar Config;
   COLstring EncryptedToken;
   PASSencrypt(Input.Token, &EncryptedToken);
   Config["token"]     = EncryptedToken;
   Config["username"]  = Input.Username;
   return Config;
}

COLostream& operator<<(COLostream& Stream, const GITEbitbucketAuth& Config){
   Stream << "GIT Host config:" << newline <<
   " username=" << Config.Username << 
   " token   =" << Config.Token;
   return Stream;
}

void GITEworkspaceExtractWorkspaces(COLarray<COLstring>* pList, const COLvar& Data){
   COL_FUNCTION(GITEworkspaceExtractWorkspaces);
   const COLvar& Values = Data["values"];
   if(!Values.isArray()) { return; }
   for (const auto& Space : Values.array()){
      COL_DBG(Space.json(1));
      if (Space.isMap() && Space.exists("workspace") && Space["workspace"].exists("slug")) {
         pList->push_back(Space["workspace"]["slug"]);
      }
   }
}

bool GITEworkspaceParse(const WEBresponse& Response, COLstring* pError, COLvar* pList){
   COL_FUNCTION(GITEworkspaceParse);
   COL_VAR2(Response.ReturnCode, Response.Body);
   if(Response.ReturnCode == 401) { 
      *pError = "Wrong credentials";
      return false;
   }
   if (!pList->parse(Response.Body)) {
      *pError = Response.Body;
      return false;
   }
   COL_VAR(pList->json(1));
   if(Response.ReturnCode == 200) { 
      return true; 
   }
   GITCextractErrorString(Response, (*pList)["error"]["message"], pError);
   return false;
}

bool GITEparseAndExtract(const WEBresponse& Response, COLvar* pWorkspaces, COLstring* pError) {
   COL_FUNCTION(GITEparseAndExtract);
   COLvar Data;
   if (!GITEworkspaceParse(Response, pError, &Data)) { return false; }
   COLarray<COLstring> List;
   GITEworkspaceExtractWorkspaces(&List, Data);
   for (const auto& Item : List) { pWorkspaces->push_back(Item); }
   COL_VAR(pWorkspaces->json(1));
   return true;
}

bool GITEparseAndExtractLocal(const WEBresponse& Response, COLvar* pWorkspaces, COLstring* pError) {
   COL_FUNCTION(GITEparseAndExtractLocal);
   COLvar Data;
   Data.parse(Response.Body);
   if (!Data.isMap() || !Data.exists("values") || !Data["values"].isArray()) { return false; }
   COL_DBG(Data.json(1));
   const COLvar& Values = Data["values"];
   for (const auto& Space : Values.array()){
      COL_DBG(Space.json(1));
      COLstring Key = Space["key"];
      pWorkspaces->push_back("scm/" + Key);
   }
   return true;
}

bool GITEbitbucketUpdateOwners(const COLstring& Username, bool Force, const GSVconfig& Credentials, COLvar* pResult, COLstring* pError){
   COL_FUNCTION(GITEbitbucketUpdateOwners);
   COLstring FileName = DIRconfigUserOwnersFile(Username);
   GITEremoveFileIfForce(Force, FileName);
   COL_TRC("Fetching the workspaces");
   WEBrequest Request;
   if (!Credentials.m_Local){ 
      // /user/permissions/workspaces was deprecated, call
      // /user/workspaces instead to list all the workspaces the user can see
      GITEbitbucketPrepare("/user/workspaces", Credentials, &Request); 
      WEBresponse Response;
      WEBrequestBlockingGet(Request, &Response);
      pResult->setArrayType();
      COL_TRC("Parsing bitbucket output");
      if(!GITEparseAndExtract(Response, pResult, pError)) { return false; }
   } else {
      // Bitbucket Server URLs are standardized, and scm is a fixed part of the URL structure. So, we only need to figure out what projects are visible to the user
      GITEbitbucketPrepare("/api/latest/projects", Credentials, &Request);
      WEBresponse Response;
      WEBrequestBlockingGet(Request, &Response);
      pResult->setArrayType();
      COL_TRC("Parsing bitbucket output");
      if(!GITEparseAndExtractLocal(Response, pResult, pError)) { return false; }
   }
   COL_TRC("Now updating the file on disk");
   return GITEupdateOwnersFile(Username, pResult, pError);
}

static bool GITEbitbucketParseCheckRepoResponse(const WEBresponse& Response, COLvar* pResult, COLstring* pError) {
   COL_FUNCTION(GITEbitbucketParseCheckRepoResponse);
   COLvar Data, ResponseJson;
   ResponseJson.parse(Response.Body);
   COL_VAR(ResponseJson);
   int Code = Response.ReturnCode;
   if(Code == 401)                { *pError = "Invalid credentials";                                            return false; }
   if(Code != 200 && Code != 404) { GITCextractErrorString(Response, ResponseJson["error"]["message"], pError); return false; }
   bool SlugExists = Code == 200;
   Data["slug_exists"] = SlugExists;
   if(SlugExists) { Data["repo_link"] = ResponseJson["links"]["html"]["href"]; }
   *pResult = Data;
   return true;
}

static bool GITEbitbucketParseLocalCheckRepoResponse(const WEBresponse& Response, COLvar* pResult, COLstring* pError) {
   COL_FUNCTION(GITEbitbucketParseLocalCheckRepoResponse);
   COLvar Data, ResponseJson;
   ResponseJson.parse(Response.Body);
   COL_VAR2(ResponseJson, Response.ReturnCode);
   int Code = Response.ReturnCode;
   if(Code == 401)                { *pError = "Invalid credentials";                                            return false; }
   if(Code != 200 && Code != 404) { GITCextractErrorString(Response, ResponseJson["error"]["message"], pError); return false; }
   Data["slug_exists"] = (ResponseJson.exists("values") && ResponseJson["values"].isArray() && ResponseJson["values"].size() != 0);
   if(Data["slug_exists"]) { Data["repo_link"] = ResponseJson["values"][0]["links"]["clone"][0]["href"]; }
   *pResult = Data;
   return true;
}

bool GITEbitbucketCheckRepo(const COLstring& Owner, const COLstring& Slug, const GSVconfig& Credentials, COLvar* pResult, COLstring* pError){
   COL_FUNCTION(GITEbitbucketCheckRepo);
   WEBrequest Request;
   if (!Credentials.m_Local){
      GITEbitbucketPrepare("/repositories/" + GITEencodeString(Owner) + "/" + Slug, Credentials, &Request);
      WEBresponse Response;
      WEBrequestBlockingGet(Request, &Response);
      return GITEbitbucketParseCheckRepoResponse(Response, pResult, pError);
   } else {
      COLstring ProjectKey = GITEbitbucketGetProjectKeyFromOwner(Owner);
      GITEbitbucketPrepare("/api/latest/repos?projectkey=" + ProjectKey + "&name=" + Slug, Credentials, &Request);
      WEBresponse Response;
      WEBrequestBlockingGet(Request, &Response);
      return GITEbitbucketParseLocalCheckRepoResponse(Response, pResult, pError);
   }
}

bool GITEbitbucketCheckSshAccess(const COLstring& IguanaUser, const COLstring& Owner, const GSVconfig& Auth, COLstring* pError){
   COL_FUNCTION(GITEbitbucketCheckSshAccess);
   COLstring SshUrl;
   GITUmakeSshUrl(Auth.m_ServerUrl, GITEencodeString(Owner), "IguanaXsshTest", &SshUrl);
   const COLstring Guid    = GUIDnewTinyId();
   const COLstring Scratch = GITuserScratchDir(IguanaUser, Guid);
   COLstring Out;
   COL_TRC("Will try to do a dry-run push to " + SshUrl);
   int ExitCode = -1;
   GITUpermissionWrite(IguanaUser, SshUrl, Scratch, &ExitCode, &Out);
   COL_VAR2(ExitCode, Out);
   if (ExitCode == 128 || ExitCode == 0){
      if (Out.find("Permission denied") != npos){
         COL_TRC("Public key authentication error - no ssh access to Bitbucket.");
         *pError = "Public key authentication failed - no ssh access to Bitbucket";
         return false;
      }
      COL_TRC("Did not see public key error so ssh access is good");
      return true;
   }
   return false;
}

bool GITEbitbucketIsDeprecatedToken(const COLstring& Token) { return Token.size() == 36; }