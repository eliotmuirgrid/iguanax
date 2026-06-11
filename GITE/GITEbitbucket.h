#ifndef __GITE_BITBUCKET_H__
#define __GITE_BITBUCKET_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITEbitbucket
//
// Description:
//
// This is an API to create and destroy repositories in bitbucket.
//
// Author: Eliot Muir 
// Date:   Tuesday 28 March 2023 - 11:34AM
// ---------------------------------------------------------------------------
#include <COL/COLmap.h>
#include <COL/COLstring.h>

class GSVconfig;

// TODO - delete this class and change to using GITEconfig
class GITEbitbucketAuth{
public:
   COLstring Username;
   COLstring Token;
};
// By Owner I mean git@bitbucket.org:<Owner>/<Repository>.git
void GITEbitbucketUser      (const GSVconfig& Config); // only used in unit test

// Helper function to extract out project key from Owner when doing local Bitbucket server operations
COLstring GITEbitbucketGetProjectKeyFromOwner(const COLstring& Owner);

bool GITEbitbucketCheckToken(const COLstring& Token, const GSVconfig& Credentials, int& ReturnCode, COLstring* pError);
bool GITEbitbucketCheckRepo(const COLstring& Owner, const COLstring& Slug, const GSVconfig& Config, COLvar* pResult, COLstring* pError);
bool GITEbitbucketRegisterSshKey(const COLstring& PublicKey, const GSVconfig& Config, COLstring* pError);
bool GITEbitbucketUpdateOwners(const COLstring& Username, bool Force, const GSVconfig& Config, COLvar* pResult, COLstring* pError);

// TODO - we could get a list of projects (but do we really care since we don't need this to create repos)
// TODO - New and Delete don't give any positive data back.
// Note - Delete and List are not used outside of unit tests.
void GITEbitbucketRepoNew   (const GSVconfig& Config, const COLstring& Owner, const COLstring& Repository);
void GITEbitbucketRepoDelete(const GSVconfig& Config, const COLstring& Owner, const COLstring& Repository); // only used in unit test

// TODO - my list code doesn't get all the pages - so we only get a partial list of repos.  Although we don't have a real
// use case for this API yet.
void GITEbitbucketRepoList  (const GSVconfig& Config, const COLstring& Owner, COLmap<COLstring, COLstring>* pList); // only used in unit test

COLostream& operator<<(COLostream& Stream, const GITEbitbucketAuth& Config); 

COLvar GITEconfigToWebVar(const GITEbitbucketAuth& Input);  // Can change easily for web
COLvar GITEconfigToFileVar(const GITEbitbucketAuth& Input); // For serialization to disc
COLvar GITEconfigToUserFileVar(const GITEbitbucketAuth& Input); // For serialization to disc for <user>/host.json

bool GITEbitbucketCheckSshAccess(const COLstring& IguanaUser, const COLstring& Owner, const GSVconfig& Auth, COLstring* pError);
bool GITEbitbucketIsDeprecatedToken(const COLstring& Token);

#endif // end of defensive include
