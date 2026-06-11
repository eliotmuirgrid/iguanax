//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLutils
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thu  6 Nov 2014 12:08:37 EST
//---------------------------------------------------------------------------
#include "COLvar.h"
#include "COLmath.h"
#include "COLvarUtils.h"
#include "COLlog.h"
COL_LOG_MODULE;

bool COLvarPathExists(const COLvar& Data, const COLstring& iPath){
   COL_FUNCTION(COLvarPathExists);
   bool PathLeft;
   COLstring Key;
   COLstring Path = iPath;
   const COLvar* pData = &Data;
   do
   {
      PathLeft = Path.split(Key, Path, "/");
      COL_VAR(Key);
      if( COLmath::isInteger(Key) ){
         if(!pData->exists( ::atoi(Key.c_str()) )){
            COL_TRC(Key << " not found");
            return false;
         }
      }
      else{
         if (!pData->exists(Key)){
            COL_TRC(Key << " not found");
            return false;
         }
      }
      pData = &(*pData)[Key];
   } while (PathLeft);

   return true;
}

void COLsetDefault(COLvar* pMap, const COLstring& Param, const char* pDefaultValue){
   COL_FUNCTION(COLsetDefault-const-char);
   COL_VAR2(Param,pDefaultValue);
   COL_VAR(pMap->json(true));
   if (!pMap->exists(Param)){
      pMap->operator [](Param) = pDefaultValue;
   }
   COL_VAR(pMap->json(true));
}

void COLsetDefault(COLvar* pMap, const COLstring& Param, const COLstring& DefaultValue){
   COL_FUNCTION(COLsetDefault-string);
   COL_VAR2(Param,DefaultValue);
   COL_VAR(pMap->json(true));
   if (!pMap->exists(Param)){
      pMap->operator [](Param) = DefaultValue;
   }
   COL_VAR(pMap->json(true));
}

void COLsetDefault(COLvar* pMap, const COLstring& Param, int DefaultValue){
   COL_FUNCTION(COLsetDefault-int);
   COL_VAR2(Param,DefaultValue);
   COL_VAR(pMap->json(true));
   if (!pMap->exists(Param)){
      pMap->operator [](Param) = DefaultValue;
   }
   COL_VAR(pMap->json(true));
}

void COLsetDefault(COLvar* pMap, const COLstring& Param, long long DefaultValue){
   COL_FUNCTION(COLsetDefault-long-long);
   COL_VAR2(Param,DefaultValue);
   COL_VAR(pMap->json(true));
   if (!pMap->exists(Param)){
      pMap->operator [](Param) = DefaultValue;
   }
   COL_VAR(pMap->json(true));
}

void COLsetDefault(COLvar* pMap, const COLstring& Param, bool DefaultValue){
   COL_FUNCTION(COLsetDefault-bool);
   COL_VAR2(Param,DefaultValue);
   COL_VAR(pMap->json(true));
   if (!pMap->exists(Param)){
      pMap->operator [](Param) = DefaultValue;
   }
   COL_VAR(pMap->json(true));
}

void COLsetDefaultMap(COLvar* pMap, const COLstring& Param){
   COL_FUNCTION(COLsetDefault);
   COL_VAR(Param);
   COL_VAR(pMap->json(true));
   if (!pMap->exists(Param)){
      pMap->operator [](Param).setMapType();
   }
   COL_VAR(pMap->json(true));
}

void COLsetDefaultArray(COLvar* pMap, const COLstring& Param){
   COL_FUNCTION(COLsetDefault);
   COL_VAR(Param);
   COL_VAR(pMap->json(true));
   if (!pMap->exists(Param)){
      pMap->operator [](Param).setArrayType();
   }
   COL_VAR(pMap->json(true));
}

const COLstring& COLvarGet(const COLvar& Map, const COLstring& Param, const COLstring& Default){
   COL_FUNCTION(COLvarGet);
   if (Map.exists(Param)){
      return Map[Param].str();  // TODO DANGEROUS - will cast
   } else {
      return Default;
   }
}

long long COLvarGet(const COLvar& Map, const COLstring& Param, int Default){
   COL_FUNCTION(COLvarGet);
   if (Map.exists(Param)){
      return Map[Param].asInt();
   } else {
      return Default;
   }
}

