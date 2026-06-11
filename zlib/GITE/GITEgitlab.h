#ifndef __GITEGITLAB_H__
#define __GITEGITLAB_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GITEgitlab.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   28/03/24 11:41 AM
//  ---------------------------------------------------------------------------
class GSVconfig;

bool GITEgitlabCheckToken(const COLstring& Token, const GSVconfig& Auth, int& ReturnCode, COLstring* pError);
bool GITEgitlabCheckRepo(const COLstring& Owner, const COLstring& Slug, const GSVconfig& Credentials, COLvar* pResult, COLstring* pError);
bool GITEgitlabRegisterSshKey(const COLstring& PublicKey, const GSVconfig& Config, COLstring* pError);
void GITEgitlabRepoNew(const GSVconfig& Auth, const COLstring& Owner, const COLstring& Repository);
bool GITEgitlabUpdateOwners(const COLstring& Username, bool Force, const GSVconfig& Auth, COLvar* pResult, COLstring* pError);
bool GITEgitlabCheckSshAccess(const COLstring& IguanaUser, const GSVconfig& Auth, COLstring* pError);

// General function to make a GET request to gitlab's api
void GITEgitlabGeneralApiCall(const COLstring& Endpoint, const GSVconfig& Auth, COLvar* pResult, COLstring* pError);

#endif // end of defensive include
