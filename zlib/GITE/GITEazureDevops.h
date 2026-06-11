#ifndef __GITE_AZURE_DEVOPS_H__
#define __GITE_AZURE_DEVOPS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITEazureDevops
//
// Description:
//
// Interface for creating repositories and organizations in AzureDevops, as 
// well as setting up access tokens and ssh keys.
//
// Author: Vismay Shah 
// Date:   Wednesday 15 May 2024 - 10:38AM
// ---------------------------------------------------------------------------
class COLstring;
class COLvar;

class GSVconfig;

bool GITEazureCheckToken(const GSVconfig& Config, bool Secure, int& ReturnCode, COLstring* pError);

// Doesn't seem like azure allows registering an ssh key via a REST api
bool GITEazureUpdateOwners(const COLstring& Username, bool Force, const GSVconfig& Auth, bool Secure, COLvar* pResult, COLstring* pError);
bool GITEazureCheckRepo(const COLstring& Owner, const COLstring& Slug, const GSVconfig& Config, bool Secure,
								COLvar* pResult, COLstring* pError);
void GITEazureRepoNew(const GSVconfig& Config, bool Secure, const COLstring& Owner, const COLstring& Repository, COLstring* pGitUrl);

bool GITEazureCheckSshAccess(const COLstring& IguanaUser, const COLstring& Owner, const GSVconfig& Config, COLstring* pError);

#endif // end of defensive include