COLstring COLvarGetString(const COLvar& Map, const COLstring& Param, const COLstring& Default){
   COL_FUNCTION(COLvarGetString);
   if (Map.exists(Param)){
      return Map[Param].asString();
   } else {
      return Default;
   }
}

bool COLvarGetBool(const COLvar& Map, const COLstring& Param, bool Default){
   COL_FUNCTION(COLvarGetBool);
   if (Map.exists(Param)){
      return Map[Param].asBool();
   } else {
      return Default;
   }
}

COLvar COLvarGetObject(const COLvar& Map, const COLstring& Param, const COLvar& Default){
   COL_FUNCTION(COLvarGetObject);
   if (Map.exists(Param)){
      return Map[Param];
   } else {
      return Default;
   }
}

bool COLvarCast(const COLvar& Value, long long* pToInt) {
   COL_FUNCTION(COLvarCast-toInt);
   COL_VAR(Value);
   if (Value.isInteger()) {
      COL_TRC("It's already an integer");
      *pToInt = Value.asInt();
      return true;
   }
   if (Value.isDouble()) {
      long long Floored = (long long)(Value.asDouble());
      if (Floored == Value.asDouble()) {
         *pToInt = Floored;
         return true;
      } else {
         return false;
      }
   }
   if (Value.isString()) {
      // The hardest case... parse various integer representations
      const COLstring& ValueStr = Value.str();
      int StringLength = ValueStr.size();
      if (ValueStr.is_null()) {
         COL_TRC("Null string");
         return false;
      }
      int StringIndex = 0;
      bool IsNegative = false;
      int Base = 10;
      if (ValueStr[StringIndex] == '-') {
         StringIndex++;
         IsNegative = true;
      }
      if (StringIndex >= StringLength) {
         COL_TRC("No digits");
         return false;
      }
      if (ValueStr[StringIndex] == '0') {
         // Look-ahead parsing
         if (StringIndex + 1 >= StringLength) {
            COL_TRC("Single 0 followed by nothing");
            *pToInt = 0;
            return true;
         }
         if (ValueStr[StringIndex + 1] == 'x') {
            // 0x ... - we're in hex at this point
            StringIndex += 2;
            Base = 16;
         } else {
            // TODO : Should we even support octal?
            // 0 ... - we're in octal at this point
            StringIndex++;
            Base = 8;
         }
      }
      if (StringIndex >= StringLength) {
         COL_TRC("No digits");
         return false;
      }
      // Now we're into the digits, finally - starting accumulating values
      int Accumulation = 0;
      while (StringIndex < StringLength) {
         char c = ValueStr[StringIndex];
         int CurrentDigitValue;
         if      (c >= '0' && c <= '9') { CurrentDigitValue = c - '0';      }
         else if (c >= 'a' && c <= 'f') { CurrentDigitValue = c - 'a' + 10; }
         else if (c >= 'A' && c <= 'F') { CurrentDigitValue = c - 'A' + 10; }
         else {
            COL_TRC("Invalid digit: " << c);
            return false;
         }
         if (CurrentDigitValue >= Base) {
            COL_TRC("Invalid digit for base " << Base << ": " << c);
            return false;
         }
         Accumulation = Base * Accumulation + CurrentDigitValue;
         StringIndex++;
      }
      *pToInt = IsNegative ? -Accumulation : Accumulation;
      return true;
   }

   COL_TRC("Not going to cast COLvar::type = " << Value.type());
   return false;
}

bool COLvarCast(const COLvar& Value, bool* pToBool) {
   COL_FUNCTION(COLvarCast-toBool);
   COL_VAR(Value);
   if (Value.isInteger() || Value.isDouble()) {
      COL_TRC("Not casting numeric types to bool");
      return false;
   }
   if (Value.isBool()) {
      COL_TRC("It's already a boolean");
      *pToBool = Value.asBool();
      return true;
   }
   if (Value.isString()) {
      // This list isn't extensive, more may be required as the need arises
      const COLstring& ValueStr = Value.str();
#define __stringToBool(str, val)                                  \
      do {                                                        \
         if (ValueStr == str) { *pToBool = val; return true; }    \
      } while (0)
      __stringToBool("t", true);
      __stringToBool("T", true);
      __stringToBool("f", false);
      __stringToBool("F", false);
      __stringToBool("true", true);
      __stringToBool("True", true);
      __stringToBool("TRUE", true);
      __stringToBool("false", false);
      __stringToBool("False", false);
      __stringToBool("FALSE", false);
#undef __stringToBool
      COL_TRC("Unrecognized string: " << ValueStr);
      return false;
   }

   COL_TRC("Not going to cast COLvar::type = " << Value.type());
   return false;
}

