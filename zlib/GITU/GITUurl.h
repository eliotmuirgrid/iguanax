#ifndef __GITU_URL_H__
#define __GITU_URL_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUurl
//
// Description:
//
// Abstraction of GIT URLs so we can treat transports independently.
//
// Author: Eliot Muir 
// Date:   Saturday 25 March 2023 - 08:51AM
// ---------------------------------------------------------------------------

#include <COL/COLstring.h>

// git@bitbucket.org:interfaceware/main2.git
// https://eliotmuir@bitbucket.org/interfaceware/main2.git
// Host=bitbucket.org, Repository=interfaceware/main2
// Some points:
//  * I ignore the username
//  * And the .git extension isn't that important.
//  * Store the full path to the repository without caring about details like project, subproject, etc.
// Open question as to whether we should store the 'type' of the URL - SSH etc.

// Azure is different enough that we need to track it. If the url was an Azure
// url when it was parsed, the url generation functions will need to generate
// azure style urls.
// https://dev.azure.com/vismayshah/Test1/_git/foobar4
// git@ssh.dev.azure.com:v3/vismayshah/Test1/foobar4
// Host=dev.azure.com, Repository=vismayshah/Test1/foobar4

// Local hosted azure urls are even more specific with how they are structured
// ex. ssh://18.218.2.204:22/DefaultCollection/IguanaX/_git/matt_test

enum GITUserver{
   GIT_invalid = -1,
	GIT_azuredev_cloud,
	GIT_azuredev_server,
	GIT_bitbucket_cloud,
	GIT_bitbucket_server,
	GIT_github_cloud,
	GIT_github_server,
	GIT_gitlab_cloud,
	GIT_gitlab_server
};

class GITUurl{
public:
   COLstring Host;
   COLstring Repository;
};

class GSVitem;

bool GITUsshUrl(const COLstring& UrlString, COLstring* pOut);
COLstring GITUsshUrlImpl(const GITUurl& Url, const GSVitem& Item); // Implementation. Use function above instead.

bool GITUmakeSshUrl(const COLstring& Address, const COLstring& Owner, const COLstring& Repo, COLstring* pUrl);

bool GITUhttpxUrl(const COLstring& Url, COLstring *pOut);
COLstring GITUhttpxUrlImpl(const GITUurl& Url, const GSVitem& Item); // Implementation. Use function above instead.

// HTTPS with no annoying .git at the end - good for logs.
bool GITUwebUrl(const COLstring& Url, COLstring *pOut);
COLstring GITUwebUrlImpl(const GITUurl& Url, const GSVitem& Item);  // Implementation. Use function above instead.

bool GITUcleanUrl(const COLstring& Url, COLstring* pOut);

bool GITUisHttpxUrl(const COLstring& Url);
bool GITUisHttpsUrl(const COLstring& Url);
bool GITUisSshUrl  (const COLstring& Url);
bool GITUisAzureSshLocal(const COLstring& Url); // Azure likes to be special

bool GITUcleanUrl(const COLstring& Url, COLstring* pOut);

// new interface:
bool GITUparseUri(const COLstring& UrlIn, GITUurl* pOut, COLstring* pError);

#endif // end of defensive include
