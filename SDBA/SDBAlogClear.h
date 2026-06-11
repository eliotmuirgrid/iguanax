#ifndef __SDBA_LOG_CLEAR_H__
#define __SDBA_LOG_CLEAR_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBAlogClear
//
// Description:
//
// Web API to clear a log for a component.
//
// Author: Eliot Muir
// Date:   Tuesday 09 May 2023 - 04:26PM
// ---------------------------------------------------------------------------

class COLwebRequest;
class SDBsystem;

void SDBAlogClear(const COLwebRequest& Request, SDBsystem* pSystem);

#endif // end of defensive include
