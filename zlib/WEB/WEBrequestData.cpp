// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBrequestData
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Saturday 20 May 2023 - 02:57PM
// ---------------------------------------------------------------------------
#include <ctype.h>

#include <WEB/WEBrequestData.h>

#include <SFI/SFIbase64DecodeFilter.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

COLostream& operator<<(COLostream& Stream, const WEBrequestData& Data){
   Stream << "Path=" << Data.Path << newline 
          << "Headers=" << Data.Headers
          << "GET vars=" << Data.Args
          << "ConnectionID=" << Data.ConnectionId
         << " secure=" << Data.IsSecure << newline
          << "Body=" << Data.Body;
   return Stream;
}

static void WEBunescapeCharacters(COLstring& Value){
   const char* pIn = Value.c_str();
   char* pOut = (char*)Value.c_str();
   for(;;) {
      int Length = strcspn(pIn, "%+");
      memmove(pOut, pIn, Length);
      pOut += Length;
      pIn += Length;
      switch(*pIn++) {
      default:
         Value.setSize(pOut - Value.c_str());
         return;
      case '+':
         *pOut++ = ' ';
         break;
      case '%':
         if(!isxdigit(pIn[0]) || !isxdigit(pIn[1])) {
            *pOut++ = '%';
         } else {
            char Hex[3];
            Hex[0]=*pIn++; Hex[1]=*pIn++; Hex[2]=0;
            *pOut++ = strtol(Hex, NULL, 16);
         }
      }
   }
}

static void WEBparseVariables(const COLstring& RawData, COLvar* pVariableList){
   COL_FUNCTION(WEBparseVariables);
   COL_VAR(RawData);

   COLvar& Vars = *pVariableList;
   const char* pInput = RawData.c_str();
   do {
      int Length = strcspn(pInput, "&");
      int KeyLength = strcspn(pInput, "&=");
      COL_VAR2(Length, KeyLength);

      if(KeyLength < Length) {
         int ValueLength = Length - KeyLength - 1;
         COL_VAR(ValueLength);
         COLstring Key, Value;
         Key.append(pInput, KeyLength);
         WEBunescapeCharacters(Key);

         if(ValueLength > 0) {
            Value.append(pInput + KeyLength + 1, ValueLength);
            WEBunescapeCharacters(Value);
         }

         COL_VAR2(Key, Value);

         // When we see a key for the second time, turn the matching string COLvar
         // into an array, and start stuffing it with strings.
         if (Vars.exists(Key)) {
            if (! Vars[Key].isArray()) {
               COLvar Tmp;
               Tmp.swap(&Vars[Key]);
               Vars[Key].setArrayType();
               Vars[Key].push_back(Tmp);
            }
            Vars[Key].push_back(COLvar(Value));
         } else {
            Vars[Key] = Value;
         }
      }
      pInput += Length;
   } while(*pInput++);
   COL_VAR(Vars);
}

static bool WEBparseBasicAuth(const COLstring& HeaderValue, COLvar* pVariableList){
   COL_VAR(HeaderValue);
   if (HeaderValue.find("Basic") != 0){
      COL_TRC("Not basic authentication.");
      return false;
   }
   COLstring EncodedData = HeaderValue.substr(6);  // strip off Basic and ' '   
   COL_VAR(EncodedData);
   EncodedData.stripAll(' ');
   COL_VAR(EncodedData);
   COLstring DecodedData;
   try{
      SFIbase64Decode(EncodedData, &DecodedData);  // TODO some time check if SFIbase64Decode can throw.
   }catch(const COLerror& Error){
      COL_TRC(Error);
      return false;
   }
   COL_VAR(DecodedData);
   COLstring UserName, Password;
   if (!DecodedData.split(UserName, Password, ":")){
      COL_TRC("Could not split on : character");
      return false;
   }
   COLvar& Out = *pVariableList;
   if (Out.exists("username") || Out.exists("password")){
      COL_WRN("Overwriting username password parameter");
   }
   Out["username"] = UserName;
   Out["password"] = Password;
   return true;
}

COLvar WEBrequestInputs(const WEBrequestData& Request){
   COL_FUNCTION(WEBrequestInputs);
   COLvar Inputs;
   Inputs.setMapType();
   if (!Request.Body.is_null()){
      COL_TRC("Parsing JSON in body");
      if (!Inputs.parse(Request.Body)){
         COL_TRC("Parsing POST variables in body");
         WEBparseVariables(Request.Body, &Inputs);
         if (Inputs.size() == 0) {
            COL_WRN("Failed to parse request body:\n" << Request);
         }
      }
      COL_VAR(Inputs.json(1));
   }
   if (Request.Args.size() > 0){
      COL_TRC("Adding GET variables");
      for (auto i=Request.Args.cbegin(); i != Request.Args.cend(); i++){
         COL_VAR2(i->first, i->second);
         Inputs[i->first] = i->second;
      }
   }
   // Ugly workaround to add basic auth support for session/login.
   // The session/login handler doesn't have access to request headers
   // so we have to process it here
   if (Request.Headers.count("authorization")) {
      COL_TRC("Adding basic authorization parameters");
      WEBparseBasicAuth(Request.Headers["authorization"], &Inputs);
   }
   COL_VAR(Inputs.json(1));
   return Inputs;
}
