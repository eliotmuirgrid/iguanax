#ifndef __TST_FLAGS_H__
#define __TST_FLAGS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTflags
//
// Description:
//
// TSTflags class.
//
// Author: Eliot Muir
// Date:   Wednesday, August 16th, 2006 @ 10:30:05 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class TSTflagsPrivate;

class TSTflags 
{
public:
   TSTflags();
   // This is non virtual because we are assuming no classes will inherit from this class
   ~TSTflags();

   void setFlag(const COLstring& Name, const COLstring& Value);

   bool flagExists(const COLstring& Name) const;
   const COLstring& flagValue(const COLstring& Name) const;

private:
   TSTflagsPrivate* pMember;
   TSTflags(const TSTflags& Orig); // not allowed
   TSTflags& operator=(const TSTflags& Orig); // not allowed
};

#endif // end of defensive include
