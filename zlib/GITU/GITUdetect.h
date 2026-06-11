#ifndef __GITU_CLIENT_INSTALLED_H__
#define __GITU_CLIENT_INSTALLED_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUclientInstalled
//
// Description: Functions to check if git client is installed on machine
//
// Author: Vismay Shah
// Date:   Fri July 09 2021
//---------------------------------------------------------------------------
class COLwebRequest;
class COLostream;
class COLstring;

void GITUnotInstalled(COLostream& Out);
bool GITUinstalled(COLstring* pResult);
void GITUcheckGitInstall(const COLwebRequest& Request);

#endif  // end of defensive include