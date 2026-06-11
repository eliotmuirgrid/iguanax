// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNannotationNode
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Thursday 04 May 2023 - 10:37AM
// ---------------------------------------------------------------------------

#include <TRN/TRNannotationNode.h>
#include <TRN/TRNformatUntruncatedString.h>
#include <TRN/TRNfunction.h>
#include <TRN/TRNnodeAddress.h>
#include <TRN/TRNscope.h>

#include <COL/COLlog.h>
#include <NTV/NTVtreeValue.h>
COL_LOG_MODULE;

void TRNstandardOnNodeCopy(TRNfunctionCall* pCurrent, int Line, const NODaddress& Source, const NODaddress& Destination, NODaddress& LastDestination, NODaddress& LastSource){
   COL_FUNCTION(TRNstandardOnNodeCopy);
   if (TRNfunctionCallIsTargetCall(*pCurrent)){
      TRNline* pLine = TRNfunctionOnLine(pCurrent->m_pFunction,Line);
      if (pLine){
         COLstring Ann;
         COLostream Stream(Ann);
         TRNoutputTruncatedAddress(Stream, Destination.parent()->name(), LastDestination, Destination);
         Stream << COL_T(" = ");
         const NODplace& AssignedNode = TRNoutputTruncatedAddress(Stream, Source.parent()->name(), LastSource, Source);
         Stream << COL_T("<span class=\'lc\'> -- </span>");

         COLstring TreeValue;
         COLostream TVStream(TreeValue);
         NTVtreeValue(TVStream, AssignedNode, false);
         TRNformatUntruncatedString(TreeValue.c_str(), TreeValue.size(), Stream);
         TRNlineAddAnnotation(pLine, Ann);
         LastDestination = Destination;
         LastSource = Source;
      }
   }
}

void TRNstandardOnNodeCleared(TRNfunctionCall* pCurrent, int Line, const NODaddress& Destination, NODaddress& LastDestination){
   COL_FUNCTION(TRNstandardOnNodeCleared);
   if (TRNfunctionCallIsTargetCall(*pCurrent)){
      TRNline* pLine = TRNfunctionOnLine(pCurrent->m_pFunction,Line);
      if (pLine){
         COLstring Ann;
         COLostream Stream(Ann);
         TRNoutputTruncatedAddress(Stream, Destination.parent()->name(), LastDestination, Destination);
         Stream << COL_T(" cleared");

         TRNlineAddAnnotation(pLine,Ann);
         LastDestination = Destination;
      }
   }
}

// Not ideal the way we always pass a string through to here.
void TRNstandardOnAssignedChild(TRNfunctionCall* pCurrent, int Line, const NODaddress& Address, const COLstring& Value, NODaddress& LastDestination){
   COL_FUNCTION(TRNstandardOnAssignedChild);
   if (TRNfunctionCallIsTargetCall(*pCurrent)){
      TRNline* pLine = TRNfunctionOnLine(pCurrent->m_pFunction,Line);
      if (pLine){
         COLstring Ann;
         COLostream Stream(Ann);
         TRNoutputTruncatedAddress(Stream, Address.parent()->name(), LastDestination, Address);
         LastDestination = Address;
         Stream << COL_T(" = ");
         TRNformatUntruncatedString(Value.c_str(), Value.size(), Stream);
         TRNlineAddAnnotation(pLine,Ann);
      }
   }
}

void TRNmapAddressCacheClear(NODaddress* pLastDestination, NODaddress* pLastSource){
   COL_FUNCTION(TRNmapAddressCacheClear);
   *pLastDestination = NODaddress();
   *pLastSource = NODaddress();
}
