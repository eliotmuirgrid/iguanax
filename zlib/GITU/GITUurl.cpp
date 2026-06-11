// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUurl
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Saturday 25 March 2023 - 08:51AM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLsplit.h>
#include <GITU/GITUurl.h>
#include <GSV/GSVmap.h>
COL_LOG_MODULE;

// Strips off .git if found
static void GITUstripGitEnding(COLstring* pRepo){
   COL_FUNCTION(GITUstripGitEnding);
   COLstring& Repo = *pRepo;
   int Length = Repo.size();
   if (Length < 4) { return; }
   if (Repo.find(".git", Length - 4) != npos){
      COL_TRC("Found .git - removing");
      Repo = Repo.substr(0, Length - 4);
   }
}

// Strips off a single trailing / if found
static void GITUstripTrailingSlash(COLstring* pRepo){
   COL_FUNCTION(GITUstripTrailingSlash);
   COLstring& Repo = *pRepo;
   Repo = Repo.strip(COLstring::Trailing, '/');
}

static bool GITUisAnyPartEmpty(const COLarray<COLstring>& Parts){
   COL_FUNCTION(GITUisAnyPartEmpty);
   for (auto it = Parts.begin(); it != Parts.end(); it++){
      if (it->is_null() || it->isWhitespace())  { 
         COL_TRC("Found empty part - fail");
         return true; 
      }
   }
   return false;
}

static COLstring GITUmakeAzureServerSshUrl(const GITUurl& Url, const GSVitem& Item){
   COL_FUNCTION(GITUmakeAzureServerSshUrl);
   COLstring Result;
   Result = "ssh://git@" + Url.Host + ":22/" + Url.Repository;
   size_t Pos = Result.rfind("/");
   Result.insert(Pos, "/_git");
   return Result;
}

static COLstring GITUmakeAzureCloudSshUrl(const GITUurl& Url, const GSVitem& Item){
   COL_FUNCTION(GITUmakeAzureCloudSshUrl);
   COLstring Result;
   COLstring HostOut = Url.Host;
   int SshPos = -1;
   if (!HostOut.find(&SshPos, "ssh.")) { HostOut.prepend("ssh."); }
   Result = "ssh://git@" + HostOut + "/v3/" + Url.Repository;
   return Result;
}

static COLstring GITUgetSecondPart(const GITUurl& Url, const GSVitem& Item){
   COL_FUNCTION(GITUgetSecondPart);
   COLstring Result;
   if (!(Item.m_SshPort == 0 || Item.m_SshPort == 22)){
      Result = ":" + COLintToString(Item.m_SshPort);
   }  
   Result = Result + "/" + Url.Repository;
   return Result + ".git";
}


static COLstring GITUgetSecondPartHttpx(const GITUurl& Url, const GSVitem& Item){
   COL_FUNCTION(GITUgetSecondPartHttpx);
   COLstring Result;
   if (!(Item.m_HttpPort == 0 || Item.m_HttpPort == 80 || Item.m_HttpPort == 443)){
      Result = ":" + COLintToString(Item.m_HttpPort);
   }  
   Result = Result + "/" + Url.Repository;
   return Result + ".git";
}

COLstring GITUsshUrlImpl(const GITUurl& Url, const GSVitem& Item){
   COL_FUNCTION(GITUsshUrlImpl);
   COLstring Result;
   switch (Item.m_Type){
      case GIT_azuredev_cloud:
         Result = GITUmakeAzureCloudSshUrl(Url, Item);
         break;
      case GIT_azuredev_server:
         Result = GITUmakeAzureServerSshUrl(Url, Item);
         break;
      default:
         COLstring FirstPart, SecondPart;
         FirstPart  = "ssh://git@" + Url.Host;
         SecondPart = GITUgetSecondPart(Url, Item);
         Result = FirstPart + SecondPart; 
         break;
   }
   COL_VAR(Result);
   return Result;
}

