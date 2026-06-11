#ifndef __CRH_UPLOAD_H__
#define __CRH_UPLOAD_H__
//  ---------------------------------------------------------------------------
//   Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//   Module: CRHupload.h
//
//   Description: upload crash report when iguana fails
//   Author: Matthew Sobkowski
//   Date:   13/10/23 1:18 PM
//  ---------------------------------------------------------------------------
class COLstring;

// returns 0 if no crash report to upload
// returns -99 if not using the latest version
// returns HTTP status code otherwise
// if a crash report is uploaded, 200 is returned and pFilename is populated if provided.
int CRHuploadCrashReport(COLstring* pFilename = 0);
int CRHupload(const COLstring& Content);
COLstring CRHlastUpload();

#endif // end of defensive includ