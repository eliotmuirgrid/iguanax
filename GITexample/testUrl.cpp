// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testUrl
//
// Description:
//
// Implementation of tests
//
// Author: Eliot Muir 
// Date:   Saturday 25 March 2023 - 09:45AM
// ---------------------------------------------------------------------------
#include "testUrl.h"

#include <COL/COLlog.h>
#include <GITU/GITUurl.h>
#include <GSV/GSVlink.h>
#include <GSV/GSVmap.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

static void testSimple(){
   COL_FUNCTION(testSimple);
   COLstring Error;
   COLstring Input = "https://bitbucket.org/interfaceware/hl7-viewer.git";
   GITUurl Url;
   Url.Host = "bitbucket.org";
   Url.Repository = "interfaceware/hl7-viewer";
   COLstring SshUrl;
   GITUsshUrl(Input, &SshUrl);
   COLstring HttpsUrl;
   GITUhttpxUrl(Input, &HttpsUrl);
   COL_VAR2(SshUrl, HttpsUrl);
   GITUurl Url2;
	GITUparseUri(HttpsUrl, &Url2, &Error);
   COL_VAR2(Url2.Host, Url2.Repository);
   UNIT_ASSERT_EQUALS(Url.Host   , Url2.Host);
   UNIT_ASSERT_EQUALS(Url.Repository   , Url2.Repository);
   GITUurl Url3;
	GITUparseUri(SshUrl, &Url3, &Error);
   COL_VAR2(Url3.Host, Url3.Repository);
   UNIT_ASSERT_EQUALS(Url.Host   , Url3.Host);
   UNIT_ASSERT_EQUALS(Url.Repository   , Url3.Repository);   
}

static void testSimpleAzure(){
   COL_FUNCTION(testSimpleAzure);
   GITUurl Url;
   COLstring Error;
   COLstring Input = "https://dev.azure.com/interfaceware/hl7/_git/hl7-viewer";
   Url.Host = "dev.azure.com";
   Url.Repository = "interfaceware/hl7/hl7-viewer";

   COLstring SshUrl;
   GITUsshUrl(Input, &SshUrl);
   COLstring HttpsUrl;
   GITUhttpxUrl(Input, &HttpsUrl);
   COL_VAR2(SshUrl, HttpsUrl);
   GITUurl Url2;
	GITUparseUri(HttpsUrl, &Url2, &Error);
   COL_VAR2(Url2.Host, Url2.Repository);
   // Note that the 'host' field may or may not contain 'ssh.'. the GITparse, GIThttpsUrl and GITUsshUrl will format this for you.
   UNIT_ASSERT_EQUALS(Url.Repository   , Url2.Repository);
   GITUurl Url3;
	GITUparseUri(SshUrl, &Url3, &Error);
   COL_VAR2(Url3.Host, Url3.Repository);
   UNIT_ASSERT_EQUALS(Url.Repository   , Url3.Repository);   
}

static void testTransform(){
   COL_FUNCTION(testTransform);
   COLstring Answer = "ssh://git@bitbucket.org/interfaceware/file_writer.git"; 
   COLstring Url1 = "git@bitbucket.org:interfaceware/file_writer.git";
   COLstring HUrl;
   GITUhttpxUrl(Url1, &HUrl);
   COLstring Url2;
   GITUsshUrl(HUrl, &Url2);
   COL_VAR3(Url1, Url2, HUrl);
   UNIT_ASSERT_EQUALS(Answer, Url2);
}

static void testTransformAzure(){
   COL_FUNCTION(testTransformAzure);
   COLstring Answer = "ssh://git@ssh.dev.azure.com:v3/interfaceware/fil/file_writer"; 
   COLstring Url1 = "git@ssh.dev.azure.com:v3/interfaceware/fil/file_writer";
   COLstring HUrl;
   GITUhttpxUrl(Url1, &HUrl);
   COLstring Url2;
   GITUsshUrl(HUrl, &Url2);
   COL_VAR3(Url1, Url2, HUrl);
   UNIT_ASSERT_EQUALS(Answer, Url2);
}

static bool testParseHelper(const COLstring& TestCase){
   COL_FUNCTION(testParseHelper);
   GITUurl ParsedUrl;
   COLstring Error;
   return GITUparseUri(TestCase, &ParsedUrl, &Error);
}

