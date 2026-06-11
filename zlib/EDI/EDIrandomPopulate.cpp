// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: EDIrandomPopulate
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 15 October 2024 - 02:17PM
// ---------------------------------------------------------------------------

#include <EDI/EDIrandomPopulate.h>
#include <EDI/EDIdata.h>

#include <COL/COLmath.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring EDIrandomLetter(){
   int C = 'A' + COLmath::randomNumber(25);
   COLstring V="A";
   V[0] = C;
   return V;
}

void EDIpopulateFirstNode(EDIdata* pData){
   COL_FUNCTION(EDIpopulateFirstNode);
   if (pData->isLeaf()){
      *pData = EDIrandomLetter();
      return;
   }
   EDIpopulateFirstNode(&(*pData)[0]);  
}

void EDIsegmentPopulate(EDIdata* pData){
   COL_FUNCTION(EDIsegmentPopulate);
   COLASSERT(pData->Type == EDIN_SEGMENT);
   EDIpopulateFirstNode(pData);
}

void EDIrandomPopulate(EDIdata* pData){
   //COL_FUNCTION(EDIrandomPopulate);
   if (pData->Type == EDIN_SEGMENT) {
      if (pData->isRequired() || COLmath::randomNumber(2)==1){
         EDIsegmentPopulate(&*pData);
      }
      return;
   }
   if (pData->isRepeat()){
      int Reps = COLmath::randomNumber(3);
      if (pData->isRequired()){
         Reps++;
      }
      for (int i=0; i < Reps; i++){
         EDIrandomPopulate(&(*pData)[i]);
      }
      return;
   }
   for (int i=0; i < pData->grammarSize(); i++){
      EDIrandomPopulate(&(*pData)[i]);    
   }
}