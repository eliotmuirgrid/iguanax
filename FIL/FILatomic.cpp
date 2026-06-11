//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: FILatomic
//
// Description: Implementation
//
// Author: Vismay Shah
// Date:   Wed, March 3rd, 2021
//---------------------------------------------------------------------------
#include "FILatomic.h"
#include "FILfile.h"
#include "FILpathUtils.h"
#include "FILutils.h"

#include <COL/COLstring.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// giving up on file atomic logic - it's not worth the complications it creates.
// Eliot

void FILsaveAtomic(const COLstring& FileName, const COLstring& Content){
   COL_FUNCTION(FILsaveAtomic);
   FILcreateDirectoryTreeToFile(FileName, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
   FILwriteFile(FileName, Content);
}

bool FILloadAtomic(const COLstring& FileName, COLstring* pData){
   COL_FUNCTION(FILloadAtomic);
   FILreadFile(FileName, pData);
   return true; 
}

void FILserializeSaveConfig(const COLvar& Config, const COLstring& FileName){
   COL_FUNCTION(FILserializeSaveConfig);
   COLstring ConfigAsJson = Config.json(1);
   FILsaveAtomic(FileName, ConfigAsJson);
}

bool FILserializeLoadConfig(COLvar* pConfig, const COLstring& FileName){
   COL_FUNCTION(FILserializeLoadConfig);
   COL_VAR(FileName);
   if (!FILfileExists(FileName)){
      COL_TRC(FileName << " does not exist.");
      return false;
   }
   if (!FILfileReadWritable(FileName)){
      throw COLerror(FileName + "\nCannot be read and written to.  Has Iguana been run under a different user?\n"
                                "You will need to run as a different user or change the permissions of these\n"
                                "IguanaX files to readable and writable by this user.\n"
                                "Alternatively should you be running Iguana as a service?",0);
   }
   COLstring FileData;
   if (!FILloadAtomic(FileName, &FileData)) return false;
   return pConfig->parse(FileData); // true if the parsing worked, else false.
}
