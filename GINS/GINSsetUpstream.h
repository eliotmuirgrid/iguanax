#ifndef __GINS_SET_UPSTREAM_H__
#define __GINS_SET_UPSTREAM_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GINSsetUpstream
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Tuesday 10 September 2024 - 07:18AM
// ---------------------------------------------------------------------------

class SCKloop;
class COLstring;
class COLwebRequest;

bool GINSsetUpstreamCall(const COLstring& Dir, const COLstring& GitUrl, COLstring* pSshUrl, COLstring* pError);
void GINSsetUpstream(const COLwebRequest& Request, SCKloop* pLoop);

#endif // end of defensive include