static void testParse(){
   COL_FUNCTION(testParse);
   // SSH style
   const COLstring BitBucket                = "git@bitbucket.org:interfaceware/imapfeeder.git";
   const COLstring BitBucketNoGit           = "git@bitbucket.org:interfaceware/imapfeeder";
   const COLstring SshTrailingSlash         = "git@bitbucket.org:interfaceware/imapfeeder/";
   const COLstring InvalidBitBucket         = "git@bitbucket.org:imapfeeder.git"; // missing workspace
   // HTTPS style
   const COLstring BitBucketHttps           = "https://bitbucket.org/interfaceware/imapfeeder.git";
   const COLstring BitBucketHttpsNoGit      = "https://bitbucket.org/interfaceware/imapfeeder";
   const COLstring HttpsNoGitTrailingSlash  = "https://bitbucket.org/interfaceware/imapfeeder/";
   const COLstring BitBucketHttpsWithUser   = "https://vismay_shah@bitbucket.org/interfaceware/imapfeeder.git";
   const COLstring GithubHttps              = "https://github.com/interfaceware/iguana-apps.git";
   const COLstring GithubHttpsNoGit         = "https://github.com/interfaceware/iguana-apps";
   const COLstring InvalidGithubHttps       = "https://github.com/iguana-apps";
   // Really odd one that was causing crashing
   const COLstring OddOne                   = "https:////.git"; 
   
   COL_TRC("testing parsing of SSH style urls");
   UNIT_ASSERT( testParseHelper(BitBucket));
   UNIT_ASSERT( testParseHelper(BitBucketNoGit));
   UNIT_ASSERT( testParseHelper(SshTrailingSlash));
   UNIT_ASSERT(!testParseHelper(InvalidBitBucket));
   COL_TRC("testing parsing of HTTPS style urls");
   UNIT_ASSERT( testParseHelper(BitBucketHttps));
   UNIT_ASSERT( testParseHelper(BitBucketHttpsNoGit));
   UNIT_ASSERT( testParseHelper(HttpsNoGitTrailingSlash));
   UNIT_ASSERT( testParseHelper(BitBucketHttpsWithUser));
   UNIT_ASSERT( testParseHelper(GithubHttps));
   UNIT_ASSERT( testParseHelper(GithubHttpsNoGit));
   UNIT_ASSERT(!testParseHelper(InvalidGithubHttps));
   COL_TRC("testing an odd case");
   UNIT_ASSERT(!testParseHelper(OddOne));
}

static void testParseAzure(){
   COL_FUNCTION(testParseAzure);
   // Azure has a different format: <organization>/<project>/<repo>, in Iguana we treat the combination of <organization>/<project> as the workspace. Here is a valid repository
   // SSH:   git@ssh.dev.azure.com:v3/vismayshah/Test1/foobar4

   const COLstring ValidAzure             = "git@ssh.dev.azure.com:v3/vismayshah/Test1/foobar4.git"; // this is invalid, but we can strip off the .git to be nice
   const COLstring ValidAzureNoGit        = "git@ssh.dev.azure.com:v3/vismayshah/Test1/foobar4";
   const COLstring ValidAzureExtraSlash   = "git@ssh.dev.azure.com:v3/vismayshah/Test1/foobar4/";
   const COLstring InvalidAzure           = "git@ssh.dev.azure.com:v3/vismayshah/foobar4.git"; // too few fields
   const COLstring InvalidAzure2          = "git@ssh.dev.azure.com:v3/Test1/foobar4.git";      // missing organization
   const COLstring MissingProject         = "git@ssh.dev.azure.com:v3/Organization//git";      // missing project
   const COLstring WhitespaceProject      = "git@ssh.dev.azure.com:v3/Organization/  /git";    // whitespace project
   const COLstring MissingOrganization    = "git@ssh.dev.azure.com:v3//Project/git";           // missing organization
   const COLstring MissingProjectAndOrg   = "git@ssh.dev.azure.com:v3///git";                  // missing project and organization
   const COLstring NoRepoGitExt           = "git@ssh.dev.azure.com:v3/Organization/Project/.git"; // empty repo name with .git
   const COLstring MissingV3              = "git@ssh.dev.azure.com:Organization/Project/.git"; // empty repo name with .git
   
   COL_TRC("testing parsing of SSH style urls");
   UNIT_ASSERT( testParseHelper(ValidAzure));
   UNIT_ASSERT( testParseHelper(ValidAzureNoGit));
   UNIT_ASSERT( testParseHelper(ValidAzureExtraSlash));
   UNIT_ASSERT(!testParseHelper(InvalidAzure));
   UNIT_ASSERT(!testParseHelper(InvalidAzure2));
   UNIT_ASSERT(!testParseHelper(MissingProject));
   UNIT_ASSERT(!testParseHelper(WhitespaceProject));
   UNIT_ASSERT(!testParseHelper(MissingOrganization));
   UNIT_ASSERT(!testParseHelper(MissingProjectAndOrg));
   UNIT_ASSERT(!testParseHelper(NoRepoGitExt));   
   UNIT_ASSERT(!testParseHelper(MissingV3));   
}

