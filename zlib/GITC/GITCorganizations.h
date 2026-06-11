#ifndef __GITC_ORGANIZATIONS_H__
#define __GITC_ORGANIZATIONS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITCorganizations
//
// Description:
//
// API to read the organizations file.
//
// Author: Vismay Shah 
// Date:   Friday 13 June 2025 - 09:53AM
// ---------------------------------------------------------------------------
class COLstring;
class COLvar;
class COLwebRequest;

bool GITCorgsFileContent(const COLstring& Username, COLvar* pContent);
void GITCorganizations(const COLwebRequest& Request);

#endif // end of defensive include
