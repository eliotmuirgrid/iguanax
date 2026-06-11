#ifndef __DOC_SOURCE_QUERY_H__
#define __DOC_SOURCE_QUERY_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCsourceQuery
//
// Description:
//
// Query a component which acts as a document source.
//
// Author: Eliot Muir 
// Date:   Thursday 11 September 2025 - 10:53AM
// ---------------------------------------------------------------------------

class COLwebRequest;
class BLUAinstanceManager;

void DOCsourceQuery(const COLwebRequest& Request, BLUAinstanceManager* pManager); 

#endif // end of defensive include
