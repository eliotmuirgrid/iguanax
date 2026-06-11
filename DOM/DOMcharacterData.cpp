//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOMcharacterData
//
// Description:
//
// Author: Rob Moyse
// Date:   November 9, 2004
//
//
//---------------------------------------------------------------------------
#include "DOMpreCompiled.h"
#pragma hdrstop

#include <DOM/DOMcharacterData.h>

class DOMcharacterDataPrivate
{
public:
   DOMcharacterDataPrivate() {}
   virtual ~DOMcharacterDataPrivate() {}

   const char* pData;
   COLuint32 DataLength;
   COLstring DataString;
};

DOMcharacterData::DOMcharacterData
(
   DOMnode* pParentNode, 
   DOMdocument* pDocument,
   const char* pData,
   COLuint32 DataLength
) 
: DOMnode(pParentNode, pDocument)
{
   pMember = new DOMcharacterDataPrivate();

   pMember->pData = pData;
   pMember->DataLength = DataLength;
}

DOMcharacterData::DOMcharacterData
(
   DOMnode* pParentNode, 
   DOMdocument* pDocument,
   const COLstring& Data
)
: DOMnode(pParentNode, pDocument)
{
   pMember = new DOMcharacterDataPrivate();

   pMember->DataString = Data;
   pMember->pData = pMember->DataString.c_str();
   pMember->DataLength = Data.size();
}

DOMcharacterData::~DOMcharacterData()
{
   delete pMember;
}

const char* DOMcharacterData::nodeValue(COLuint32* pLength) const
{
   *pLength = pMember->DataLength;
   return pMember->pData;
}

COLstring DOMcharacterData::nodeValue() const
{
   return COLstring(pMember->pData, pMember->DataLength);
}

void DOMcharacterData::setNodeValue(const char* Value, COLuint32 Length)
{
   pMember->pData = Value;
   pMember->DataLength = Length;
}

void DOMcharacterData::setNodeValue(const COLstring& Value)
{
   pMember->DataString = Value;
   pMember->pData = pMember->DataString.c_str();
   pMember->DataLength = Value.size();
}

COLstring DOMcharacterData::substringData(COLuint32 Offset, COLuint32 Count)
{
   ensureDataStringPopulated();
   return pMember->DataString.substr(Offset, Count);
   updateFromDataString();
}

void DOMcharacterData::appendLength(COLuint32 ExtraLength)
{
   pMember->DataLength += ExtraLength;
}

void DOMcharacterData::appendData(const COLstring& Data)
{
   ensureDataStringPopulated();
   pMember->DataString += Data;
   updateFromDataString();
}

void DOMcharacterData::insertData(COLuint32 Offset, const COLstring& Data)
{
   ensureDataStringPopulated();
   pMember->DataString.insert(Offset, Data);
   updateFromDataString();
}

void DOMcharacterData::deleteData(COLuint32 Offset, COLuint32 Count)
{
   ensureDataStringPopulated();
   pMember->DataString.remove(Offset, Count);
   updateFromDataString();
}

void DOMcharacterData::replaceData(COLuint32 Offset, COLuint32 Count, const COLstring& Data)
{
   ensureDataStringPopulated();
   pMember->DataString.replace(Offset, Count, Data);
   updateFromDataString();
}

void DOMcharacterData::ensureDataStringPopulated()
{
   if (pMember->DataString == "")
   {
      pMember->DataString = COLstring(pMember->pData, pMember->DataLength);
   }
}

void DOMcharacterData::updateFromDataString()
{
   pMember->pData = pMember->DataString.c_str();
   pMember->DataLength = pMember->DataString.size();
}
