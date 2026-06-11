// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CANanalysis
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 15 January 2025 - 12:10PM
// ---------------------------------------------------------------------------

#include <CAN/CANanalysis.h>
#include <CAN/CANdir.h>

#include <FIL/FILdirEnumerator.h>

#include <COL/COLsplit.h>
#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

bool CANisCapitalized(const COLstring& Dir){
   COL_FUNCTION(CANisCapitalized);
   COLstring Copy = COLupperCase(Dir);
   return Copy == Dir;
}

void CANanalysis(const COLstring& RootDir){
   COL_FUNCTION(CANanalysis);
   COL_VAR(RootDir);
   FILdirEnumerator Enum(RootDir + FIL_ANYFILE, true);
   COLstring Dir;
   while (Enum.getFile(Dir)){
      if (CANisCapitalized(Dir)){
         CANdir(RootDir + Dir + "/");
      }
   }
}

void CANloadList(const COLstring& RootDir, const COLstring& ListFile){
   COL_FUNCTION(CANloadList);
   COLstring Content;
   FILreadFile(ListFile, &Content);
   COLarray<COLstring> List;
   COLsplit(&List, Content, "\n");
   COLcout << "Module count: " << List.size() << newline;
   for(int i=0; i < List.size(); i++){
      //COLcout << List[i] << newline;
      CANdir(RootDir + List[i] + "/");
   }
}