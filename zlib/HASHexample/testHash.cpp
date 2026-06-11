//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: HASHtest.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   05/04/24 1:43 PM
//  ---------------------------------------------------------------------------
#include "testHash.h"

#include <HASH/HASHutils.h>

#include <UNIT/UNITapp.h>

#include <COL/COLutils.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static COLarray<COLstring> createTags() {
   COLarray<COLstring> Tags;
   Tags.push_back("#dev");
   Tags.push_back("#prod");
   Tags.push_back("#support");
   Tags.push_back("#critical_systems");
   return Tags;
}

static void testVarToVec() {
   COL_FUNCTION(testVarToVec);
   const COLarray<COLstring> Tags = createTags();
   COLvar TagVar;
   TagVar.push_back("#dev");
   TagVar.push_back("#prod");
   TagVar.push_back("#support");
   TagVar.push_back("#critical_systems");
   COLarray<COLstring> Result;
   UNIT_ASSERT(HASHvarToArr(TagVar, &Result));
   UNIT_ASSERT(Tags.size() == Result.size());
	Result.clear();
   TagVar.push_back("dev");
   UNIT_ASSERT(HASHvarToArr(TagVar, &Result));
	UNIT_ASSERT(Tags.size() == Result.size());
}

static void testValid() {
   COL_FUNCTION(testValid);
   const COLarray<COLstring> Tags = createTags();
   for(const auto& it : Tags) { UNIT_ASSERT(HASHisValid(it)); }
   UNIT_ASSERT(!HASHisValid("dev"));
   UNIT_ASSERT(!HASHisValid("dev#"));
}

void testHash(UNITapp* pApp) {
   pApp->add("hash/valid",      &testValid);
   pApp->add("hash/serialize",  &testVarToVec);
}