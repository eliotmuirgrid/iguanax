// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CFGfieldGrammar
//
// Description: Implementation
//
// Author: Matthew Sobkowski
// Date:   Monday 24 April 2023 - 3:59PM
// ---------------------------------------------------------------------------
#include <CFG/CFGconfigApi.h>
#include <CFG/CFGfieldGrammar.h>
#include <CFG/CFGfieldSanitize.h>
#include <COL/COLlog.h>
#include <COL/COLsplit.h>
#include <COL/COLutils.h>
#include <COL/COLvar.h>
#include <FIL/FILutils.h>
COL_LOG_MODULE;

#ifdef _WIN32
#include <windows.h>
#include <processenv.h>

static void CMPCwindowsGetVarsCaseSensitive(COLmap<COLstring, COLstring>* pOut){
   COL_FUNCTION(CMPCwindowsGetVarsCaseSensitive);
   if (!pOut) { return; }
   pOut->clear();
   LPWCH pEnv = GetEnvironmentStringsW();
   if (pEnv == NULL) { return; }
   // MULTI_SZ walk: "NAME=VALUE\0NAME2=VALUE2\0\0"
   for (const wchar_t* p = pEnv; *p; p += wcslen(p) + 1) {
      const wchar_t* entry = p;
      // Normal: NAME=VALUE
      // Hidden drive vars: =C:=C:\path (first '=' belongs to key)
      const wchar_t* eq = (entry[0] == L'=') ? wcschr(entry + 1, L'=') : wcschr(entry, L'=');
      if (eq == NULL) { continue; }
      const int keyLen = static_cast<int>(eq - entry);
      if (keyLen <= 0) { continue; }
      COLstring EntryStr(entry);
      COLstring Key, Value;
      EntryStr.splitAtIndex(Key, Value, keyLen + 1);
      Key = Key.substr(0, keyLen);
      (*pOut)[Key] = Value;
   }
   FreeEnvironmentStringsW(pEnv);
}

static int CMPCgetVarNameLength(const char* Position){
   int VarLength = 0;
   while (Position[0] && Position[0] != '}'){
      ++VarLength;
      ++Position;
   }
   return Position[0] == 0 ? -1 : VarLength;
}

static COLstring CMPCgetEnvWin(const COLstring& Name){
   COL_FUNCTION(CMPCgetEnvWin);
   COLmap<COLstring, COLstring> WinEnvironment;
   CMPCwindowsGetVarsCaseSensitive(&WinEnvironment);
   // Same as COLexpandEnvironmentVariables
   COLstring Expanded;
   Expanded.setCapacity(Name.size());
   if (const char* Position = Name.c_str()){
      while (char Ch = Position[0]){
         if (Ch == '$'){
            int VarNameLength = 0;
            if (Position[1] == '{' && ((VarNameLength = CMPCgetVarNameLength(Position + 2)) >= 0)){
               COLstring VarName(Position + 2, VarNameLength);
               if (WinEnvironment.count(VarName)){
                  Expanded += WinEnvironment[VarName];
               } else {
                  COL_TRC("No match found for " + VarName);
               }
               Position += 2 + VarNameLength + 1; // skip over ${VAR}
               continue;
            }
         }
         Expanded += Ch;
         ++Position;
      }
   }
   return Expanded;
}
#endif

// IX-4338 Make the lookup of variables in custom fields case sensitive on Windows
// No change needed for POSIX
COLstring CMPCexpandEnvironmentVariables(const COLstring& TrueValue){
   COL_FUNCTION(CMPCexpandEnvironmentVariables);
#ifdef _WIN32
   COL_TRC("Using custom windows implementation for case sensitive matching");
   return CMPCgetEnvWin(TrueValue);
#else
   return COLexpandEnvironmentVariables(TrueValue);
#endif
}

COLostream& operator<<(COLostream& Stream, const COLarray<CFGfield>& Grammar) {
   for(const auto& it : Grammar) {
      Stream << "Name=" << it.Name << "\tType=" << CFGfieldTypeToString(it.FieldType) << "\tDefault=" << it.Default
             << newline;
   }
   return Stream;
}

CFGfieldType CFGstringToFieldType(const COLstring& Type) {
   COL_FUNCTION(CFGstringToFieldType);
   COL_VAR(Type);
   if(COLlowerCase(Type) == "bool") { return CFGfieldType::BOOL; }
   if(COLlowerCase(Type) == "file") { return CFGfieldType::FILE; }
   if(COLlowerCase(Type) == "folder") { return CFGfieldType::FOLDER; }
   if(COLlowerCase(Type) == "number") { return CFGfieldType::NUMBER; }
   if(COLlowerCase(Type) == "password") { return CFGfieldType::PASSWORD; }
   if(COLlowerCase(Type) == "port") { return CFGfieldType::PORT; }
   if(COLlowerCase(Type) == "list") { return CFGfieldType::List; }
   return CFGfieldType::STRING;
}

COLstring CFGfieldTypeToString(const CFGfieldType& Type) {
   COL_FUNCTION(CFGfieldTypeToString);
   COL_VAR((int)Type);
   switch(Type) {
      case CFGfieldType::BOOL: return "Bool";
      case CFGfieldType::FILE: return "File";
      case CFGfieldType::FOLDER: return "Folder";
      case CFGfieldType::NUMBER: return "Number";
      case CFGfieldType::PASSWORD: return "Password";
      case CFGfieldType::PORT: return "Port";
      case CFGfieldType::List: return "List";
      default: return "String";
   }
}

