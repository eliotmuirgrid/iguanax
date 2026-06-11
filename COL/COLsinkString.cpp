//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLsinkString
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Sunday, December 23rd, 2012 @ 10:13:03 PM
//---------------------------------------------------------------------------
#include "COLsinkString.h"

#include "COLlog.h"
COL_LOG_MODULE;

COLsinkString::COLsinkString()
: m_pString(new COLstring()), m_IsOwner(true) {
}

COLsinkString::COLsinkString(COLstring& StringRef)
: m_pString(&StringRef), m_IsOwner(false) {
}

COLsinkString::COLsinkString(COLstring* pString)
: m_pString(pString), m_IsOwner(true) {
}

COLsinkString::COLsinkString(COLstring* pString, bool IsOwner)
: m_pString(pString), m_IsOwner(IsOwner) {
}

COLsinkString::~COLsinkString(){
   if (m_IsOwner){
      delete m_pString;
   }
}

COLuint32 COLsinkString::write(const void* cpBuffer, COLuint32 SizeOfBuffer){
   m_pString->append((const char*)cpBuffer, SizeOfBuffer);
   return SizeOfBuffer;
}

COLuint32 COLsinkString::writeString(const COLstring& String){
   return this->write(String.get_buffer(), String.size());
}

void COLsinkString::flush() {
}
