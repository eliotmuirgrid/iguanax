//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GITverifyModulesFile.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   27/05/25 2:22 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLsplit.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GIT/GITverifyModulesFile.h>
COL_LOG_MODULE;

bool GITverifyModulesFile(const COLstring& Directory, COLstring* pErr) {
   COL_FUNCTION(GITverifyModulesFile);
   const COLstring ModulesPath = FILpathAppend(Directory, ".gitmodules");
   if(!FILfileExists(ModulesPath)) {
      COL_TRC(".gitmodules not found -- nothing to check");
      return true;
   }

   COLstring Contents;
   try {
      FILreadFile(ModulesPath, &Contents);
      Contents.replace("\r\n", "\n");
   } catch(const COLerror& E) {
      *pErr = E.description();
      return false;
   }

   bool                 InSection       = false;
   bool                 HasValidSection = false;
   COLarray<COLstring> Lines;
   COLsplit(&Lines, Contents, "\n");

   for(int i = 0; i < Lines.size(); ++i) {
      const COLstring TrimmedLine = COLstripWhiteSpace(Lines[i]);
      if(TrimmedLine.is_null() || TrimmedLine[0] == '#') {
         continue;  // Skip empty lines and comments
      }

      if(TrimmedLine[0] == '[') {
         // Section header check
         COL_TRC("Line: " << TrimmedLine);
         if(TrimmedLine.size() < 13 ||  // "[submodule ""] minimum length
            TrimmedLine.substr(0, 12) != "[submodule \"" || TrimmedLine[TrimmedLine.size() - 1] != ']') {
            *pErr = "Line " + COLint32ToString(i + 1) + ": Invalid section header format: " + TrimmedLine;
            return false;
         }
         InSection       = true;
         HasValidSection = true;
      } else if(InSection) {
         // Key-value pair check
         COLarray<COLstring> KeyValue;
         COLsplit(&KeyValue, TrimmedLine, "=");
         if(KeyValue.size() != 2 || COLstripWhiteSpace(KeyValue[0]).is_null()) {
            *pErr = "Line " + COLint32ToString(i + 1) + ": Invalid key-value format: " + TrimmedLine;
            return false;
         }

         // Validate required fields
         const COLstring Key   = COLstripWhiteSpace(KeyValue[0]);
         const COLstring Value = COLstripWhiteSpace(KeyValue[1]);
         if(Key == "path" && Value.is_null()) {
            *pErr = "Line " + COLint32ToString(i + 1) + ": Empty path in submodule configuration";
            return false;
         }
         if(Key == "url" && Value.is_null()) {
            *pErr = "Line " + COLint32ToString(i + 1) + ": Empty URL in submodule configuration";
            return false;
         }
      }
   }

   if(!HasValidSection) {
      *pErr = "No valid submodule sections found in .gitmodules";
      return false;
   }
   return true;
}
