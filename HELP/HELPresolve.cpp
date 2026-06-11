//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: HELPresolve.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   27/06/25 11:21 AM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <FIL/FILutils.h>
#include <HELP/HELPresolve.h>
#include <HELP/HELPmodel.h>
#include <COL/COLpair.h>
#include <COL/COLsplit.h>
COL_LOG_MODULE;

static bool HELPreplaceMetaTable(const COLpair<COLstring, COLstring>& Pair, COLstring* pName) {
   COL_FUNCTION(HELPreplaceMetaTable);
   if(pName->find(Pair.first) != 0) { return false; }
   COL_TRC("Replacing metatable preifx: " << Pair.first << " to " << Pair.second);
   pName->replace(0, Pair.first.size(), Pair.second);
   return true;
}

static void HELPreplaceLastDot(COLstring* pName) {
   COL_FUNCTION(HELPreplaceLastDot);
   const size_t lastDot = pName->find_last_of(".");
   if(lastDot != npos) { (*pName)[lastDot] = ':'; }
}

static bool HELPisStringMethod(const COLstring& Name) {
   COL_FUNCTION(HELPisStringMethod);
   if(Name.find("string") != 0) { return false; }
   return !Name.equals("string.char") && !Name.equals("string.format") && !Name.equals("string.dump");
}

static bool HELPisNetMethod(const COLstring& Name) {
   COL_FUNCTION(HELPisNetMethod);
   if(Name.find("net.") != 0 || Name.find("net.http.") == 0 || Name.find("net.smtp.") == 0) { return false; }
   return Name.find(".init") == npos && Name.find(".connect") == npos && Name.find(".listen") == npos;
}

void HELPreplaceMetatablePrefix(COLstring* pName) {
   COL_FUNCTION(HELPreplaceMetatablePrefix);
   if(pName->find(':') != npos) { return; }  // already in method form
   if(HELPisNetMethod(*pName) || HELPisStringMethod(*pName)) { return HELPreplaceLastDot(pName); }
   static const COLpair<COLstring, COLstring> Replacements[] = {
       {"iofilehandle.", "io:"},
       {"dbconnection.", "db:"},
   };
   for(const auto& it : Replacements) {
      if(HELPreplaceMetaTable(it, pName)) { break; }
   }
}

static void HELPresolveMetatableName(COLarray<COLstring>* pFunctionParts) {
   COL_FUNCTION(HELPresolveMetatableNames);
   COL_VAR(pFunctionParts->size());
   static const COLpair<COLstring, COLstring> Replacements[] = {{"io", "FILE*"},
                                                                {"db", "dbconnection"},
                                                                {"net.ftp", "CurlFtpMeta_ftp"},
                                                                {"net.ftps", "CurlFtpMeta_ftps"},
                                                                {"net.sftp", "CurlFtpMeta_sftp"},
                                                                {"net.tcp", "tcp_socket"}};
   for(auto& it : *pFunctionParts) {
      for(const auto& entry : Replacements) {
         if(it == entry.first) {
            it = entry.second;
            break;
         }
      }
      COL_TRC(it);
   }
}

void HELPresolveLua(const COLstring& String, COLarray<COLstring>* pFunctionParts, bool* pIsMethod) {
   COL_FUNCTION(HELPresolveLua);
   COL_VAR(String);
   *pIsMethod = String.find(":") != npos;
   if(*pIsMethod) {
      COL_TRC("Have method syntax -- split via ':'");
      COLsplit(pFunctionParts, String, ":");
      HELPresolveMetatableName(pFunctionParts);
   } else {
      COL_TRC("Have function syntax -- split via '.'");
      COLsplit(pFunctionParts, String, ".");
   }
}