// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LHL7test
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 13 June 2023 - 04:13PM
// ---------------------------------------------------------------------------

#include <LHL7/LHL7test.h>

#include <LUA/LUAutils.h>

#include <COL/COLsafeCount.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


class LHL7foo{
public:
    LHL7foo() { COL_METHOD(LHL7foo::LHL7foo); COLincCount();}
    ~LHL7foo() { COL_METHOD(LHL7foo::~LHL7foo); COLdecCount();}
    
};

int LHL7test(lua_State* L){
   COL_FUNCTION(LHL7test);
   LHL7foo* pFoo = LUApushObject<LHL7foo>(L, "LHL7foo");
   return 1;
}