bool COLvarCast(const COLvar& Value, COLstring* pToString) {
   COL_FUNCTION(COLvarCast-toString);
   COL_VAR(Value);
   if (Value.isScalar()) {
      *pToString = Value.asString();
      return true;
   }

   COL_TRC("Not going to cast COLvar::type = " << Value.type());
   return false;
}

void COLvarExtend(COLvar* pExistingArray, const COLvar& NewItems){
   COL_FUNCTION(COLvarExtend);
   COLPRECONDITION(NewItems.isArray());
   pExistingArray->setArrayType();
   for (int i=0; i<NewItems.size(); i++) {
      pExistingArray->push_back(NewItems[i]);
   }
}

void COLvarConvertFromHashMap(const COLhashmap<COLstring, COLstring>& TheMap, COLvar* pTheVar) {
   COL_FUNCTION(COLvarConvertFromHashMap);
   COLvar& TheVar = *pTheVar;
   TheVar.reset();
   TheVar.setMapType();

   for (COLhashmap<COLstring, COLstring>::const_iterator i = TheMap.cbegin(); i != TheMap.cend(); ++i) {
      COL_VAR2(i->first, i->second);
      TheVar[i->first] = i->second;
   }
   COL_VAR(TheVar);
}

void COLvarConvertToHashMap(const COLvar& TheVar, COLhashmap<COLstring, COLstring>* pTheMap) {
   COL_FUNCTION(COLvarConvertToHashMap);
   if (! TheVar.isMap()) {
      return;
   }
   COLhashmap<COLstring, COLstring>& TheMap = *pTheMap;
   TheMap.clear();
   const COLvar::COLvarMap& TheVarMap = TheVar.map();
   for (COLvar::COLvarMap::const_iterator i = TheVarMap.begin(); i != TheVarMap.end(); ++i) {
      TheMap[i->first] = i->second.asString();
   }
}

void COLvarConvertFromArray(const COLarray<COLstring>& TheVector, COLvar* pTheVar) {
   COL_FUNCTION(COLvarConvertFromArray);
   COLvar& TheVar = *pTheVar;
   TheVar.reset();
   TheVar.setArrayType();
   // TODO - could we size the COLvar vector to fit the size of the vector that we are copying from

   for (COLarray<COLstring>::const_iterator i = TheVector.begin(); i != TheVector.end(); ++i) {
      TheVar.push_back(*i);
   }
}

void COLvarConvertToVector(const COLvar& TheVar, COLarray<COLstring>* pTheVector) {
   COL_FUNCTION(COLvarConvertToVector);
   if (! TheVar.isArray()) {
      return;
   }
   COLarray<COLstring>& TheVector = *pTheVector;
   const COLvar::COLvarVec& TheVarVec = TheVar.array();
   for (COLvar::COLvarVec::const_iterator i = TheVarVec.begin(); i != TheVarVec.end(); ++i) {
      // No checking is done here to see if the type is a string
      TheVector.push_back(i->asString());
   }
}

bool COLvarIsVector(const COLvar& TheVar) {
   COL_FUNCTION(COLvarIsVector);
   if (! TheVar.isArray()) {
      return false;
   }
   const COLvar::COLvarVec& TheVarVec = TheVar.array();
   for (COLvar::COLvarVec::const_iterator i = TheVarVec.begin(); i != TheVarVec.end(); ++i) {
      if (! i->isString()) {
         return false;
      }
   }
   // This will therefore return true if the array is empty as well
   return true;
}

void COLvarKeys(const COLvar& TheVar, COLarray<COLstring>* pKeys) {
   COL_FUNCTION(COLvarKeys);
   if (! TheVar.isMap()) {
      return;
   }
   COLarray<COLstring>& Keys = *pKeys;
   Keys.resize(TheVar.size());
   const COLvar::COLvarMap& TheMap = TheVar.map();
   int j = 0;
   for (COLvar::COLvarMap::const_iterator i = TheMap.begin(); i != TheMap.end(); ++i) {
      Keys[j++] = i->first;
   }
}

