// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BROargumentPopulate
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Friday 29 November 2024 - 04:09PM
// ---------------------------------------------------------------------------

#include <BRO/BROargumentPopulate.h>

#include <BRO/BROenabled.h>

#include <TRN/TRNargumentRequest.h>

#include <COL/COLvar.h>

#include <NOD/NODplace.h>

#include <LND/LNDutils.h>

#include <LUA/LUAutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void BROpopulateNodeRecursive(const NODplace& Place, COLvar* pResult){
   COL_FUNCTION(BROpopulateNodeRecursive);
   COLvar& Result = *pResult;
   if (Place.isSimple()){
      Result["v"] = Place.value();
   } else {
      COLvar& Items = Result["v"];
      Items.setArrayType();
      for (int i = 0, n = Place.size(); i < n; i++){
         if (!Place.child(i).isNull()){
            COLvar& Item = Items.push_back();
            Item["i"]= i;
            BROpopulateNodeRecursive(Place.child(i), &Item);
         }
      }
   } 
}

void BROpopulateNodeValue(const NODplace& Place, COLvar* pResult){
   COL_FUNCTION(BROpopulateNodeValue);
   COLvar& Result = *pResult;
   Result["t"] = Place.name(); 
   Place.grammar(&Result["g"]);
   BROpopulateNodeRecursive(Place, &Result);
}

void BROpopulateNode(lua_State* L, int i, COLvar* pResult){
   COL_FUNCTION(BROpopulateNode);
   if (LNDisNode(L, i)){
      NODplace* pPlace = LNDgetNode(L, i);
      BROpopulateNodeValue(*pPlace, pResult);
   } else{
      *pResult = "userdata";
   }
}

void BROargumentPopulate(lua_State* L, int i, COLvar* pResult){
   COL_FUNCTION(BROargumentPopulate);
   COL_VAR(i);
   switch (lua_type(L, i)){
      case LUA_TSTRING:        *pResult = "string";             break;
      case LUA_TNIL:           *pResult = "nil";                break;
      case LUA_TBOOLEAN:       *pResult = "boolean";            break;
      case LUA_TFUNCTION:      *pResult = "function";           break; 
      case LUA_TNUMBER:        *pResult = "number";             break;
      case LUA_TLIGHTUSERDATA: *pResult = "lightuserdata";      break;
      case LUA_TTABLE:         *pResult = "table";              break;
      case LUA_TUSERDATA:      BROpopulateNode(L, i, pResult);  break;
      default:                 *pResult = "unknown";            break;
   }
}

void BROwriteResponses(const COLarray<TRNargRequest>& FullArgumentRequests, COLvar* pResponse){
   COL_FUNCTION(BROwriteResponses);
   COLvar FullArgResp;
   COL_VAR(FullArgumentRequests.size());
   for (int i = 0, n = FullArgumentRequests.size(); i < n; i++){
      const TRNargRequest& Request = FullArgumentRequests[i];  
      #ifdef BRO_ENABLED
         FullArgResp[Request.m_Id] = Request.m_BroResult;  // CONNECTOR BRO code.
      #endif
   }
   COL_VAR(FullArgResp.json(1));
   COL_VAR(FullArgResp.json());
   (*pResponse)["BroArgs"].swap(&FullArgResp);
}
