//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testMemory
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Wed 5 Oct 2022 17:15:21 EDT
//---------------------------------------------------------------------------
#include "testMemory.h"

#include <UNIT/UNITapp.h>

#include <SCK/SCKloop.h>

#include <LOGG/LOGGgenerator.h>
#include <SDB/SDBsystem.h>

#include <CFG/CFGconfig.h>
#include <CFG/CFGmap.h>

#include <COL/COLvar.h>
#include <COL/COLthreadPool.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// make sure LOGGenerator doesn't leak memory
static void testMemoryLeak() {
   COLthreadPool Pool;
   SCKloop Loop(&Pool);
   COLstring Name = "Component_1";
   CFGmap ComponentMap;
   CFGconfig ComponentConfig;
   ComponentConfig.CoreConfig.Name = Name;
   ComponentMap.ConfigMap.add(Name, ComponentConfig);
   SDBsystem LogSystem(&Loop, ComponentMap);
   COLvar Args;
   Args["component"] = Name;
   Args["months"]    = 1;
   Args["volume"]    = 1;
   Args["rarity"]    = 10000;
   LOGGeneratorWorker(Args, &LogSystem, &Loop);
}

void testMemory(UNITapp* pApp){
   pApp->add("log/generator/mem", &testMemoryLeak);
}