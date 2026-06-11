// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBfileRead
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 11 May 2023 - 02:54PM
// ---------------------------------------------------------------------------

#include <SDB/SDBfileRead.h>

#include <FIL/FILfile.h>

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// TODO - could make this more light weight?
bool SDBfileRead(const COLstring& FileName, COLstring* pDestination){
   COL_FUNCTION(SDBfileRead);
   try { 
      FILfile File(FileName, FILfile::Read);
      int FileSize = File.size();
      COL_VAR(FileSize);
      pDestination->setCapacity(FileSize);
      COL_VAR(pDestination->capacity());
      pDestination->setSize(FileSize);
      File.read((void*)pDestination->c_str(), FileSize);
      int FileSize2 = File.size();
      if (FileSize2 != FileSize){
         COL_TRC("Have to read extra data on this file.");
         pDestination->setCapacity(FileSize2);
         File.read((void*)(pDestination->c_str() + FileSize), FileSize2-FileSize);
      }
      File.close();
      return true; 
   } catch (const COLerror& Error){
      COL_WRN(Error.description());
      return false;
   }
}