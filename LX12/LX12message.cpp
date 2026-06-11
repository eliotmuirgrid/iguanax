//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LX12message.cpp
//
//  Description:
//
//  Implementation
//
//  Author: Matthew Sobkowski
//  Date:   3/21/23 2:18 PM
//  ---------------------------------------------------------------------------
#include <LX12/LX12message.h>
#include <LX12/LX12node.h>

#include <EDI/EDIdata.h>
#include <EDI/EDIgrammar.h>
#include <EDI/EDIx12.h>
#include <EDI/EDIparse.h>

#include <FIL/FILutils.h>

#include <LEDI/LEDIcache.h>
#include <LEDI/LEDIutils.h>
#include <LND/LNDutils.h>

#include <LUA/LUAutils.h>
#include <LUA/LUAtable.h>
#include <LUA/LUAerror.h>
#include <LUA/LUAopen.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

int LX12newMessage(lua_State* L) {
   COL_FUNCTION(LX12newMessage);
   COLstring Name = LUAexpectedStringParam(L, 1, "name");
   COLstring Vmd  = LEDIresolveVmdFile(L);

   bool LegacyWarning;
   COLstring ConversionWarning;
   EDIcollection* pCollection = LEDIcacheGetEdiGrammar(L, Vmd, &LegacyWarning, &ConversionWarning);
   if(pCollection->Nodes.empty()) {
      LUAraiseLuaError(L, "Could not generate a message - this vmd file has no nodes");
      return 0;
   }

   COL_TRC("checking provided message name");
   if(pCollection->Nodes.count(Name) == 0) {
      COLstring List;
      EDIlistMessages(*pCollection, &List);
      LUAraiseLuaError(L, "Message name " + Name + " does not exist.  Try one of these instead: " + List);
   }

   COLref<EDIdata>  pRoot    = new EDIdata(*pCollection, Name);
   COLref<NODplace> pMessage = new LX12node(pRoot.get());
   LNDpushNode(L, pMessage.get());

   if(!LegacyWarning) { return 1; }
   COLstring Warning = "⚠️Warning: Legacy vmd file format detected. It is recommended to use the importer and convert the file to the new format for future use.";
   COL_TRC("Returned the table node and the legacy vmd file warning.");
   LUApushString(L, Warning);
   return 2; // node + warning
}
