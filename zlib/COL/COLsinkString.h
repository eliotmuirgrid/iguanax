#ifndef __COL_SINK_STRING_H__
#define __COL_SINK_STRING_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLsinkString
//
// Description:
//
// COLstring sink object.  The data gets written into a COLstring.  Originally
// COLsink was implemented directly into COLstring - which had the negative of
// putting vtable overhead into COLstring directly.
//
// Author: Eliot Muir
// Date:   Wed 08/22/2012 
//---------------------------------------------------------------------------

#include "COLsink.h"
#include "COLstring.h"

class COLsinkString : public COLsink
{
public:
   COLsinkString();
   COLsinkString(COLstring& StringRef);
   COLsinkString(COLstring* pString);
   COLsinkString(COLstring* pString, bool IsOwner);

   virtual ~COLsinkString();

   virtual COLuint32 write(const void* cpBuffer, COLuint32 SizeOfBuffer);
   virtual COLuint32 writeString(const COLstring& String);
   virtual void flush();

   COLstring& string()             { return *m_pString; }
   const COLstring& string() const { return *m_pString; }

private:
   COLstring* m_pString;
   bool m_IsOwner;

   COLsinkString(const COLsinkString& Orig); // not allowed
   COLsinkString& operator=(const COLsinkString& Orig); // not allowed
};

#endif // end of defensive include

