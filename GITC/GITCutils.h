#ifndef __GITC_GITUTILS_H__
#define __GITC_GITUTILS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITEbitbucketAuth
//
// Description: GIT utils file for common functions in git operations
//
// Author: Matthew Sobkowski
// Date: May 16, 2022
//---------------------------------------------------------------------------
class WEBrequest;
class WEBresponse;
class COLstring;
class COLvar;

WEBrequest GITCbitbucketRequest(const COLstring& Url, const COLstring& Username, const COLstring& AppPassword);

void GITCextractErrorString(const WEBresponse& Response, const COLstring& ErrMsg, COLstring* pOut);
#endif // end of defensive include
