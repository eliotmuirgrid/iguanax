//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: REGregistry
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Mon 28/06/1999 
//---------------------------------------------------------------------------
#include "REGprecomp.h"
#pragma hdrstop

#include "REGregistryWindows.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

class REGregistryPrivate 
{
public:
   REGregistryPrivate(HKEY iRootKey);
   ~REGregistryPrivate();

   HKEY RootKey;
   HKEY CurrentKey;
   bool LazyWrite;
   DWORD KeyIndex;

   void openKey(const COLstring& SubKeyPath, bool CanCreate, REGSAM AdditionalAccessMask);
   void closeKey(HKEY& Key);

   void writeString(const COLstring& Name, const COLstring& Value, REGregistry::REGstringType StringType);
   const COLstring readString(const COLstring& Name, const char* pDefault, REGregistry::REGstringType* pActualType);

   COLint32 readInteger(const COLstring& Name, const COLint32 Default) const;
   bool readBoolean(const COLstring& Name, bool Default) const;
   void checkResult(LONG Result) const;

   void writeBoolean(const COLstring& Name, bool pValue);

   void writeInteger(const COLstring& Name, COLint32 Value);

   void deleteEntry(const COLstring& Name);

   bool subKeyExists(const COLstring& SubKeyPath);

   void deleteSubKey(const COLstring& SubKeyPath);

   static const COLuint32 REG_MAXIMUM_KEY_NAME_LENGTH;
   static const COLuint32 REG_INITIAL_SIZE_OF_BUFFER;
};

const COLuint32 REGregistryPrivate::REG_MAXIMUM_KEY_NAME_LENGTH = 1024;
const COLuint32 REGregistryPrivate::REG_INITIAL_SIZE_OF_BUFFER  = 200;

REGregistryPrivate::REGregistryPrivate(HKEY iRootKey) : LazyWrite(true), RootKey(iRootKey), CurrentKey(NULL), KeyIndex(0){
   COL_METHOD(REGregistryPrivate::REGregistryPrivate);
   COL_VAR(RootKey);
   // do nothing
   COLPRECONDITION(RootKey == HKEY_LOCAL_MACHINE  ||
                   RootKey == HKEY_CLASSES_ROOT   ||
                   RootKey == HKEY_CURRENT_CONFIG ||
                   RootKey == HKEY_CURRENT_USER   ||
                   RootKey == HKEY_USERS);
}

REGregistryPrivate::~REGregistryPrivate(){
   COL_METHOD(REGregistryPrivate::REGregistryPrivate);
   try{
      closeKey(CurrentKey);
   }
   catch(const COLerror& Error){
      COL_ERR(Error);
   }
}

void REGregistryPrivate::openKey(const COLstring& SubKeyPath, bool CanCreate, REGSAM AdditionalAccessMask){
   COL_METHOD(REGregistryPrivate::openKey);
   COL_VAR3(SubKeyPath, CanCreate, AdditionalAccessMask);
   HKEY SourceKey;
   HKEY OldCurrentKey = CurrentKey;
   if (0 == SubKeyPath.size() || '\\' == SubKeyPath[0] || NULL == CurrentKey){
      SourceKey = RootKey;
   }else{
      SourceKey = CurrentKey;
   }

   REGSAM AccessMask = KEY_ALL_ACCESS | AdditionalAccessMask;
   if (CanCreate){
      //check if user can modify
      if (RegCreateKeyEx(SourceKey,SubKeyPath.c_str(),0,"",REG_OPTION_NON_VOLATILE, 
                         AccessMask,NULL,&CurrentKey,NULL) != ERROR_SUCCESS )
      {
         //key probably cannot be created. try to open read only.
         checkResult(RegOpenKeyEx(SourceKey, SubKeyPath.c_str(), 0, KEY_READ, &CurrentKey));
      }
   } else{
      if ( RegOpenKeyEx(SourceKey, SubKeyPath.c_str(), 0, AccessMask, &CurrentKey) != ERROR_SUCCESS ){
         //key probably cannot be opened read+write. try to open read only.
         checkResult(RegOpenKeyEx(SourceKey, SubKeyPath.c_str(), 0, KEY_READ, &CurrentKey));
      }
   }
   closeKey(OldCurrentKey);
}

