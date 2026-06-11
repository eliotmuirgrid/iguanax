#ifndef __GITSDOWNLOAD_H__
#define __GITSDOWNLOAD_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GITSdownloadImpl.h
//
//  Description: Api to download the git installer
//
//  Author: Matthew Sobkowski
//  Date:   5/3/23 10:53 AM
//  ---------------------------------------------------------------------------
class COLstring;
class COLvar;
class COLwebRequest;

bool GITSdownloadImpl(COLvar* pResult, COLstring* pError);
void GITSdownload(const COLwebRequest& Request);

#endif // end of defensive include
