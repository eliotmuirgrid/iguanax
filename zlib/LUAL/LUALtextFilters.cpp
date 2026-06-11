//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUALtextFilters
//
// Description:
//
// Implementation
//
// Author: Eric Mulvaney
// Date:   Monday, June 13th, 2011 @ 05:53:28 PM
//---------------------------------------------------------------------------



#include "LUALutils.h"
#include "LUALtextFilters.h"

#include <LUA/LUAutils.h>
#include <LUA/LUAerror.h>
#include <LUA/LUAcheck.h>

#include <COL/COLfilter.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


#undef ERROR
#define ERROR(X) \
   do{ COL_ERROR_STREAM_PLAIN(X, COLerror::PreCondition); }while(0)

// These objects aren't visible to users, so the name can be funky.
#undef TNDL_TEXT_FILTER_META_TABLE
#define TNDL_TEXT_FILTER_META_TABLE "textFilter"

LUALtextFilter::LUALtextFilter(COLauto<COLfilter>& pFilter) : m_pFilter(pFilter), m_Finished(false) {
   COL_METHOD(LUALtextFilter::textFilter);
   COLPRECONDITION(m_pFilter.get() && m_pFilter.isOwner());
}

static inline void pushBuffer(lua_State* L, COLstring& Buf) {
   lua_pushlstring(L, Buf.c_str(), Buf.size());
}

// This requires LUALtextFilter::reader()'s closure and args.
//
int LUALtextFilter::filterChunk(lua_State* L, int nArgs) {
   if(m_Finished) {  // Already done, just return nil.
      lua_pushnil(L);
      return 1;
   }
   COLstring Out;
   COLsinkString Sink(Out);
   m_pFilter->setNext(&Sink);
   try {
      // Get data from our reader, passing the arguments we received along.
      lua_pushvalue(L, lua_upvalueindex(1));
      for(int i=1; i<=nArgs; i++) {
         lua_pushvalue(L,i);
      }
      lua_call(L,nArgs,1);
      if(lua_isnil(L,-1)) {  // No more data; EOS might yield output.
         m_Finished = true;
         m_pFilter->onEndStream();
      } else if(lua_type(L,-1) == LUA_TSTRING) {  // Got data.
         size_t len;
         const char* s = lua_tolstring(L, -1, &len);
         if(m_pFilter->write(s, len) != len) {
            m_Finished = true;  // Treat as fatal; no retries.
            ERROR("filter failed: invalid input data");  // Probably.
         }
         m_pFilter->flush();
      } else {
         // Don't set m_Finished, they can try again.
         ERROR("reader returned non-string/non-nil value");
      }
      if(m_Finished && Out.size() == 0) {
         lua_pushnil(L);
      } else {
         pushBuffer(L, Out);
      }
   } catch(...) {
      m_pFilter->setNext(NULL);
      throw;
   }
   m_pFilter->setNext(NULL);
   // TODO: how to deal with this?
   // TNDLcheckTimeout(L);
   return 1;
}

int LUALtextFilter::reader(lua_State* L) {
   try {
      int nArgs = lua_gettop(L);  // We just pass them on.
      lua_pushvalue(L, lua_upvalueindex(2));
      LUALtextFilter* pSelf = (LUALtextFilter*)LUAcheckUdata(L, -1, TNDL_TEXT_FILTER_META_TABLE,
         "Invalid 'textFilter'");

      return pSelf->filterChunk(L, nArgs);
   } LUA_CATCH();
   return 0;
}


static void writeTo(COLsink& Sink, const char* pData, size_t Length){
   COL_FUNCTION(writeTo);
   COL_VAR2(pData, Length);
   if(Sink.write(pData, Length) != Length) {
      ERROR("filter failed: invalid input data");  // Probably.
   }
   Sink.flush();
   Sink.onEndStream();
}

int LUALapplyTextFilter(lua_State* L, int i, COLauto<COLfilter> pFilter) {
   COL_FUNCTION(applyTextFilter);
   COL_VAR(i);
   try {
      const char* Data;
      size_t DataLength;
      if (LUAextractString(L, i, &Data, &DataLength)) {
         // String case - run it through the filter and push the result onto the stack
         COLsinkString Out;
         pFilter->setNext(&Out);
         writeTo(*pFilter, Data, DataLength);
         COL_VAR(Out.string());
         LUApushString(L, Out.string());
      } else if (lua_isfunction(L, i)) {
         // Stream case - provide a closure that acts as a write into the filter
         lua_pushvalue(L,i);
         LUApushObject<LUALtextFilter>(L, pFilter, TNDL_TEXT_FILTER_META_TABLE);
         lua_pushcclosure(L, &LUALtextFilter::reader, 2);
      } else {
         luaL_argerror(L, i, "string or stream expected");
      }
      return 1;
   }  LUA_CATCH();
   return 0;
}

int LUALapplyTextFilter(lua_State* L, COLauto<COLfilter> pFilter) {
   COL_FUNCTION(applyTextFilter);
   // Implied i == 1
   LUAcheckArgCount(L, 1, -1);
   return LUALapplyTextFilter(L, 1, pFilter);
}
