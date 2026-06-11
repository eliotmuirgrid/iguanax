//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLsinkNull
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Fri 01/16/2004 
//---------------------------------------------------------------------------
#include "COLsinkNull.h"

// These have to go here because we want to export this class in a DLL
COLsinkNull::COLsinkNull()
{
   // do nothing
}

COLsinkNull::~COLsinkNull()
{
   // do nothing
}

COLuint32 COLsinkNull::write(const void* cpBuffer, COLuint32 SizeOfBuffer)
{
   // do nothing. - we do this because other wise the compiler links the symbol
   // in multiple times which causes problems when COLsink is in DLL.  Hope there
   // is a way around this problem.

   return SizeOfBuffer;
}
COLuint32 COLsinkNull::writeString(const COLstring& String) {
   return 0;
}
