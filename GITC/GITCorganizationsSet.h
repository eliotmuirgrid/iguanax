#ifndef __GITC_ORGANIZATIONS_SET_H__
#define __GITC_ORGANIZATIONS_SET_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITCorganizationsSet
//
// Description:
//
// Api to save the organizations file.
//
// Author: Vismay Shah 
// Date:   Friday 13 June 2025 - 10:09AM
// ---------------------------------------------------------------------------
class COLstring;
class COLwebRequest;

bool GITCremoveOrgsFile(const COLstring& FileName, COLstring* pError);
void GITCorganizationsSet(const COLwebRequest& Request);

#endif // end of defensive include
