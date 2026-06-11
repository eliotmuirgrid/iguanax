#ifndef __GITC_CREDENTIALS_H__
#define __GITC_CREDENTIALS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITCcredentials
//
// Description: get the git credentials set in the global git config
//
// Author: Tyler B
// Date:   29 Dec 2022
//---------------------------------------------------------------------------
class COLwebRequest;
class COLstring;
class COLvar;

void GITCgetUserEmail(const COLstring& Username, COLvar* pGitConfig);
void GITCcredentials(const COLwebRequest& Request);

#endif