// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAinstanceLife
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 16 October 2023 - 04:29PM
// ---------------------------------------------------------------------------

#include <BLUA/BLUAinstance.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

BLUAinstance2::BLUAinstance2(CFGconfig& Config, SCKloop* pLoop)
 : m_pLoop(pLoop), m_Config(Config) {
   COL_METHOD(BLUAinstance2::BLUAinstance2);
   BLUAregisterTasks(this);
}

BLUAinstance2::~BLUAinstance2(){
   COL_METHOD(BLUAinstance2::~BLUAinstance2);

}