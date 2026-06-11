// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKonConnect
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 19 June 2023 - 03:30PM
// ---------------------------------------------------------------------------

#include <SCK/SCKonConnect.h>
#include <SCK/SCKconnection.h>
#include <SCK/SCKcallStack.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

bool SCKonConnect(SCKconnection* pConnection){
   COL_FUNCTION(SCKonConnect);
   if (pConnection->m_pOnConnect.get()){
      COL_TRC("Calling OnConnect runnable.");     
      SCK_INC_CALLSTACK(pConnection);
      try{
         pConnection->m_pOnConnect->run(pConnection->m_Id);
      } catch (const COLerror& Error){
         COLabortWithMessage("ERROR - MAJOR PROBLEM - ON CONNECT HANDLER THREW EXCEPTION " + Error.description());
      }
      SCK_DEC_CALLSTACK(pConnection);
      if (pConnection->checkPendingClose()) { 
         return false; 
      }
   } else {
      COL_TRC("No connect callback set.");
   }
   return true;
}