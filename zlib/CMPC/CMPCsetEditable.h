#ifndef __CMPCSETEDITABLE_H__
#define __CMPCSETEDITABLE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCsetEditable
//
// Description: Makes a component editable
//
// Author: Matthew Sobkowwski
// Date:   Tuesday August 8, 2023
// ---------------------------------------------------------------------------
class COLwebRequest;
class SCKloop;
struct CFGmap;

void CMPCsetEditable(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop);


#endif // end of defensive include
