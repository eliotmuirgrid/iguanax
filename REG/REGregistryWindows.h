#ifndef __REG_REGISTRY_H__
#define __REG_REGISTRY_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: REGregistry
//
// Description:
//
// Class for accessing the registry.
//
// Author: Eliot Muir
// Date:   Mon 28/06/1999 
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

#ifdef _WIN32
#include <wtypes.h>
#endif

class REGregistryPrivate;
class COLstring;

class REGregistry {
public:
   enum REGstringType{
      REGnone = 0,
      REGdefaultString = 1,
      REGexpandString  = 2
   };
   REGregistry(HKEY RootKey);
   virtual ~REGregistry();
   // Open a registry key using either an absolute or relative registry path.
   // 
   // If KeyPath begins with backslash, it will be interpreted as as an 
   // absolute path under RootKey.  Otherwise, it will be interpreted as
   // a path relative to the current key.
   //
   // If CanCreate is false, and the current user has full registry access
   // then the key is opened and read+writing is possible. If the user has
   // limited registry access, the key is opened in read only, and writing 
   // will throw
   //
   // If CanCreate is true, the above rules are the same, except
   // that the key is created if it does not exist. It will not
   // be created if the user has no write access, and if the key does not
   // already exit, the function will throw.
   //
   // AdditionalAccessMask lets you specify any additional access rights.
   // by default, the access mask used will be KEY_ALL_ACCESS.
   void openKey(const COLstring& KeyPath, bool CanCreate, REGSAM AdditionalAccessMask = 0);

   COLint32 readInteger(const COLstring& Name, const COLint32 Default) const;
   void writeInteger(const COLstring& Name, COLint32 Value);

   bool readBoolean(const COLstring& Name, bool Default) const;
   void writeBoolean(const COLstring& Name, bool pValue);

   void deleteEntry(const COLstring& Name);
   void writeString(const COLstring& Name, const COLstring& Value, REGstringType StringType = REGdefaultString);
   COLstring readString(const COLstring& Name, const char* pDefault, REGstringType* pActualType=0) const; 

   void moveToFirstEnum();

   bool readEnumString(COLstring& Name, COLstring& Value, DWORD* pOutType=0);
   bool readEnumValue(COLstring& Name, char* buffer);
   bool readEnumKey(COLstring& Name);

   HKEY rootKey() const;

   bool lazyWrite() const;
   void setLazyWrite(bool NewValue);

   bool subKeyExists(const COLstring& SubKeyPath);

   // This will delete a subkey only if the subkey has no additional subkeys.
   // If you are enumerating through keys (using readEnumKey) when you call
   // this function, you could get unexpected results (mainly the key
   // appearing after SubKeyPath will be skipped in the enumeration).
   void deleteSubKey(const COLstring& SubKeyPath);

   // This will delete a subkey even if the subkey has additional subkeys.
   void deleteSubKeyRecursive(const COLstring& SubKeyPath);

private:
   REGregistryPrivate* pMember;
   REGregistry(const REGregistry& Orig);
   REGregistry& operator=(const REGregistry& Orig);
};

#endif // end of defensive include