void REGregistryPrivate::closeKey(HKEY& Key){
   COL_METHOD(REGregistryPrivate::closeKey);
   COL_VAR(Key);
   if (NULL == Key){
      return;
   }
   if (!LazyWrite){
      checkResult(RegFlushKey(Key));
   }
   checkResult(RegCloseKey(Key));
   Key = NULL;
}

void REGregistryPrivate::checkResult(LONG Result) const{
   if (Result != ERROR_SUCCESS){
      COL_ERROR_STREAM(COLerrorString(Result) + " while accessing windows registry.", COLerror::SystemError);
   }
}

void REGregistryPrivate::writeString(const COLstring& Name, const COLstring& Value, REGregistry::REGstringType StringType){
   COL_METHOD(REGregistryPrivate::writeString);
   COL_VAR3(Name, Value, StringType);
   HKEY Key;
   if (NULL == CurrentKey){
      Key = RootKey;
   } else {
      Key = CurrentKey;
   }
   COLPOSTCONDITION(Key != NULL);
   checkResult(RegSetValueExA(Key, Name.c_str(),0, StringType, (LPBYTE)Value.c_str(), Value.size()));
}

COLint32 REGregistryPrivate::readInteger(const COLstring& Name, const COLint32 Default) const{
   COL_METHOD(REGregistryPrivate::readInteger);
   COL_VAR2(Name, Default);
   HKEY Key;
   if (NULL == CurrentKey){
      Key = RootKey;
   } else{
      Key = CurrentKey;
   }
   DWORD Type;
   LONG Result;
   COLint32 Value;
   DWORD BufferLength = sizeof(Value);
   Result = RegQueryValueEx(Key, Name.c_str(), NULL, &Type, (LPBYTE)&Value, &BufferLength);
   if (ERROR_SUCCESS != Result || REG_DWORD != Type){
      return Default;
   }
   return Value;
}

bool REGregistryPrivate::readBoolean(const COLstring& Name, bool Default) const{
   COL_METHOD(REGregistryPrivate::readBoolean);
   COLint32 Result = readInteger(Name, COLint32(Default));
   if (1 == Result){
      return true;
   } else {
      return false;
   }
}

void REGregistryPrivate::writeBoolean(const COLstring& Name, bool pValue){
   COL_METHOD(REGregistryPrivate::writeBoolean);
   writeInteger(Name, COLint32(pValue));
}

void REGregistryPrivate::writeInteger(const COLstring& Name, COLint32 Value){
   COL_METHOD(REGregistryPrivate::writeInteger);
   COL_VAR2(Name, Value);
   HKEY Key;
   if (NULL == CurrentKey){
      Key = RootKey;
   } else {
      Key = CurrentKey;
   }
   COLPOSTCONDITION(Key != NULL);
   checkResult(RegSetValueEx(Key, Name.c_str(),0, REG_DWORD, (LPBYTE)&Value, sizeof(Value)));
}

void REGregistry::writeBoolean(const COLstring& Name, bool pValue){
   pMember->writeBoolean(Name, pValue);   
}

void REGregistry::writeInteger(const COLstring& Name, COLint32 Value){
   pMember->writeInteger(Name, Value);
}

void REGregistryPrivate::deleteEntry(const COLstring& Name){
   COL_METHOD(REGregistryPrivate::deleteEntry);
   COL_VAR(Name);
   HKEY Key;
   if (NULL == CurrentKey){
      Key = RootKey;
   } else {
      Key = CurrentKey;
   }
   checkResult(RegDeleteValue(Key, Name.c_str()));
}
   