static COLstring GITUmakeAzureStyleHttpUrl(const COLstring& Scheme, const GITUurl& Url, const GSVitem& Item){
   COL_FUNCTION(GITUmakeAzureStyleHttpUrl);
   COLstring Result;
   COLstring FirstPart, SecondPart;
   FirstPart = Scheme + Url.Host;
   //Should break this into a function, similar code used in 3 places
   if (!(Item.m_HttpPort == 0 || Item.m_HttpPort == 80 || Item.m_HttpPort == 443)){
         FirstPart = FirstPart + ":" + COLintToString(Item.m_HttpPort);
   }  
   // Here we have to do a split, insert _git as the second last element of the vector and then join
   int Pos = 0;
   Url.Repository.rfind(&Pos, "/");
   COLstring AzurePreGit, AzureRepo;
   Url.Repository.splitAtIndex(AzurePreGit, AzureRepo, Pos);
   Result = FirstPart + "/" + AzurePreGit  + "/_git" + AzureRepo;
   return Result;
}

COLstring GITUhttpxUrlImpl(const GITUurl& Url, const GSVitem& Item){
   COL_FUNCTION(GITUhttpxUrlImpl);
   COLstring Result;
   COLstring Scheme = Item.m_Secure ? "https://" : "http://";
   COLstring FirstPart, SecondPart;
   switch (Item.m_Type){
      case GIT_azuredev_cloud:
      case GIT_azuredev_server:
         Result = GITUmakeAzureStyleHttpUrl(Scheme, Url, Item);
         break;
      case GIT_bitbucket_server:
         FirstPart = Scheme + Url.Host;
         if (!(Item.m_HttpPort == 0 || Item.m_HttpPort == 80 || Item.m_HttpPort == 443)){
            SecondPart = ":" + COLintToString(Item.m_HttpPort);
         }  
         SecondPart = SecondPart + "/scm/" + Url.Repository + ".git";
         Result = FirstPart + SecondPart;
         break;
      default:
         FirstPart = Scheme + Url.Host;
         SecondPart = GITUgetSecondPartHttpx(Url, Item);
         Result = FirstPart + SecondPart;
         break;
   }
   COL_VAR(Result);
   return Result;
}

COLstring GITUwebUrlImpl(const GITUurl& Url, const GSVitem& Item){
   COL_FUNCTION(GITUwebUrlImpl);
   COLstring Result = GITUhttpxUrlImpl(Url, Item);
   GITUstripGitEnding(&Result);
   COL_VAR(Result);
   return Result;
}

bool GITUwebUrl(const COLstring& UrlString, COLstring *pOut){
   COL_FUNCTION(GITUwebUrl);
   if (UrlString.is_null()){
      return false;
   }
   GITUurl Url;
   COLstring Error;
   if (!GITUparseUri(UrlString, &Url, &Error)) { return false; }
   GSVitem Item = GSVmapItem(Url.Host);
   *pOut = GITUwebUrlImpl(Url, Item);
   return true;
}

bool GITUsshUrl(const COLstring& UrlString, COLstring* pOut){
   COL_FUNCTION(GITUsshUrl);
   if (UrlString.is_null()){
      return false;
   }
   GITUurl Url;
   COLstring Error;
   if (!GITUparseUri(UrlString, &Url, &Error)) { COL_VAR(Error); return false; }
   GSVitem Item = GSVmapItem(Url.Host);
   COL_VAR(GSVserviceName(Item.m_Type));
   *pOut = GITUsshUrlImpl(Url, Item);
   return true;
}

bool GITUmakeSshUrl(const COLstring& Address, const COLstring& Owner, const COLstring& Repo, COLstring* pUrl){
   COL_FUNCTION(GITUmakeSshUrl);
   COLstring SshUrl;
   SshUrl = "ssh://git@" + Address + "/" + Owner + "/" + Repo;
   return GITUsshUrl(SshUrl, pUrl);
}

bool GITUhttpxUrl(const COLstring& UrlString, COLstring* pOut){
   COL_FUNCTION(GITUhttpxUrl);
   if (UrlString.is_null()){
      return false;
   }
   GITUurl Url;
   COLstring Error;
   if (!GITUparseUri(UrlString, &Url, &Error)) { return false; }
   GSVitem Item = GSVmapItem(Url.Host); 
   *pOut = GITUhttpxUrlImpl(Url, Item);
   return true;
}

