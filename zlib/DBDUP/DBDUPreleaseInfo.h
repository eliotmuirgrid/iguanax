#ifndef __DBDUP_RELEASE_INFO_H__
#define __DBDUP_RELEASE_INFO_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDUPreleaseInfo
//
// Description:
//
// Checks the release server to see if a new version is available. If so, returns
// the contents of version.json for that release version.
//
// Author: Vismay Shah 
// Date:   Monday 14 August 2023 - 10:19AM
// ---------------------------------------------------------------------------
class COLwebRequest;
class COLstring;

void DBDUPreleaseInfo(const COLwebRequest& Request);
bool DBDUPcompareVersion(const COLstring& ReleaseVersion, bool* IsNewer); //exposed for unit test
#endif // end of defensive include