const COLstring REGregistryPrivate::readString(const COLstring& Name, const char* pDefault, REGregistry::REGstringType* pActualType){
   COL_METHOD(REGregistryPrivate::readString);
   COL_VAR2(Name, pDefault);
   HKEY Key;
   if (NULL == CurrentKey){
      Key = RootKey;
   } else {
      Key = CurrentKey;
   }
   DWORD Type;   
   LONG Result;
   DWORD BufferLength = 0;
   COLstring Buffer;
   //get size   
   Result = RegQueryValueExA(Key, Name.c_str(), NULL, &Type, NULL,&BufferLength);
   //No key found
   if (Result != 0){
      return pDefault;
   }
   //else fetch the key
   //get data
   Buffer.setCapacity(BufferLength+1); 
   Buffer.setSize(BufferLength + 1);
   //fill with nulls
   ::memset((void*)Buffer.data(),0,Buffer.size());
   Result = RegQueryValueExA(Key,Name.c_str(), NULL,&Type,(LPBYTE) (Buffer.c_str()), &BufferLength);
   COLPRECONDITION(Result == 0);
   COLstring FinalBuffer;
   if (REG_SZ != Type && REG_EXPAND_SZ != Type){
      FinalBuffer = pDefault;
   } else {
      FinalBuffer = (const char*)Buffer.c_str();
      if(pActualType){
         // REGstringType constants were set up to match the REG_SZ, REG_EXPAND_SZ values
         *pActualType = (REGregistry::REGstringType) Type;
      }
   }
   return FinalBuffer;
}

bool REGregistryPrivate::subKeyExists(const COLstring& SubKeyPath){
   COL_METHOD(REGregistryPrivate::subKeyExists);
   COL_VAR(SubKeyPath);
   HKEY Key;
   if (NULL == CurrentKey){
      Key = RootKey;
   } else {
      Key = CurrentKey;
   }
   COLPOSTCONDITION(Key != NULL);
   HKEY NewKey;
   DWORD Result = RegOpenKeyEx(Key, SubKeyPath.c_str(), 0, KEY_ALL_ACCESS, &NewKey);
   if (ERROR_FILE_NOT_FOUND == Result){
      closeKey(NewKey);
      return false;
   }
   checkResult(Result);  // will throw an error with a description...
   closeKey(NewKey);
   return true;
}

void REGregistryPrivate::deleteSubKey(const COLstring& SubKeyPath){
   COL_METHOD(REGregistryPrivate::deleteSubKey);
   COL_VAR(SubKeyPath);
   HKEY Key;
   if (NULL == CurrentKey){
      Key = RootKey;
   } else {
      Key = CurrentKey;
   }
   COLPOSTCONDITION(Key != NULL);
   checkResult(RegDeleteKey(Key, SubKeyPath.c_str()));
}

REGregistry::REGregistry(HKEY RootKey){
   pMember = new REGregistryPrivate(RootKey);
}

REGregistry::~REGregistry(){
   delete pMember;
}

void REGregistry::openKey(const COLstring& Key, bool CanCreate, REGSAM AdditionalAccessMask){
   pMember->openKey(Key, CanCreate, AdditionalAccessMask);
}
   
void REGregistry::writeString(const COLstring& Name, const COLstring& Value, REGregistry::REGstringType StringType){
   pMember->writeString(Name, Value, StringType);
}

COLstring REGregistry::readString(const COLstring& Name, const char* pDefault, REGregistry::REGstringType* pActualType) const{
   return pMember->readString(Name, pDefault, pActualType);
}

bool REGregistry::readBoolean(const COLstring& Name, bool Default) const{
   return pMember->readBoolean(Name, Default);
}

HKEY REGregistry::rootKey() const{
   return pMember->RootKey;
}

bool REGregistry::lazyWrite() const{
   return pMember->LazyWrite;
}

