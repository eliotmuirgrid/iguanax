// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CFGfieldsSanitize
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday 24 April 2023 - 03:30PM
// ---------------------------------------------------------------------------
#include <cstdint>
#include <CFG/CFGfieldSanitize.h>

#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


static bool CFGfieldsSanitizeFile(const COLstring& Name, COLstring& Value, COLstring* pError) {
   COL_FUNCTION(CFGfieldsSanitizeFile);
   char LastChar = Value[Value.size() - 1];
   COL_VAR3(Name, Value, LastChar);
   if(LastChar == FIL_DIR_SEPARATOR[0]) {
      *pError += "Invalid 'file' path for custom field '" + Name + "\n";
      return false;
   }
   FILcorrectPathSeparators(Value);
   return true;
}

static bool CFGfieldsSanitizeFolder(COLstring& Value) {
   COL_FUNCTION(CFGfieldsSanitizeFolder);
   FILcorrectPathSeparators(Value);
   FILaddPathSeparator(Value);
   return true;
}

static bool CFGfieldsSanitizePort(const COLstring& Name, const COLstring& Value, COLstring* pError) {
   COL_FUNCTION(CFGfieldsSanitizePort);
   char* pEnd = NULL;
   int Port = ::strtol(Value.c_str(), &pEnd, 0);
   if ((pEnd != Value.c_str() + Value.size()) || (Port < 0 || Port > UINT16_MAX)) {
      *pError += "Invalid 'port' value for custom field '" + Name + "'\n";
      return false;
   }
   return true;
}

static bool CFGfieldsSanitizeNumber(const COLstring& Name, const COLstring& Value, COLstring* pError) {
   COL_FUNCTION(CFGfieldsSanitizeNumber);
   char* pEnd = NULL;
   // TODO: Do we want to support real numbers as well?
   COLint64 Number = ::strtoll(Value.c_str(), &pEnd, 0);
   // All numbers are doubles in Lua and not every integer > 9007199254740992 can be represented
   if ((pEnd != Value.c_str() + Value.size()) || Number > 9007199254740992) {
      *pError += "Invalid 'number' value for custom field '" + Name + "'\n";
      return false;
   }
   return true;
}

static bool CFGfieldsSanitizeBoolean(const COLstring& Name, const COLstring& Value, COLstring* pError) {
   COL_FUNCTION(CFGfieldsSanitizeBoolean);
   if (Value != "true" && Value != "false") {
      *pError += "Invalid 'boolean' value for custom field '" + Name + "'\n";
      return false;
   }
   return true;
}

bool CFGfieldsSanitizeValue(const COLstring& Name, CFGfieldType Type, COLstring& Value, COLstring* pError) {
   COL_FUNCTION(CFGfieldsSanitizeValue);
   COL_VAR3(Name, (int) Type, Value);
   if(Value.is_null() && CFGfieldType::BOOL != Type) { return true; }
   switch(Type) {
      case CFGfieldType::BOOL   : return CFGfieldsSanitizeBoolean(Name, Value, pError);
      case CFGfieldType::NUMBER : return CFGfieldsSanitizeNumber(Name, Value, pError);
      case CFGfieldType::PORT   : return CFGfieldsSanitizePort(Name, Value, pError);
      case CFGfieldType::FOLDER : return CFGfieldsSanitizeFolder(Value);
      case CFGfieldType::FILE   : return CFGfieldsSanitizeFile(Name, Value, pError);
      default                   : return true;
   }
}