static void testParseAzureHttps(){
   COL_FUNCTION(testParseAzureHttps);
   // Valid repository:
   // HTTPS: https://vismayshah@dev.azure.com/vismayshah/Test1/_git/foobar4  note the addition of _git, and the lack of .git at the end.

   const COLstring ValidAzureHttps          = "https://vismayshah@dev.azure.com/vismayshah/Test1/_git/foobar4";
   const COLstring AzureHttpsWithGitExt     = "https://vismayshah@dev.azure.com/vismayshah/Test1/_git/foobar4.git";
   const COLstring HttpsNoGitTrailingSlash  = "https://vismayshah@dev.azure.com/vismayshah/Test1/_git/foobar4/";

   const COLstring ValidAzureHttpsNoUser    = "https://dev.azure.com/vismayshah/Test1/_git/foobar4";
   const COLstring NoUserHttpsWithGitExt    = "https://dev.azure.com/vismayshah/Test1/_git/foobar4.git";
   const COLstring HttpsNoUserGitTrailingSlash  = "https://dev.azure.com/vismayshah/Test1/_git/foobar4/";

   const COLstring InvalidAzureHttps        = "https://dev.azure.com/vismayshah/Test1/foobar4/";     // missing _git
   const COLstring InvalidAzureHttps2       = "https://dev.azure.com/Test1/_git/foobar4/";                // too few fields
   const COLstring InvalidAzureHttps3       = "https://dev.azure.com/vismayshah/Test1/_git/foobar4/asd/"; // too many fields

   const COLstring NoProjectHttps              = "https://dev.azure.com/vismayshah/_git/foobar4";
   const COLstring NoOrganizationHttps         = "https://dev.azure.com/Test1/_git/foobar4";
   const COLstring NoRepoHttps                 = "https://dev.azure.com/vismayshah/Test1/_git/";

   GITUurl ParsedUrl;
   COLstring Error;
   COL_TRC("testing parsing of HTTPS style urls");
   UNIT_ASSERT(GITUparseUri(ValidAzureHttps, &ParsedUrl, &Error));
   UNIT_ASSERT(GITUparseUri(AzureHttpsWithGitExt, &ParsedUrl, &Error));
   UNIT_ASSERT(GITUparseUri(HttpsNoGitTrailingSlash, &ParsedUrl, &Error));
   UNIT_ASSERT(GITUparseUri(ValidAzureHttpsNoUser, &ParsedUrl, &Error));
   UNIT_ASSERT(GITUparseUri(NoUserHttpsWithGitExt, &ParsedUrl, &Error));
   UNIT_ASSERT(GITUparseUri(HttpsNoUserGitTrailingSlash, &ParsedUrl, &Error));
   UNIT_ASSERT(!GITUparseUri(InvalidAzureHttps, &ParsedUrl, &Error));
   UNIT_ASSERT(!GITUparseUri(InvalidAzureHttps2, &ParsedUrl, &Error));
   UNIT_ASSERT(!GITUparseUri(InvalidAzureHttps3, &ParsedUrl, &Error));
   UNIT_ASSERT(!GITUparseUri(NoProjectHttps, &ParsedUrl, &Error));
   UNIT_ASSERT(!GITUparseUri(NoOrganizationHttps, &ParsedUrl, &Error));
   UNIT_ASSERT(!GITUparseUri(NoRepoHttps, &ParsedUrl, &Error));
}

