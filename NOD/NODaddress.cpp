//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODaddress
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday, June 16th, 2010 @ 11:40:58 AM
//
//---------------------------------------------------------------------------
#include "NODprecomp.h"
#pragma hdrstop

#include "NODaddress.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

NODaddress::NODaddress() {
   COL_METHOD(NODaddress::NODaddress);
}

NODaddress::~NODaddress() {
   COL_METHOD(NODaddress::~NODaddress);
}

int NODaddress::depth() const {
   return m_Address.size();
}

int NODaddress::addressIndex(int i) const {
   return m_Address[i];
}

void NODaddress::setParent(NODplace* pNewValue) {
   COL_METHOD(NODaddress::setParent);
   COL_VAR2(m_pParent, pNewValue);
   m_pParent = pNewValue; 
}

void NODaddress::pushAddressIndex(int AddressIndex) {
   m_Address.push_back(AddressIndex);
}

COLostream& operator<<(COLostream& Stream, const NODaddress& Address){
   NODoutputAddress(Stream, "", Address);
   return Stream;
}

static void NODoutputNameAccessor(COLostream& Stream, const COLstring& Name) {
   COL_FUNCTION(NODoutputNameAccessor);
   // TODO - we ought to check for more illegal characters and escape them.
   COL_VAR(Name);
   if (Name.find(" ") != npos) {
      Stream << COL_T("['") << Name << COL_T("']");
   } else {
      Stream << '.' << Name;
   }
}

const NODplace& NODoutputAddress(COLostream& Stream, const COLstring& SourceName, const NODaddress& Address) {
   COL_FUNCTION(NODoutputAddress);
   COL_VAR(SourceName);
   Stream << SourceName;
   const NODplace* pPlace = Address.parent();
   for (int i=0; i < Address.depth(); i++) {
      pPlace = &NODoutputAddressIndex(Stream, Address.addressIndex(i), *pPlace);
   }
   return *pPlace;
}

const NODplace& NODoutputAddressIndex(COLostream& Stream, int AddressIndex, const NODplace& Place) {
   COL_FUNCTION(NODoutputAddressIndex);
   const NODplace& Child = Place.child(AddressIndex-1);
   if (Place.grammarType() == NOD_COMPLEX) {
      NODoutputNameAccessor(Stream, Child.name());
   } else {
      Stream << '[' << AddressIndex << ']';
   }
   return Child;
}
