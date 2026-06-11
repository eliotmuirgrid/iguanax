// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKonData
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 19 June 2023 - 04:23PM
// ---------------------------------------------------------------------------

#include <SCK/SCKonData.h>
#include <SCK/SCKconnection.h>
#include <SCK/SCKcallStack.h>

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

bool SCKonData(SCKconnection* pConnection){
   COL_FUNCTION(SCKonData);
   if (pConnection->m_pOnData.get()){
      SCK_INC_CALLSTACK(pConnection);
      try{
         pConnection->m_pOnData->run(pConnection->m_Id, &pConnection->bufferIn());
      } catch (const COLerror& Error){
         COLabortWithMessage("ERROR - MAJOR PROBLEM - ON DATA HANDLER THREW EXCEPTION " + Error.description());
      }
      SCK_DEC_CALLSTACK(pConnection);
   } else {
      COL_TRC("No data callback set - just maintaining buffer");
   }
   return true;
}
