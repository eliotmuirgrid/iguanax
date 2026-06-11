#ifndef __SERVERUPLOAD_H__
#define __SERVERUPLOAD_H__
// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: serverUpload.h
//
//  Description: Contains all logic related to uploading files to release server
//
//  Author: Matthew Sobkowski
//  Date:   Thursday August 17, 2023
// ---------------------------------------------------------------------------
class COLstring;

bool checkVersionExists(const COLstring& Version);
bool changeFilePermission(const COLstring& FilePath);
bool uploadRelease(const COLstring& UploadDir);
bool uploadChangelog(const COLstring& Version, const COLstring& PathToChangelog);
bool uploadReleaseJson();
#endif // end of defensive include
