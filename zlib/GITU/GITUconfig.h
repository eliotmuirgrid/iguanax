#ifndef __GITU_CONFIG_H__
#define __GITU_CONFIG_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUconfig
//
// Description:
//
// Many git operations can be done by simply manipulating the .git/config
// 
// Mostly likely this insulates us from differences in specific versions of
// git.  It also means we can make a few changes at once, and we do it with
// less operating system overhead.  Invoking a whole process especially on
// windows is expensive - loading a file, changing it and saving it takes less
// resources.
//
// We also avoid unreliability of processes not forking etc.  In general we'll
// have less issues overall as we put a lighter footprint on the system.
//
// Author: Eliot Muir 
// Date:   Saturday 25 March 2023 - 11:20AM
// ---------------------------------------------------------------------------

class INIdata;
class COLstring;

bool GITUconfigLoad(const COLstring& RepoDir, INIdata* pConfig, COLstring* pError);
void GITUconfigSave(const COLstring& RepoDir, const INIdata& Config);

void GITUconfigSetAutoLineEndingConversion(INIdata* pConfig);
void GITUconfigSetPullRebaseFalse(INIdata* pConfig);
void GITUconfigSetDefaultAuthor(INIdata* pConfig);
void GITUconfigSetPushAutoSetupRemote(INIdata* pConfig);

COLstring GITUconfigRemoteOriginUrl(const INIdata& Config);

void GITUconfigSubmoduleClear(INIdata* pConfig, const COLstring& SubmodulePath);

void GITUconfigRemoteClear(INIdata* pConfig, const COLstring& Branch);
void GITUconfigRemoteOriginUrlSet(INIdata* pConfig, const COLstring& Url);
void GITUconfigRemoteOriginFetchRuleSet(INIdata* pConfig);

#endif // end of defensive include
