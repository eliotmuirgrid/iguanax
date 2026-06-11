#ifndef __GITU_CONFIG_GLOBAL_H__
#define __GITU_CONFIG_GLOBAL_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUconfigGlobal
//
// Description:
//
// Functions to load from the global configuration for GIT - like username/email credentials
//
// Author: Eliot Muir 
// Date:   Saturday 25 March 2023 - 12:00PM
// ---------------------------------------------------------------------------

class INIdata;
class COLstring;

bool GITUconfigLoadUser(const COLstring& Username, INIdata* pData);
void GITUconfigSaveUser(const COLstring& Username, const INIdata& Data);

void GITUinitializeUsersPrivateKey(const COLstring& Username, const INIdata& Config);
INIdata GITUinitializeConfigUser(const COLstring& Username);

bool GITUgetUserEmail(const COLstring& Username, COLstring* pGitUser, COLstring* pGitEmail);

bool GITUconfigGlobalLoad(INIdata* pConfig);
void GITUconfigGlobalSave(const INIdata& Config);

COLstring GITUconfigSshPath(const INIdata& Config);
COLstring GITUconfigUserName(const INIdata& Config);
COLstring GITUconfigUserEmail   (const INIdata& Config);

void GITUconfigSetSshPath       (INIdata* pConfig, const COLstring& Path);
void GITUconfigGlobalSetUserName(INIdata* pConfig, const COLstring& Value);
void GITUconfigGlobalSetEmail   (INIdata* pConfig, const COLstring& Value);

COLstring GITUconfigPublicKey(const COLstring& PrivateKey);
COLstring GITUconfigPrivateKey(const COLstring& Username);

COLstring GITUstripPathsQuotes(const COLstring& PrivateKey);

#endif // end of defensive include