static bool GITUparseIsAzure(const GSVitem& Item){
   COL_FUNCTION(GITUparseIsAzure);
   COL_VAR(GSVserviceName(Item.m_Type));
   return (Item.m_Type == GIT_azuredev_cloud || Item.m_Type == GIT_azuredev_server);
}

bool GITUisHttpsUrl         (const COLstring& Url) { return Url.substr(0,5) == "https";}
bool GITUisHttpUrl          (const COLstring& Url) { return Url.substr(0,4) == "http" && !GITUisHttpsUrl(Url); }
bool GITUisHttpxUrl         (const COLstring& Url) { return (GITUisHttpsUrl(Url) || GITUisHttpUrl(Url)); }
bool GITUisSshUrl           (const COLstring& Url) { return Url.substr(0,4) == "git@"; }
bool GITUisAzureSshLocal    (const COLstring& Url) { return Url.substr(0,6) == "ssh://" && Url.find("_git"); }

bool GITUcleanUrl(const COLstring& Url, COLstring* pOut){
   COL_FUNCTION(GITUcleanUrl);
	if(GITUisHttpxUrl(Url) && GITUhttpxUrl(Url, pOut))                          return true;
   if((GITUisSshUrl(Url) || GITUisAzureSshLocal(Url)) && GITUsshUrl(Url, pOut)) return true;
   return false;
}

static bool GITUparseScheme(const COLstring& Uri, COLstring* pScheme, int& Cursor, COLstring* pError){
   COL_FUNCTION(GITUparseScheme);
   COLstring& Scheme = *pScheme;
   int SchemePos = 0; 
   if (Uri.find(&SchemePos, "://")) {
      Scheme = Uri.substr(Cursor, SchemePos - Cursor);
      Cursor = SchemePos + 3;
   } else {
      COL_TRC("No scheme provided, defaulting to ssh");
      Scheme = "ssh";
   }
   if (!(Scheme == "http" || Scheme == "https" || Scheme == "ssh")) {
      COL_TRC("Invalid scheme");
      *pError = "Not a valid scheme/protocol";
      return false;
   }
   return true;
}

static bool GITUisValidPort(const COLstring& PotentialPort){
   COL_FUNCTION(GITUisValidPort);
   for (int i = 0; i < PotentialPort.size(); i++) {
      if (!COLisdigit(PotentialPort[i])) { 
         return false;
      }
   }
   return true;
}

static bool GITUparseHostAndPort(const COLstring& Authority, const COLstring& Scheme, COLstring* pHost, COLstring* pPort, int& Cursor, COLstring* pError){
   COL_FUNCTION(GITUparseHostAndPort);
   int ColonPos = -1;
   COLstring& Host = *pHost;
   COLstring& Port = *pPort;
   if (Authority.find(&ColonPos, ':')){ 
      COL_TRC("Looking for host and port.");
      Host = Authority.substr(0, ColonPos);
      if (Host.size() == 0){
         *pError = "Invalid url. Host cannot be empty";
         return false; 
      }
      COLstring HostNoSsh = Host;
      if (HostNoSsh.substr(0, 4) == "ssh.") { HostNoSsh = HostNoSsh.substr(4); }
      if (GSVmapExists(HostNoSsh) && GITUparseIsAzure(GSVmapItem(HostNoSsh))){
         Host = HostNoSsh;
      }
      GSVitem Item;
      if (GSVmapExists(Host)){
         Item = GSVmapItem(Host);
         COL_TRC("Found host in map.");
      } 
      COLstring PotentialPort = Authority.substr(ColonPos + 1);
      if (GITUisValidPort(PotentialPort)) {
         COL_TRC("Found a port: " << PotentialPort);
         Port = PotentialPort;
         COL_TRC("Url has port. Check to see if it matches what's in the map");
         int MapPort = -1;
         (Scheme == "ssh") ? MapPort = Item.m_SshPort : MapPort = Item.m_HttpPort;
         if (COLintToString(MapPort) != Port) { COL_TRC("Port found in url doesn't match port from map!"); } 
      } else {
         COL_TRC("Content after : is non-numeric. Not a port.");
         Cursor = Cursor - PotentialPort.size() - 1; // Shift cursor back a bit since the : is being used to separate the host from the repository 
      }
   } else {
      Host = Authority;
      COLstring HostNoSsh = Host;
      if (HostNoSsh.substr(0, 4) == "ssh.") { HostNoSsh = HostNoSsh.substr(4); }
      if (GSVmapExists(HostNoSsh) && GITUparseIsAzure(GSVmapItem(HostNoSsh))){
         Host = HostNoSsh;
      }
      if (Host.size() == 0){
         *pError = "Invalid url. Host cannot be empty";
         return false; 
      }
   }
   return true;
}