static void testParseAzureLocal() {
	COL_FUNCTION(testParseAzureLocal);
	COLstring LocalHost = "http://badhost/DefaultCollection/IguanaX/_git/test";
	COLstring LocalSsh = "ssh://badhost:22/DefaultCollection/IguanaX/_git/test";
   COLstring LocalSshNotInMap = "ssh://git@badhost/DefaultCollection/IguanaX/_git/test.git";
	GITUurl Url;
   COLstring Error;
	GITUparseUri(LocalSsh, &Url, &Error);
	UNIT_ASSERT_EQUALS("badhost", Url.Host);
	UNIT_ASSERT_EQUALS("DefaultCollection/IguanaX/_git/test", Url.Repository); // Here we keep the _git since we do not know that the host is an Azure server
	COLstring HUrl, Url2;
	GITUhttpxUrl(LocalHost, &HUrl);
	GITUsshUrl(HUrl, &Url2);
	UNIT_ASSERT_EQUALS(LocalSshNotInMap, Url2);

   GSVmapAdd("12.222.2.222", GITUserver::GIT_azuredev_server, false, 0, 0); // add custom localhost server
   LocalHost = "http://12.222.2.222/DefaultCollection/IguanaX/_git/test";
   LocalSsh = "ssh://12.222.2.222:22/DefaultCollection/IguanaX/_git/test";
   COLstring LocalSshAnswer = "ssh://git@12.222.2.222:22/DefaultCollection/IguanaX/_git/test";
	GITUparseUri(LocalSsh, &Url, &Error);
	UNIT_ASSERT_EQUALS("12.222.2.222", Url.Host);
	UNIT_ASSERT_EQUALS("DefaultCollection/IguanaX/test", Url.Repository); // If we know the host is azure, we strip the _git 
	GITUhttpxUrl(LocalHost, &HUrl);
	GITUsshUrl(HUrl, &Url2);
	UNIT_ASSERT_EQUALS(LocalSshAnswer, Url2);
}

static void testTranslateSshToHttps(){
   COL_FUNCTION(testTranslateSshToHttps);
   COLstring AzureSshUrl  = "git@ssh.dev.azure.com:v3/vismayshah/Test1/mycol3";
   COLstring GithubSshUrl = "git@github.com:interfaceware/imapfeeder.git";

   COLstring Answer1, Answer2;
   GITUhttpxUrl(AzureSshUrl, &Answer1);
   GITUhttpxUrl(GithubSshUrl, &Answer2);
   UNIT_ASSERT_EQUALS("https://dev.azure.com/vismayshah/Test1/_git/mycol3", Answer1);
   UNIT_ASSERT_EQUALS("https://github.com/interfaceware/imapfeeder.git",    Answer2);
}

static void testTranslateHttpsToSsh(){
   COL_FUNCTION(testTranslateHttpsToSsh);
   COLstring AzureHttpsUrl  = "https://dev.azure.com/vismayshah/Test1/_git/mycol3";
   COLstring GithubHttpsUrl = "https://github.com/interfaceware/imapfeeder.git";

   COLstring Answer1, Answer2;
   GITUsshUrl(AzureHttpsUrl, &Answer1);
   GITUsshUrl(GithubHttpsUrl, &Answer2);
   UNIT_ASSERT_EQUALS("ssh://git@ssh.dev.azure.com:v3/vismayshah/Test1/mycol3", Answer1);
   UNIT_ASSERT_EQUALS("ssh://git@github.com/interfaceware/imapfeeder.git",      Answer2);
}

