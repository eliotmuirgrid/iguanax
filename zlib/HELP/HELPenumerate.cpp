//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: HELPenumerate.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   30/11/23 12:21 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <HELP/HELPenumerate.h>
#include <HELP/HELPresolve.h>
#include <HELP/HELPserialize.h>
#include <HELP/HELPmodel.h>
COL_LOG_MODULE;

static COLstring HELPresolveName(const COLstring& File, const COLstring& FunctionName) {
   COL_FUNCTION(HELPresolveName);
   try {
      COLstring Content;
      FILreadFile(File, &Content);
      HELPobject Help;
      HELPtoObject(&Help, Content);
      if(!Help.Name.is_null()) { return Help.Name; }
   }
   catch (const COLerror& E) {
      COL_TRC(E.description());
   }
   return FunctionName;
}

static void HELPprocessAndAdd(COLhashmap<COLstring, COLstring>* pMap, const COLstring& Directory,
                              const COLstring& BaseFunction, const COLstring& Filename, COLstring* pName) {
   COL_FUNCTION(HELPprocessAndAdd);
   const COLstring File         = FILpathAppend(Directory, Filename);
   const COLstring Prefix       = BaseFunction.is_null() ? COLstring() : BaseFunction + ".";
   const COLstring FunctionName = Prefix + FILpathNameWithoutLastExt(Filename);
   const COLstring ResolvedName = HELPresolveName(File, FunctionName);
   COL_VAR2(FunctionName, ResolvedName);
   if(pName && !pName->is_null()) {
      HELPreplaceMetatablePrefix(pName);
      if(*pName == FunctionName) { *pName = ResolvedName; }
      COL_VAR(*pName);
   }
   pMap->add(ResolvedName, File);
}

void HELPenumerateHelp(COLhashmap<COLstring, COLstring>* pMap, const COLstring& Directory,
                       const COLstring& BaseFunction, COLstring* pName) {
   COL_FUNCTION(HELPenumerateHelp);
   FILdirEnumerator Enum(Directory + "*", false, true);
   Enum.moveToFirst();
   COLstring Filename;
   while (Enum.getFile(Filename)) {
      if (Enum.isDirectory()) {
         COLstring NewBaseFunction = BaseFunction + (BaseFunction.is_null() ? "" : ".") + Filename;
         HELPenumerateHelp(pMap, FILpathAppend(Directory, Filename) + FIL_DIR_SEPARATOR, NewBaseFunction, pName);
      } else {
         if (FILpathExtLast(Filename, false) == "help") {
            HELPprocessAndAdd(pMap, Directory, BaseFunction, Filename, pName);
         }
      }
   }
}
