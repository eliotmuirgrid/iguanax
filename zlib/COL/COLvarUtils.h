#ifndef __COL_VAR_UTILS_H__
#define __COL_VAR_UTILS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLvarUtils
//
// Description:
//
// COLvarUtils - common utilities we use with COLvar that we haven't quite
// decided to make methods...
//
// Author: Eliot Muir
// Date:   Thu  6 Nov 2014 12:08:12 EST
//---------------------------------------------------------------------------

#include <COL/COLerror.h>
#include <COL/COLarray.h>
#include <COL/COLhashmap.h>  // TODO - do we really need this conversion.

class COLvar;
class COLstring;

// This function is intended to check for a path existing in a COLvar
// bool Exists = COLvarPathExists(Data, "error/fields/key/0");
// bool Exists = COLvarPathExists(Data, "errors/0/message");
bool COLvarPathExists(const COLvar& Data, const COLstring& Path);

void COLvarConvertFromHashMap(const COLhashmap<COLstring, COLstring>& TheMap, COLvar* pTheVar);
void COLvarConvertToHashMap(const COLvar& TheVar, COLhashmap<COLstring, COLstring>* pTheMap);
void COLvarConvertFromArray(const COLarray<COLstring>& TheVector, COLvar* pTheVar);
// NOTE : Does not check that the individual elements are of type string
void COLvarConvertToVector(const COLvar& TheVar, COLarray<COLstring>* pTheVector);

// NOTE : This returns true for empty arrays, which may or may not be desirable in certain usages
bool COLvarIsVector(const COLvar& TheVar);

// Fills a vector with the keys used in the COLvar
void COLvarKeys(const COLvar& TheVar, COLarray<COLstring>* pKeys);
// Populates a map using the strings from the vector as keys
void COLvarAddKeys(const COLarray<COLstring> Keys, COLvar* pTheVar, bool SkipEmpties = false);

//Reverses the order of the entries in a COLvar array.
void COLvarReverseVarArray(COLvar* pTheVar);

// These functions will throw a COLerror if the required named variable does not exist.  There might be a case for
// adding these as methods to COLvar itself one day.
COLstring COLvarRequiredString(const COLvar& Args, const COLstring& Name);
bool      COLvarRequiredBool  (const COLvar& Args, const COLstring& Name);
long long COLvarRequiredInt   (const COLvar& Args, const COLstring& Name);
COLvar    COLvarRequiredObject(const COLvar& Args, const COLstring& Name);
COLvar    COLvarRequiredStringOrStringList(const COLvar& Args, const COLstring& Name);
COLvar    COLvarRequiredObjectOrObjectList(const COLvar& Args, const COLstring& Name);

void COLsetDefault     (COLvar* pMap, const COLstring& Param, const char* pDefaultValue);
void COLsetDefault     (COLvar* pMap, const COLstring& Param, const COLstring& DefaultValue);
void COLsetDefault     (COLvar* pMap, const COLstring& Param, int DefaultValue);
void COLsetDefault     (COLvar* pMap, const COLstring& Param, long long DefaultValue);
void COLsetDefault     (COLvar* pMap, const COLstring& Param, bool DefaultValue);
void COLsetDefaultMap  (COLvar* pMap, const COLstring& Param);
void COLsetDefaultArray(COLvar* pMap, const COLstring& Param);

// TODO - bool and double etc.
const COLstring& COLvarGet(const COLvar& Map, const COLstring& Param, const COLstring& Default);
long long        COLvarGet(const COLvar& Map, const COLstring& Param, int Default);

// ELIOT - Deliberately not using overloading - explicit is better
COLstring COLvarGetString(const COLvar& Map, const COLstring& Param, const COLstring& Default);
bool      COLvarGetBool  (const COLvar& Map, const COLstring& Param, bool Default);
COLvar    COLvarGetObject(const COLvar& Map, const COLstring& Param, const COLvar& Default);

// Tries to cast a COLvar type to another type - but also gives a diagnostic return value if the conversion fails
// e.g. casting a string "hello" to an integer will return false - instead of COLvar::asInteger() which returns 0
bool COLvarCast(const COLvar& Value, long long* pToInt);
bool COLvarCast(const COLvar& Value, double* pToDouble);
bool COLvarCast(const COLvar& Value, bool* pToBool);
// NOTE : This casts integers, bools, and doubles, but does NOT convert arrays/maps into JSON representations
//        An application would need to make that conversion itself, as it's always possible to call COLvar::json
bool COLvarCast(const COLvar& Value, COLstring* pToString);

void COLvarExtend(COLvar* pExistingArray, const COLvar& NewItems);

// Several macros to assert that maps (potentially) have keys of a particular type
// Defined as macros for stringification (specifically so that the #Map can be used in the error description)
#define COLassertParamIsString(Map, Param) \
   do {  \
      if (!(Map).exists((Param)) || !(Map)[(Param)].isString()) {  \
         COL_ERROR_STREAM(#Map " " << (Param) << " must be provided as a string.", COLerror::PreCondition);   \
      }  \
   } while (0)
#define COLassertParamIsInt(Map, Param) \
   do {  \
      if (!(Map).exists((Param)) || !(Map)[(Param)].isInt()) {  \
         COL_ERROR_STREAM(#Map " " << (Param) << " must be provided as an integer.", COLerror::PreCondition);   \
      }  \
   } while (0)
#define COLassertParamIsBool(Map, Param) \
   do {  \
      if (!(Map).exists((Param)) || !(Map)[(Param)].isBool()) {  \
         COL_ERROR_STREAM(#Map " " << (Param) << " must be provided as a bool.", COLerror::PreCondition);   \
      }  \
   } while (0)
#define COLassertParamIsStringIfExists(Map, Param)   \
   do {  \
      if ((Map).exists((Param)) && !(Map)[(Param)].isString()) {  \
         COL_ERROR_STREAM(#Map " " << (Param) << " must be a string.", COLerror::PreCondition);   \
      }  \
   } while (0)
#define COLassertParamIsIntIfExists(Map, Param)   \
   do {  \
      if ((Map).exists((Param)) && !(Map)[(Param)].isInt()) {  \
         COL_ERROR_STREAM(#Map " " << (Param) << " must be an integer.", COLerror::PreCondition);   \
      }  \
   } while (0)
#define COLassertParamIsArrayOrStringIfExists(Map, Param)   \
   do {  \
      if ((Map).exists((Param)) && !COLvarIsVector((Map)[(Param)]) && !(Map)[(Param)].isString()) {  \
         COL_ERROR_STREAM(#Map " " << (Param) << " must be an array or a string.", COLerror::PreCondition);   \
      }  \
   } while (0)
#define COLassertParamIsBoolIfExists(Map, Param)   \
   do {  \
      if ((Map).exists((Param)) && !(Map)[(Param)].isBool()) {  \
         COL_ERROR_STREAM(#Map " " << (Param) << " must be a bool.", COLerror::PreCondition);   \
      }  \
   } while (0)
#define COLassertParamIsTableIfExists(Map, Param)   \
   do {  \
      if ((Map).exists((Param)) && !(Map)[(Param)].isMap()) {  \
         COL_ERROR_STREAM(#Map " " << (Param) << " must be a table.", COLerror::PreCondition);   \
      }  \
   } while (0)

#endif // end of defensive include
