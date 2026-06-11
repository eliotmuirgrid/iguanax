// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKonClose
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 19 June 2023 - 04:35PM
// ---------------------------------------------------------------------------

#include <SCK/SCKonClose.h>
#include <SCK/SCKconnection.h>
#include <SCK/SCKcallStack.h>

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

bool SCKonClose(SCKconnection* pConnection){
   COL_FUNCTION(SCKonClose);
   if (pConnection->m_pOnClose.get()){
      COL_TRC("Running OnClose callback.");
      SCK_INC_CALLSTACK(pConnection);
      try{
         pConnection->m_pOnClose->run(pConnection->m_Id);
      } catch (const COLerror& Error){
         COLabortWithMessage("ERROR - MAJOR PROBLEM - ON CLOSE HANDLER THREW EXCEPTION " + Error.description());
      }
      SCK_DEC_CALLSTACK(pConnection);
      pConnection->m_pOnClose = NULL;
   } else {
      COL_TRC("No on close handler.");
   }
   return true;
}
