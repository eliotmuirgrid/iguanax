//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ICOLmap.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   20/03/25 12:21 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <ICOL/ICOLmap.h>
COL_LOG_MODULE;

// <colName, colInfo>
static COLmap<COLstring, ICOLinfo>* s_pColumnMap = nullptr;

void ICOLinit() {
   COL_FUNCTION(ICOLinit);
   s_pColumnMap = new COLmap<COLstring, ICOLinfo>();
}

void ICOLfree() {
   COL_FUNCTION(ICOLfree);
   delete s_pColumnMap;
}

void ICOLadd(const COLstring& Name, const int Width, const int Priority, const COLstring& Title) {
   COL_FUNCTION(ICOLadd);
   COL_VAR3(Name, Width, Priority);
   if(s_pColumnMap->count(Name) == 0) {
      s_pColumnMap->add(Name, ICOLinfo(Width, Priority, Title));
   } else {
      (*s_pColumnMap)[Name] = ICOLinfo(Width, Priority, Title);
   }
}

void ICOLremove(const COLstring& Name) {
   COL_FUNCTION(ICOLremove);
   if(s_pColumnMap->count(Name) != 0) { s_pColumnMap->removeKey(Name); }
}

void ICOLmapSort(const COLmap<COLstring, ICOLinfo>& Map, COLvar* pOut) {
   COL_FUNCTION(ICOLmapSort);
   COL_VAR(Map.size());
   if(Map.empty()) { return; }

   COLarray<COLpair<COLstring, int>> SortedItems;
   for(const auto& item : Map) { SortedItems.push_back(COLpair<COLstring, int>(item.first, item.second.m_Priority)); }

   // Insertion sort
   for(size_t i = 1; i < SortedItems.size(); i++) {
      auto key = SortedItems[i];
      int  j   = i - 1;

      while(j >= 0) {
         const auto& a = SortedItems[j];
         const auto& b = key;

         bool shouldMove = false;

         // Handle -1 priority (should go last)
         if(a.second < 0 && b.second >= 0) {
            shouldMove = true;
         }
         // Compare priorities (only if neither has -1 priority or both have -1)
         else if((a.second >= 0 && b.second >= 0) || (a.second < 0 && b.second < 0)) {
            if(a.second > b.second || (a.second == b.second && a.first > b.first)) { shouldMove = true; }
         }

         if(shouldMove) {
            SortedItems[j + 1] = SortedItems[j];
            j--;
         } else {
            break;
         }
      }
      SortedItems[j + 1] = key;
   }

   for(const auto& item : SortedItems) {
      COLvar element;
      element["name"]  = item.first;
      element["width"] = Map[item.first].m_Width;
      element["title"] = Map[item.first].m_Title;
      pOut->push_back(element);
   }
}

void ICOLmapToVar(COLvar* pOut) {
   COL_FUNCTION(ICOLmapToVar);
   pOut->setArrayType();
   if(!s_pColumnMap) { return; }
   ICOLmapSort(*s_pColumnMap, pOut);
   COL_VAR(pOut->json(1));
}