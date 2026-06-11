// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: USRsetup
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Sunday 18 September 2022 - 05:01PM
// ---------------------------------------------------------------------------

#include <USR/USRsetup.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


COLmap<COLstring, USRuser>*  USRcreate(){
   COL_FUNCTION(USRcreate);
   return new COLmap<COLstring, USRuser>();
}

void USRdestroy(COLmap<COLstring, USRuser>* * ppMap, COLmap<COLstring, PRMpermission>** ppPermissionMap){
   COL_FUNCTION(USRdestroy);
   delete *ppMap;
   *ppMap = NULL;
   *ppPermissionMap = NULL;
}
