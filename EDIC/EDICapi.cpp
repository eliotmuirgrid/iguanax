//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Implementation
//
// Author:   Eliot Muir
// Date:     26th July 2022
//---------------------------------------------------------------------------

#include <EDIC/EDICapi.h>  // switching to using full file names since it makes it easier to move code around

#include <EDIC/EDICcomponentLoad.h>  
#include <EDIC/EDICcomponentSet.h>  

#include <COL/COLlog.h>
COL_LOG_MODULE;

void EDICapi(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap){
   COL_FUNCTION(EDICapi);
   ApiMap.add("/component/edi/set",  &EDICcomponentSet);
   ApiMap.add("/component/edi/load", &EDICcomponentLoad);
}