COLvar CFGfieldsParseOptions(const COLstring& Options) {
   COL_FUNCTION(CFGfieldsParseOptions);
   COLvar ListVar;
   ListVar.setArrayType();
   if(Options.is_null() || Options.isWhitespace()) { return ListVar; }
   COLarray<COLstring> List;
   COLsplit(&List, Options, ",");
   for(const auto& it : List) {
      if(!it.is_null()) { ListVar.push_back(it); }
   }
   COL_VAR2(Options, ListVar);
   return ListVar;
}

bool CFGfieldIsValid(const COLarray<CFGfield>& Grammar, const COLstring& Name, COLstring& Value, COLstring* pError) {
   COL_FUNCTION(CFGfieldIsValid);
   for(const auto& Field : Grammar) {
      if(Name == Field.Name) { return CFGfieldsSanitizeValue(Name, Field.FieldType, Value, pError); }
   }
   *pError = "Custom field '" + Name + "' is not defined in config.json";
   return false;
}

static bool CFGfieldValidateEntry(const COLvar& Entry) {
   COL_FUNCTION(CFGfieldValidateEntry);
   const bool KeysValid = Entry.exists("name") && Entry.exists("type") && (Entry.exists("default") || Entry.exists("value"));
   if(!KeysValid) { COL_TRC("Invalid FieldVar -- " << Entry); }
   return KeysValid;
}

static COLstring CFGfieldExtractDefault(const COLvar& Entry, CFGfieldType Type, const COLstring& Options) {
   COL_FUNCTION(CFGfieldExtractDefault);
   if(Type == CFGfieldType::List) {
      COL_TRC("Have list type -- get first item if not null");
      COLstring Out1, Out2;
      return Options.split(Out1, Out2, ",") ? Out1 : Options;
   }
   return Entry.exists("default") && Type != CFGfieldType::PASSWORD ? Entry["default"].asString()
                                                                    : Entry["value"].asString();
}

static bool CFGfieldProcessEntry(const CFGfield& Field, COLarray<CFGfield>* pGrammar, COLstring* pErr) {
   COL_FUNCTION(CFGfieldProcessEntry);
   COLstring Expanded = COLexpandEnvironmentVariables(Field.Default);
   COL_TRC("Default=" << Expanded);

   COLstring Err;
   if(CFGfieldsSanitizeValue(Field.Name, Field.FieldType, Expanded, &Err)) {
      pGrammar->push_back(Field);
      return true;
   }
   pErr->append(Err + "\n");
   return false;
}

bool CFGfieldCreateGrammar(const COLvar& Data, COLarray<CFGfield>* pGrammar, COLstring* pErr) {
   COL_FUNCTION(CFGfieldCreateGrammar);
   if(Data.isNull()) return true;
   if(!Data.isArray()) {
      COL_ERR("Field grammar must be of array type");
      return false;
   }

   for(const auto& it : Data.array()) {
      if(!CFGfieldValidateEntry(it)) continue;
      CFGfieldType Type        = CFGstringToFieldType(it["type"]);
      COLstring    Options     = it["options"];
      COLstring    Default     = CFGfieldExtractDefault(it, Type, Options);
      COLstring    Description = it.exists("description") ? it["description"].asString() : "";
      CFGfield     Field(it["name"], Type, Default, Options, Description);
      CFGfieldProcessEntry(Field, pGrammar, pErr);
   }
   return pErr->is_null();
}

bool CFGfieldLoadGrammar(const COLstring& Path, COLarray<CFGfield>* pGrammar, COLstring* pErr) {
   COL_FUNCTION(CFGfieldLoadGrammar);
   COL_VAR(Path);
   COLvar Config;
   if(!FILfileExists(Path)) {
      *pErr = Path + " does not exist";
      COL_TRC(*pErr);
      return true;
   }
   try {
      COLstring RawData;
      FILreadFile(Path, &RawData);
      if(!RawData.is_null()) {
         COLvar Json;
         if(!Json.parse(RawData)) {
            *pErr = "Failed to parse field grammar";
            return false;
         }
         return CFGfieldCreateGrammar(Json["fields"], pGrammar, pErr);
      }
      return true;
   } catch(const COLerror& Exception) {
      *pErr = Exception.description();
      return false;
   }
}

bool CFGfieldSaveGrammar(const COLstring& Component, const COLarray<CFGfield>& Grammar, COLstring* pErr, bool DevMode) {
   COL_FUNCTION(CFGfieldSaveGrammar);
   COLvar Data;
   Data["fields"].setArrayType();
   for(const auto& Field : Grammar) {
      COLvar FieldVar;
      FieldVar["name"]    = Field.Name;
      FieldVar["default"] = Field.Default;
      FieldVar["type"]    = CFGfieldTypeToString(Field.FieldType);
      FieldVar["options"] = Field.Options;
      if(!Field.Description.is_null()) { FieldVar["description"] = Field.Description; }
      Data["fields"].push_back(FieldVar);
   }
   const COLstring Path = CFGresolveRepoFile(Component, "config.json", DevMode);
   COL_VAR2(Path, Data);
   COLstring  Error;
   const bool Success = FILsave(Path, Data.json(1), pErr);
   COL_VAR2(Success, *pErr);
   return Success;
}
