#ifndef __INST_DOWNLOAD_H__
#define __INST_DOWNLOAD_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTdownload
//
// Description:
//
// Routines to download the new versions of Iguana
//
// Author: Vismay Shah 
// Date:   Monday 28 August 2023 - 10:11AM
// ---------------------------------------------------------------------------
class COLstring;
class COLwebRequest;

COLstring INSTdownloadDir();
COLstring INSTdownloadFilePath(const COLstring& Version);
bool INSTdownloaded(const COLstring& Version);
bool INSTdownload(const COLstring& Version, COLstring* pError);

void INSTupgradeDownloadSpecial(const COLwebRequest& Request);
void INSTupgradeDownload(const COLwebRequest& Request);

#endif // end of defensive include