static bool GITUparseAuthority(const COLstring& Uri, const COLstring& Scheme, COLstring* pHost, COLstring* pPort, int& Cursor, COLstring* pError){
   COL_FUNCTION(GITUparseAuthority);
   int AuthPos = -1;
   COLstring Authority;
   if (Uri.find(&AuthPos, '/', Cursor)) { // If we have a url where the : splits the host from the repo, this will return more than just the authority part of the uri. This is fixed later by moving the cursor backwards.
      Authority = Uri.substr(Cursor, AuthPos - Cursor);
      Cursor = AuthPos + 1; // we want to skip the leading / before the rest of the url, which is the repository path.
   } else {
      COL_TRC("Uri is improperly formated: " + Uri);
      *pError = "Uri is improperly formated. Could not find '/' character. Url: " + Uri;
      return false;
   }
   // Handle userinfo (username:password)
   // Right now, we just skip these. Can upgrade the parse to support these later if needed.
   int AtPos = -1;
   if (Authority.find(&AtPos, '@')){
      Authority = Authority.substr(AtPos + 1);
   }
   // Handle host and port
   if (!GITUparseHostAndPort(Authority, Scheme, pHost, pPort, Cursor, pError)) { return false; }
   return true;
}

static void GITUcleanupAzureRepoPath(COLarray<COLstring>* pParts){
   COL_FUNCTION(GITUcleanupAzureRepoPath);
   COLarray<COLstring>& Parts = *pParts;
   // Azure in http(s) form and Azure local server ssh form adds an '_git' fragment that we don't want to keep.
   int IndexOfGit = Parts.indexOf("_git");
   if (IndexOfGit != -1) { Parts.remove(IndexOfGit); }
   // Azure in ssh form adds a 'v3' fragment that we don't want to keep.
   int IndexOfV3 = Parts.indexOf("v3");
   if (IndexOfV3 != -1) { Parts.remove(IndexOfV3); }
}


static void GITUcleanupBitbucketServerRepoPath(COLarray<COLstring>* pParts){
   COL_FUNCTION(GITUcleanupBitbucketServerRepoPath);
   COLarray<COLstring>& Parts = *pParts;
   // Bitbucket server in http(s) form adds an 'scm' fragment that we don't want to keep.
   int IndexOfScm = Parts.indexOf("scm");
   if (IndexOfScm != -1) { Parts.remove(IndexOfScm); }
}

static bool GITUcheckRequiredParts(const COLarray<COLstring>& Parts, const GSVitem& Item, bool IsAzure, COLstring* pError){
   COL_FUNCTION(GITUcheckRequiredParts);
   // After removing all the fragments we do not need, repo parts should be [project, repo] or [owner, project, repo] if azure.
   int RequiredParts = 2;
   if (IsAzure) { 
      COL_TRC("Host is an Azure host");
      RequiredParts = 3;
   }
   COL_VAR(RequiredParts);
   if (Parts.size() < RequiredParts) { 
      COL_TRC("Not enough parts remaining to be a proper repo.");
      *pError = "Invalid url. Invalid format for a " + GSVserviceName(Item.m_Type) + " repository path.";
      return false;
   }
   return true;
}

