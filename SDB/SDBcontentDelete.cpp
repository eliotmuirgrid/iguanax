// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBcontentDelete
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 15 May 2023 - 12:26PM
// ---------------------------------------------------------------------------

#include <SDB/SDBcontentDelete.h>
#include <SDB/SDBcontent.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void SDBcontentDelete(COLlist<SDBcontent*>* pList){
   COL_FUNCTION(SDBcontentDelete);
   COL_VAR(pList->size());
   while (pList->size() > 0){
      SDBcontent* pBack = pList->back();
      pList->pop_back();
      COL_VAR(pBack);
      SDBcontentDestroy(pBack);
      COL_VAR(pList->size());
   }
}