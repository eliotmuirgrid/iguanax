// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKonWritable
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 19 June 2023 - 04:41PM
// ---------------------------------------------------------------------------

#include <SCK/SCKonWritable.h>
#include <SCK/SCKconnection.h>
#include <SCK/SCKcallStack.h>

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

bool SCKonWritable(SCKconnection* pConnection){
   COL_FUNCTION(SCKonWritable);
   if (pConnection->m_pOnWritable.get()){
      COL_TRC("Running OnWritable runnable.");
      SCK_INC_CALLSTACK(pConnection);
      try{
         pConnection->m_pOnWritable->run(pConnection->m_Id);
      } catch (const COLerror& Error){
         COLabortWithMessage("ERROR - MAJOR PROBLEM - ON DATA WRITABLE HANDLER THREW EXCEPTION " + Error.description());
      }
      SCK_DEC_CALLSTACK(pConnection);
   }
   return true;
}