void REGregistry::setLazyWrite(bool NewValue){
   pMember->LazyWrite = NewValue;
}

COLint32 REGregistry::readInteger(const COLstring& Name, const COLint32 Default) const{
   return pMember->readInteger(Name, Default);  
}

void REGregistry::moveToFirstEnum(){
   pMember->KeyIndex = 0;
}

void REGregistry::deleteEntry(const COLstring& Name){
   pMember->deleteEntry(Name);
}

bool REGregistry::subKeyExists(const COLstring& SubKeyPath){
   return pMember->subKeyExists(SubKeyPath);
}

void REGregistry::deleteSubKey(const COLstring& SubKeyPath){
   pMember->deleteSubKey(SubKeyPath);
}

void REGregistry::deleteSubKeyRecursive(const COLstring& SubKeyPath){
   HKEY TargetKey;
   pMember->checkResult(RegOpenKey(pMember->CurrentKey, SubKeyPath.c_str(), &TargetKey));
   REGregistry TargetKeyObject(pMember->RootKey);
   TargetKeyObject.pMember->CurrentKey = TargetKey;
   COLstring SubSubKeyPath;
   while (TargetKeyObject.readEnumKey(SubSubKeyPath)){
      TargetKeyObject.deleteSubKeyRecursive(SubSubKeyPath);
   }
   deleteSubKey(SubKeyPath);
   --(pMember->KeyIndex);
}

bool REGregistry::readEnumString(COLstring& Name, COLstring& Value, DWORD* pOutType){
   DWORD Type;
   do
   {
      Name.assign(REGregistryPrivate::REG_MAXIMUM_KEY_NAME_LENGTH, '\0');  // TODO - don't hard code
      DWORD SizeOfBuffer = Name.size();
      DWORD Result = RegEnumValue(pMember->CurrentKey, pMember->KeyIndex, Name.get_buffer(), 
                                  &SizeOfBuffer, NULL, &Type, NULL, NULL);
      if (ERROR_NO_MORE_ITEMS == Result){
         return false;
      }
      pMember->KeyIndex++;
      pMember->checkResult(Result);  // will throw an error with a description...
   }
   while (REG_SZ != Type && REG_EXPAND_SZ != Type);
   Value = readString(Name, "");
   if(pOutType){
      *pOutType = Type;
   }
   return true;
}

bool REGregistry::readEnumValue(COLstring& Name, char* buffer){
   Name.assign(REGregistryPrivate::REG_MAXIMUM_KEY_NAME_LENGTH, '\0');  // TODO - don't hard code
   DWORD SizeOfBuffer = Name.size();
   DWORD Type;
   //IMPORTANT : buffer parameter is IGNORED!
   //Todo: We are not asking for values at this point. No need so far.
   //A more complete implementation should do that if buffer is not NULL..
   DWORD Result = RegEnumValue(pMember->CurrentKey,pMember->KeyIndex, Name.get_buffer(), &SizeOfBuffer,
                               NULL,&Type,NULL,NULL);
   if (ERROR_NO_MORE_ITEMS == Result){
      return false;
   }
   pMember->KeyIndex++;
   pMember->checkResult(Result);  // will throw an error with a description...

   return true;
}

bool REGregistry::readEnumKey(COLstring& Name){
   Name.assign(REGregistryPrivate::REG_MAXIMUM_KEY_NAME_LENGTH, '\0');  // TODO - don't hard code
   DWORD SizeOfBuffer = Name.size();
   DWORD Result = RegEnumKeyEx(pMember->CurrentKey,pMember->KeyIndex,Name.get_buffer(), 
                               &SizeOfBuffer,NULL,NULL,NULL,NULL);
   if (ERROR_NO_MORE_ITEMS == Result){
      return false;
   }
   pMember->KeyIndex++;
   pMember->checkResult(Result);  // will throw an error with a description...
   return true;
}