static void testUrlLinks() {
	GSVlink Link;

	const COLstring AzureSshUrl = "git@ssh.dev.azure.com:v3/vismayshah/Test1/mycol3";
	GSVconstructLink(AzureSshUrl, &Link);
	UNIT_ASSERT_EQUALS("https://dev.azure.com/vismayshah/Test1/_git/mycol3", Link.m_Httpx);
	UNIT_ASSERT_EQUALS("ssh://git@ssh.dev.azure.com:v3/vismayshah/Test1/mycol3", Link.m_Ssh);
	UNIT_ASSERT_EQUALS("https://dev.azure.com/vismayshah/Test1/_git/mycol3", Link.m_Web);

	const COLstring GithubSshUrl = "git@github.com:interfaceware/imapfeeder.git";
	GSVconstructLink(GithubSshUrl, &Link);
	UNIT_ASSERT_EQUALS("https://github.com/interfaceware/imapfeeder.git", Link.m_Httpx);
	UNIT_ASSERT_EQUALS("ssh://git@github.com/interfaceware/imapfeeder.git", Link.m_Ssh);
	UNIT_ASSERT_EQUALS("https://github.com/interfaceware/imapfeeder", Link.m_Web);

	GSVmapAdd("12.222.2.222", GITUserver::GIT_azuredev_server, false, 0, 0); // add custom localhost server

	const COLstring AzureLocalHost = "http://12.222.2.222/DefaultCollection/IguanaX/_git/test";
	GSVconstructLink(AzureLocalHost, &Link);
	UNIT_ASSERT_EQUALS("http://12.222.2.222/DefaultCollection/IguanaX/_git/test", Link.m_Httpx);
	UNIT_ASSERT_EQUALS("ssh://git@12.222.2.222:22/DefaultCollection/IguanaX/_git/test", Link.m_Ssh);
	UNIT_ASSERT_EQUALS("http://12.222.2.222/DefaultCollection/IguanaX/_git/test", Link.m_Web);

	const COLstring AzureLocalSsh = "ssh://12.222.2.222:22/DefaultCollection/IguanaX/_git/test";
	GSVconstructLink(AzureLocalSsh, &Link);
	UNIT_ASSERT_EQUALS("http://12.222.2.222/DefaultCollection/IguanaX/_git/test", Link.m_Httpx);
	UNIT_ASSERT_EQUALS("ssh://git@12.222.2.222:22/DefaultCollection/IguanaX/_git/test", Link.m_Ssh);
	UNIT_ASSERT_EQUALS("http://12.222.2.222/DefaultCollection/IguanaX/_git/test", Link.m_Web);

	const COLstring GithubHttps = "https://github.com/interfaceware/iguana-apps.git";
	GSVconstructLink(GithubHttps, &Link);
	UNIT_ASSERT_EQUALS("https://github.com/interfaceware/iguana-apps.git", Link.m_Httpx);
	UNIT_ASSERT_EQUALS("ssh://git@github.com/interfaceware/iguana-apps.git", Link.m_Ssh);
	UNIT_ASSERT_EQUALS("https://github.com/interfaceware/iguana-apps", Link.m_Web);

	const COLstring GithubHttpsNoGit = "https://github.com/interfaceware/iguana-apps";
	GSVconstructLink(GithubHttpsNoGit, &Link);
	UNIT_ASSERT_EQUALS("https://github.com/interfaceware/iguana-apps.git", Link.m_Httpx);
	UNIT_ASSERT_EQUALS("ssh://git@github.com/interfaceware/iguana-apps.git", Link.m_Ssh);
	UNIT_ASSERT_EQUALS("https://github.com/interfaceware/iguana-apps", Link.m_Web);

	const COLstring IDontExist = "http://i_dont_exist/DefaultCollection/IguanaX";
	GSVconstructLink(IDontExist, &Link); // default to bitbucket form
	UNIT_ASSERT_EQUALS("https://i_dont_exist/DefaultCollection/IguanaX.git", Link.m_Httpx);
	UNIT_ASSERT_EQUALS("ssh://git@i_dont_exist/DefaultCollection/IguanaX.git", Link.m_Ssh);
	UNIT_ASSERT_EQUALS("https://i_dont_exist/DefaultCollection/IguanaX", Link.m_Web);

	GSVmapAdd("18.116.24.138", GITUserver::GIT_bitbucket_server, false, 7990, 7999); // add custom bitbucket server with its non-standard ports

   const COLstring BitbucketServerSsh = "ssh://git@18.116.24.138:7999/ig/asdf.git";
   GSVconstructLink(BitbucketServerSsh, &Link);
   UNIT_ASSERT_EQUALS("http://18.116.24.138:7990/scm/ig/asdf.git", Link.m_Httpx);
	UNIT_ASSERT_EQUALS("ssh://git@18.116.24.138:7999/ig/asdf.git", Link.m_Ssh);
	UNIT_ASSERT_EQUALS("http://18.116.24.138:7990/scm/ig/asdf", Link.m_Web);

   const COLstring BitbucketServerHttp = "http://18.116.24.138:7990/scm/ig/asdf.git";
   GSVconstructLink(BitbucketServerHttp, &Link);
   UNIT_ASSERT_EQUALS("http://18.116.24.138:7990/scm/ig/asdf.git", Link.m_Httpx);
	UNIT_ASSERT_EQUALS("ssh://git@18.116.24.138:7999/ig/asdf.git", Link.m_Ssh);
	UNIT_ASSERT_EQUALS("http://18.116.24.138:7990/scm/ig/asdf", Link.m_Web);
}

void testUrl(UNITapp* pApp) {
   pApp->add("url/simple",                &testSimple);
   pApp->add("url/simple/azure",          &testSimpleAzure);
   pApp->add("url/transform",             &testTransform);
   pApp->add("url/transform/azure",       &testTransformAzure);
   pApp->add("url/parse",                 &testParse);
   pApp->add("url/parse/azure",           &testParseAzure);
   pApp->add("url/parse/azure/https",     &testParseAzureHttps);
	pApp->add("url/parse/azure/local",     &testParseAzureLocal);
   pApp->add("url/translate/from_ssh",    &testTranslateSshToHttps);
   pApp->add("url/translate/from_https",  &testTranslateHttpsToSsh);
	pApp->add("url/links",                 &testUrlLinks);
}