static bool GITUcheckAzureCloudPath(const COLarray<COLstring>& PathParts, const COLstring& Scheme, COLstring* pError) {
   COL_FUNCTION(GITUcheckAzureCloudPath);
   if ("https" == Scheme) {
      int IndexOfGit = PathParts.indexOf("_git");
      if (-1 == IndexOfGit) {
         *pError = "Invalid url. Expecting '/_git/' in an HTTPS " + GSVserviceName(GIT_azuredev_cloud) + " repository path.";
         return false;
      }
      if (IndexOfGit != PathParts.size() - 2 ) {
         *pError = "Invalid url. Expecting '/_git/' immediately before the repository name in an HTTPS " + GSVserviceName(GIT_azuredev_cloud) + " repository path.";
         return false;
      }
   }
   if ("ssh" == Scheme) {
      int IndexOfV3 = PathParts.indexOf("v3");
      if (-1 == IndexOfV3) {
         *pError = "Invalid url. Expecting 'v3/' in an SSH " + GSVserviceName(GIT_azuredev_cloud) + " repository path.";
         return false;
      }
      if (IndexOfV3 != 0 ) {
         *pError = "Invalid url. Expecting 'v3/' at the beginning of an SSH " + GSVserviceName(GIT_azuredev_cloud) + " repository path.";
         return false;
      }
   }
   return true;
}

static bool GITUrepoCheckParts(const COLstring& Scheme, const COLstring& Host, const COLstring& Path, COLstring* pCleanPath, COLstring* pError){
   COL_FUNCTION(GITUrepoCheckParts);
   if (Path.size() == 0){
      *pError = "Invalid url. Repository path is empty.";
      return false;
   }
   COLstring Input = Path;
   COLarray<COLstring> Parts;
   GITUstripTrailingSlash(&Input);
   GITUstripGitEnding(&Input);
   COLsplit(&Parts, Input, "/");
   if (GITUisAnyPartEmpty(Parts)) {
      COL_TRC("Encountered empty part when parsing repo.");
      *pError = "Invalid url. Encountered empty part when parsing repository path.";
      return false; 
   }
   // Figure out if we have an Azure host, as that affects how to clean up the repository path. Same with bitbucket server
   GSVitem Item;
   if (GSVmapExists(Host)){
      COL_TRC("Found host in map.");
      Item = GSVmapItem(Host);
   }
   // We could add path check for other server types, or we could get rid of it altogether.
   // This additional checking conflicts with some of our generation code. Its more trouble than its worth for now.
   // if (GIT_azuredev_cloud == Item.m_Type && !GITUcheckAzureCloudPath(Parts, Scheme, pError)) { return false; }
   bool IsBitbucketLocal = Item.m_Type == GIT_bitbucket_server;
   bool IsAzure = GITUparseIsAzure(Item);
   if (!IsAzure) { // This isn't necessary outside of unit tests.
      int FoundPos = 0;
      IsAzure = Host.find(&FoundPos, "dev.azure.com");
   }
   if (IsAzure) { GITUcleanupAzureRepoPath(&Parts); }
   if (IsBitbucketLocal) { GITUcleanupBitbucketServerRepoPath(&Parts); }
   if (!GITUcheckRequiredParts(Parts, Item, IsAzure, pError)) { return false; }
   // Join the repo back together using /
   COLjoin(pCleanPath, Parts, "/");
   return true;
}

bool GITUparseUri(const COLstring& UrlIn, GITUurl* pOut, COLstring* pError){
   COL_FUNCTION(GITUparseUri);
   COL_VAR(UrlIn);
   int       Cursor = 0;
   COLstring Uri    = UrlIn;
   COLstring Scheme;
   // Pre-processing
   Uri = COLstripWhiteSpace(Uri, true, true);
   COL_VAR(Uri);
   // Scheme is optional. It must be one of ssh:// or http(s)://
   if (!GITUparseScheme(Uri, &Scheme, Cursor, pError)) { return false; }
   COL_VAR(Scheme);
   // Authority has the form username:password@host:port. Of these, need the host and port (if present).
   COLstring Host, Port;
   COL_VAR(GSVmaptoVar().json(1));
   if (!GITUparseAuthority(Uri, Scheme, &Host, &Port, Cursor, pError)) { return false; }
   COL_VAR2(Host, Port);
   pOut->Host = Host;
   // Path is whatever's left
   COLstring CleanPath;
   const COLstring Path = Uri.substr(Cursor);
   if (!GITUrepoCheckParts(Scheme, Host, Path, &CleanPath, pError)) { return false; }
   COL_VAR(CleanPath);
   pOut->Repository = CleanPath;
   return true;
}
