//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TNDLxml
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Wednesday, March 9th, 2011 @ 10:12:07 AM
//
//---------------------------------------------------------------------------

#include "LXMLapi.h"

#include <NOD/NODplace.h>
#include <NOD/NODoperation.h>

#include <NOX/NOXparse.h>
#include <NOX/NOXconstant.h>

#include <LUA/LUAutils.h>
#include <LUA/LUAerror.h>


#include <LUAL/LUALutils.h>
#include <LUA/LUAtable.h>

#include <LND/LNDutils.h>

#include <COL/COLref.h>
#include <COL/COLstring.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


static const char *const LXML_OPTIONS[] = { "data", NULL };

int LXMLparseString(lua_State* L){
   COL_FUNCTION(LXMLparseString);
   LUAcheckParamsEx(L, 1, LXML_OPTIONS);
   COLstring XmlString = LUAexpectedStringParam(L, 1, "data");
   try {
      COLref<NODplace> pDocument = NOXparseString(XmlString);
      LNDpushNode(L, pDocument.get());      
      return 1; // number of results
   }
   LUA_CATCH();
}

void LXMLaddXmlNamespace(lua_State* L) {
   COL_FUNCTION(LXMLaddXmlNamespace);

   lua_newtable (L);      
   LUAaddMethod(L, "parse",      &LXMLparseString);
   LUAaddConstant(L, "ELEMENT",   NOXelementName);
   LUAaddConstant(L, "ATTRIBUTE", NOXattributeName);
   LUAaddConstant(L, "TEXT",      NOXtextName);
   LUAaddConstant(L, "CDATA",     NOXcdataName);
   lua_setglobal(L, "xml");
}
