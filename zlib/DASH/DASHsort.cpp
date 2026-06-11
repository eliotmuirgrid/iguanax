//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: DASHsort.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   10/07/25 1:57 PM
//  ---------------------------------------------------------------------------
#include <DASH/DASHsort.h>
#include <DASH/DASHlistComparison.h>
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <COL/COLlog.h>
#include <COL/COLsplit.h>
#include <COL/COLstring.h>
#include <COL/COLvarUtils.h>
COL_LOG_MODULE;

static int DASHpartition(const int start, const int end, const COLstring& Column, const bool Ascending, CFGmap* pMap,
                         const DASHcompareFunc* pFunc, COLarray<COLstring>* pResult) {
   COL_FUNCTION(DASHpartition);
   COL_TRC("Partition from " << start << " to " << end);
   COLarray<COLstring>& Result = *pResult;
   const COLstring    pivot  = Result[start];
   COL_VAR(pivot);
   int count = 0;
   for(int i = start + 1; i <= end; i++) {
      if((*pFunc)(CFGmapNode(pMap, pivot), CFGmapNode(pMap, Result[i]), Ascending, Column)) { count++; }
   }
   const int pivotIndex = start + count;
   COL_TRC("Pivot is greater than " << count << " items. Pivot index is " << pivotIndex);
   const COLvar tempPivotSwap = Result[pivotIndex];
   Result[pivotIndex]         = Result[start];
   Result[start]              = tempPivotSwap;
   int i = start, j = end;
   while(i < pivotIndex && j > pivotIndex) {
      while(i < Result.size() && i >= 0 &&
            (*pFunc)(CFGmapNode(pMap, pivot), CFGmapNode(pMap, Result[i]), Ascending, Column)) {
         i++;
      }
      COL_VAR2(Result[j], pivot);
      while(j < Result.size() && j >= 0 &&
            (*pFunc)(CFGmapNode(pMap, Result[j]), CFGmapNode(pMap, pivot), Ascending, Column)) {
         j--;
      }
      if(i < pivotIndex && j > pivotIndex) {
         const COLvar tempSwap = Result[i];
         Result[i]             = Result[j];
         Result[j]             = tempSwap;
         i++;
         j--;
      }
   }
   return pivotIndex;
}

static void DASHquickSort(const int start, const int end, const COLstring& Order, const bool Ascending, CFGmap* pMap,
                          DASHcompareFunc* pFunc, COLarray<COLstring>* pResult) {
   COL_FUNCTION(DASHquickSort);
   COL_TRC("Quicksort from " << start << " to " << end);
   if(start >= end) {
      COL_TRC("Bad range, so return");
      return;
   }
   const int p = DASHpartition(start, end, Order, Ascending, pMap, pFunc, pResult);
   DASHquickSort(start, p - 1, Order, Ascending, pMap, pFunc, pResult);
   DASHquickSort(p + 1, end, Order, Ascending, pMap, pFunc, pResult);
}

void DASHsort(const COLstring& Order, const bool Ascending, CFGmap* pMap, COLarray<COLstring>* pResult) {
   COL_FUNCTION(DASHsortComponents);
   if(pResult->size() <= 1) { return; }
   DASHcompareFunc Func = DASHgetCompareFunction(Order);
   DASHquickSort(0, pResult->size() - 1, Order, Ascending, pMap, &Func, pResult);
}