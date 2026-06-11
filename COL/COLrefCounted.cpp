//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLrefCounted
//
// Description:
//
// COLrefCounted implementation
//
// Author: Eliot Muir
// Date:   Tue 08/25/1998 
//---------------------------------------------------------------------------
#include "COLrefCounted.h"
#include "COLerror.h"
#include "COLlog.h"
COL_LOG_MODULE;

COLrefCounted::COLrefCounted() : m_CountOfRef(0){
   COL_METHOD(COLrefCounted::COLrefCounted);
}

COLrefCounted::~COLrefCounted(){
   COL_METHOD(COLrefCounted::~COLrefCounted);
   if (0 != m_CountOfRef){
      COL_ABORT("Reference count = " << m_CountOfRef << " for this = " << this);
   }
//   COLASSERT(0 == m_CountOfRef); // andrew: we do not want a COLPRECONDITION here
}

void COLrefCounted::AddRef() {
   COL_METHOD(COLrefCounted::AddRef);
   m_CountOfRef++;
}

void COLrefCounted::Release(){
   COL_METHOD(COLrefCounted::Release);
   COLASSERT(m_CountOfRef > 0);
   m_CountOfRef--;
   if (0 == m_CountOfRef) {
      delete this;  // good bye cruel world...!
   }
}

// TODO - This seems really unsafe...?
// Windows compiler needs this private method implementation for some reason.
//COLrefCounted::COLrefCounted(const COLrefCounted& Original){
//   m_CountOfRef = Original.m_CountOfRef;
//}

