//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ICOLcomponent.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   20/03/25 3:52 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <ICOL/ICOLcomponent.h>
COL_LOG_MODULE;

// <component, <colName, colVal>>
static COLmap<COLstring, COLmap<COLstring, COLstring>>* s_pComponentMap = nullptr;

void ICOLcomponentInit() {
   COL_FUNCTION(ICOLcomponentInit);
   s_pComponentMap = new COLmap<COLstring, COLmap<COLstring, COLstring>>();
}

void ICOLcomponentFree() {
   COL_FUNCTION(ICOLcomponentFree);
   delete s_pComponentMap;
}

void ICOLcomponentAdd(const COLstring Component, const COLstring Name, const COLstring Value) {
   COL_FUNCTION(ICOLcomponentAdd);
   if(s_pComponentMap->count(Component) == 0) { s_pComponentMap->add(Component, COLmap<COLstring, COLstring>()); }
   COLmap<COLstring, COLstring>& ComponentMap = (*s_pComponentMap)[Component];
   if(ComponentMap.count(Name) == 0) {
      ComponentMap.add(Name, Value);
   } else {
      ComponentMap[Name] = Value;
   }
}

void ICOLcomponentRemove(const COLstring Component, const COLstring Name) {
   COL_FUNCTION(ICOLcomponentRemove);
   if(s_pComponentMap->count(Component) == 0) { return; }
   COLmap<COLstring, COLstring>& ComponentMap = (*s_pComponentMap)[Component];
   if(ComponentMap.count(Name) != 0) { ComponentMap.removeKey(Name); }
}

void ICOLcomponentGet(const COLstring& Component, COLvar* pOut) {
   COL_FUNCTION(ICOLcomponentGet);
   pOut->setMapType();
   if(!s_pComponentMap || s_pComponentMap->empty() || s_pComponentMap->count(Component) == 0) { return; }
   COLmap<COLstring, COLstring>& ComponentMap = (*s_pComponentMap)[Component];
   for(const auto& it : ComponentMap) { (*pOut)[it.first] = it.second; }
   COL_VAR2(Component, pOut->json(1));
}

void ICOLcomponentToVar(COLvar* pOut) {
   COL_FUNCTION(ICOLcomponentToVar);
   pOut->setMapType();
   if(!s_pComponentMap || s_pComponentMap->empty()) { return; }
   for(const auto& it : *s_pComponentMap) { 
      (*pOut)[it.first].setMapType();
      for(const auto& Column : it.second) {
         (*pOut)[it.first][Column.first] = Column.second;
      }
   }
   COL_VAR(pOut->json(1));
}
