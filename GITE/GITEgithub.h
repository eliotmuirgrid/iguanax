#ifndef __GITE_GITHUB_H__
#define __GITE_GITHUB_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITEgithub
//
// Description:
//
// Well defined interface for creating, and checking for the existence of repositories.
//
// Author: Eliot Muir 
// Date:   Tuesday 28 March 2023 - 11:34AM
// ---------------------------------------------------------------------------
#include <COL/COLstring.h>
#include <COL/COLmap.h>

class GSVconfig;

bool GITEgithubCheckToken(const COLstring& Token, const GSVconfig& Config, int& ReturnCode, COLstring* pError);
bool GITEgithubCheckRepo (const COLstring& Owner, const COLstring& Slug, const GSVconfig& Config, COLvar* pResult, COLstring* pError);
bool GITEgithubRegisterSshKey(const COLstring& PublicKey, const GSVconfig& Config, COLstring* pError);
void GITEgithubRepoNew       (const GSVconfig& Config, const COLstring& Owner, const COLstring& Repository);
bool GITEgithubUpdateOwners  (const COLstring& Username, bool Force, const GSVconfig& Config, COLvar* pResult, COLstring* pError);

void GITEgithubRepoNewPersonal    (const GSVconfig& Config, const COLstring& Repository);
void GITEgithubRepoNewOrganization(const GSVconfig& Config, const COLstring& Owner, const COLstring& Repository);
bool GITEgithubCheckSshAccess     (const COLstring& IguanaUser, const GSVconfig& Auth, COLstring* pError);
#endif // end of defensive include
