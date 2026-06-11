// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LHL7message
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Friday 07 October 2022 - 08:32AM
// ---------------------------------------------------------------------------
#include <LHL7/LHL7message.h>
#include <LHL7/LHL7node.h>

#include <EDI/EDIdata.h>
#include <EDI/EDIgrammar.h>
#include <EDI/EDIhl7.h>
#include <EDI/EDIparse.h>

#include <FIL/FILutils.h>

#include <LND/LNDutils.h>
#include <LEDI/LEDIcache.h>
#include <LEDI/LEDIutils.h>

#include <LUA/LUAutils.h>
#include <LUA/LUAtable.h>
#include <LUA/LUAerror.h>
#include <LUA/LUAopen.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

int LHL7newMessage(lua_State* L) {
   COL_FUNCTION(LHL7newMessage);
   COLstring Name = LUAexpectedStringParam(L, 1, "name");
   COLstring Vmd  = LEDIresolveVmdFile(L);
   // TODO - sort out what to do with memory management of loaded vmd files.  I could leak memory initially.
   // Then figure out a better long term solution.
   bool LegacyWarning;
   COLstring ConversionWarning;
   EDIcollection* pCollection = LEDIcacheGetEdiGrammar(L, Vmd, &LegacyWarning, &ConversionWarning);
   if (pCollection->Nodes.empty()) {
      LUAraiseLuaError(L, "Could not generate a message - this vmd file has no nodes");
      return 0;
   }
   COL_TRC("Checking msh");
   if (!EDIcheckMshFirst(*pCollection)) {
      LUAraiseLuaError(L, "MSH segment is either missing or not at the start");
      return 0;
   }
   COL_TRC("checking provided message name");
   COLstring List;
   EDIlistMessages(*pCollection, &List);
   int FoundPos;
   if (pCollection->Nodes.count(Name) == 0 || !List.find(&FoundPos, Name)) { // Check that the Name is a valid Node and that it is a message definition
      LUAraiseLuaError(L, "Message name " + Name + " does not exist.  Try one of these instead: " + List);
   }
   COLref<EDIdata> pRoot = new EDIdata(*pCollection, Name);  
   COLref<NODplace> pMessage = new LHL7node(pRoot.get());
   EDIpostProcess(pRoot.get());   // Populate the HL7 delimiter characters.
   LNDpushNode(L, pMessage.get());
   if(!LegacyWarning) { return 1; }
   COLstring Warning = "⚠️Warning: Legacy vmd file format detected. It is recommended to edit this file to convert the file to the new format for future use.";
   COL_TRC("Returned the table node and the legacy vmd file warning.");
   LUApushString(L, Warning);
   return 2; // node + warning
}