void COLvarAddKeys(const COLarray<COLstring> Keys, COLvar* pTheVar, bool SkipEmpties) {
   COL_FUNCTION(COLvarAddKeys);
   COLvar& TheVar = *pTheVar;
   for (COLarray<COLstring>::const_iterator i = Keys.begin(); i != Keys.end(); ++i) {
      if (! i->is_null() || ! SkipEmpties) {
         TheVar[*i];
      }
   }
}

void COLvarReverseVarArray(COLvar* pTheVar){
   COL_FUNCTION(COLvarReverseArray);
   COL_VAR( pTheVar->json(true) );
   COLvar& TheVar = *pTheVar;
   COLvar ReversedArray;
   int Size = TheVar.size();
   for(int i = 0; i < Size; i++){
      int CurrentEndIndex = Size - (i + 1);
      ReversedArray[CurrentEndIndex].swap( &TheVar[i] );
   }
   ReversedArray.swap( pTheVar );
   COL_VAR( pTheVar->json(true) );
}

COLstring COLvarRequiredString(const COLvar& Args, const COLstring& Name){
   COL_FUNCTION(COLvarRequiredString);
   COL_VAR(Args);
   if (!Args.exists(Name)) { throw COLerror(Name + " parameter is required.", 0);}
   COL_VAR2(Name, Args[Name]);
   return Args[Name];
}

bool COLvarRequiredBool(const COLvar& Args, const COLstring& Name){
   COL_FUNCTION(COLvarRequiredBool);
   COL_VAR2(Args.json(1),Name);
   if (!Args.exists(Name)) {
      COL_TRC("Couldn't find name " << Name);
      throw COLerror(Name + " parameter is required.", 0);
   }
   bool BoolArg = Args[Name]; // convert to a boolean.
   COL_VAR3(Name, Args[Name], BoolArg);
   return BoolArg;
}

long long COLvarRequiredInt(const COLvar& Args, const COLstring& Name){
   COL_FUNCTION(COLvarRequiredInt);
   if (!Args.exists(Name)) { throw COLerror(Name + " parameter is required.",0);}
   long long intArg = Args[Name].asInt();
   COL_VAR3(Name, Args[Name], intArg);
   return intArg;
}

COLvar COLvarRequiredObject(const COLvar& Args, const COLstring& Name) {
   COL_FUNCTION(COLvarRequiredObject);
   if (!Args.exists(Name)) { throw COLerror(Name + " parameter is required.", 0);}
   COLvar arrArg = Args[Name];
   COL_VAR3(Name, Args[Name], arrArg);
   return arrArg;
}

COLvar COLvarRequiredStringOrStringList(const COLvar& Args, const COLstring& Name) {
   COL_FUNCTION(COLvarRequiredStringOrStringList);
   if (!Args.exists(Name)) { throw COLerror(Name + " parameter is required.", 0);}
   COLvar Arg = Args[Name];
   if (Arg.isString()) {
      COLvar Out;
      Out.push_back(Arg);
      return Out;    // return an array of 1 string
   }
   if (Arg.isArray()) {
      for (const auto& Item : Arg.array()) {
         if (!Item.isString()) {
            throw COLerror(Name + " parameter must be a string or an array of strings.");
         }
      }
      return Arg;    // return the string array
   }
   // not a string or a map
   throw COLerror(Name + " parameter must be a string or an array of strings.");
}

COLvar COLvarRequiredObjectOrObjectList(const COLvar& Args, const COLstring& Name) {
   COL_FUNCTION(COLvarRequiredObjectOrObjectList);
   if (!Args.exists(Name)) {
      if(Args.isMap()) {
         COLvar Out;
         Out.push_back(Args);
         return Out;  // return an array of 1 object
      }
      throw COLerror("Args is not of type map.", 0);
   }
   COLvar Arg = Args[Name];
   if (Arg.isMap()) {
      COLvar Out;
      Out.push_back(Arg);
      return Out;    // return an array of 1 object
   }
   if (Arg.isArray()) {
      for (const auto& Item : Arg.array()) {
         if (!Item.isMap()) {
            throw COLerror(Name + " parameter must be an object or an array of objects.");
         }
      }
      return Arg;    // return the object array
   }
   // not a string or a map
   throw COLerror(Name + " parameter must be a object or an array of objects.");
}
