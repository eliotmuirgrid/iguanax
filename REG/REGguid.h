#ifndef __REG_GUID_H__
#define __REG_GUID_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: REGguid
//
// Description:
//
// This is a wrapper for the windows generate GUID function
//
// Author: Eliot Muir
// Date:   Mon 28/06/1999 
//
//---------------------------------------------------------------------------

struct REGguid
{
   REGguid() 
   : Data1(0), 
     Data2(0),
     Data3(0) 
   {}  // Warning leaving Data array uninitialized.
   COLuint32  Data1;
   COLuint16  Data2;
   COLuint16  Data3;
   COLuint8   Data[8];
};

enum REGguidResult
{
   REG_GUID_SUCCESS=0,    // no problem
   REG_GUID_LOCAL_ONLY=1, // only guaranteed to be locally  
   REG_GUID_FAILED=2      // call failed
};

enum REGparseResult
{
   REG_PARSE_OK=1,
   REG_ILLEGAL_CHARACTER=-1
};

COLstring REGguidToString(const REGguid& Guid);

// #8538 Warning - the GUID here doesn't use terribly strong
// randomness - for some applications you might need something
// more elaborate - read the discussion in the ticket - Eliot
REGguidResult REGcreateGuid(REGguid& Guid);
bool REGstringToGuid(const COLstring& String, REGguid& Guid);

bool REGguidMatch(const REGguid& First, const REGguid& Second);

#endif // end of defensive include
