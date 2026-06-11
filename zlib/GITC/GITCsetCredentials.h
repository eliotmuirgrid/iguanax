#ifndef __GITC_SET_CREDENTIALS_H__
#define __GITC_SET_CREDENTIALS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITCsetCredentials
//
// Description: set the git credentials set in the global git config
//
// Author: Tyler B
// Date:   29 Dec 2022
//---------------------------------------------------------------------------
class COLwebRequest;
class COLstring;

void GITCsetUserEmail(const COLstring& Username, const COLstring& Name, const COLstring& Email);
void GITCsetCredentials(const COLwebRequest& Request);

#endif