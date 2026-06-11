#ifndef _GINSREMOVEUPSTREAM_H_
#define _GINSREMOVEUPSTREAM_H_
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GINSremoveUpstream
//
//  Description:
//
//  Implementation
//
//  Author: Matthew Sobkowski
//  Date:   20/02/25 10:27 AM
//  ---------------------------------------------------------------------------
class SCKloop;
class COLwebRequest;

void GINSremoveUpstream(const COLwebRequest& Request, SCKloop* pLoop);

#endif  // _GINSREMOVEUPSTREAM_H_