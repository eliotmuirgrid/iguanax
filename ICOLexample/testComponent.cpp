//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2026 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: testComponent.cpp
//
//  Description: Implementation
//
//  ---------------------------------------------------------------------------
#include <COL/COLclosure.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <ICOL/ICOLcomponent.h>
#include <UNIT/UNITapp.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void addColumns(COLstring Component, int WorkerId) {
   for(int i = 0; i < 250; ++i) {
      ICOLcomponentAdd(Component, "column_" + COLintToString(i), COLintToString(WorkerId * 1000 + i));
   }
}

static void readColumns(COLstring Component) {
   for(int i = 0; i < 100; ++i) {
      COLvar Result;
      ICOLcomponentGet(Component, &Result);
   }
}

static void testConcurrentAddAndRead() {
   const COLstring ComponentAName = "component_a";
   const COLstring ComponentBName = "component_b";

   ICOLcomponentInit();

   {
      COLthreadPool Pool(1, 8);
      Pool.start();
      for(int i = 0; i < 8; ++i) {
         Pool.scheduleTask(COLnewClosure0(&addColumns, ComponentAName, i));
         Pool.scheduleTask(COLnewClosure0(&readColumns, ComponentAName));
         Pool.scheduleTask(COLnewClosure0(&addColumns, ComponentBName, i));
         Pool.scheduleTask(COLnewClosure0(&readColumns, ComponentBName));
      }
      Pool.shutdown(COLthreadPool::shutdown_after_pending_tasks);
   }

   COLvar ComponentA, ComponentB, All;
   ICOLcomponentGet(ComponentAName, &ComponentA);
   ICOLcomponentGet(ComponentBName, &ComponentB);
   ICOLcomponentToVar(&All);
   COL_VAR(All);

   UNIT_ASSERT_EQUALS(250, ComponentA.size());
   UNIT_ASSERT_EQUALS(250, ComponentB.size());
   UNIT_ASSERT(All.exists(ComponentAName));
   UNIT_ASSERT(All.exists(ComponentBName));
   UNIT_ASSERT_EQUALS(250, All[ComponentAName].size());
   UNIT_ASSERT_EQUALS(250, All[ComponentBName].size());

   ICOLcomponentFree();
}

void testComponent(UNITapp* pApp) {
   pApp->add("/component/concurrent_add_and_read", &testConcurrentAddAndRead);
}
