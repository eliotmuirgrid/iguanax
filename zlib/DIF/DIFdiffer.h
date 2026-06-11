#ifndef __DIF_DIFFER_H__
#define __DIF_DIFFER_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DIFdiffer
//
// Description:
//
// This is a simple class to see if two binary files are different.
//
// Author: Eliot Muir
// Date:   Fri 01/16/2004 
//
//---------------------------------------------------------------------------


#include <COL/COLminimumInclude.h>

class DIFdifferPrivate;

class DIFdiffer
{
public:
   DIFdiffer();
   ~DIFdiffer();

   bool filesMatch(const COLstring& FileName1,
                         const COLstring& FileName2,
                         COLostream& ErrStream);
private:
   DIFdifferPrivate* pMember;

   DIFdiffer(const DIFdiffer& Orig); // not allowed
   DIFdiffer& operator=(const DIFdiffer& Orig); // not allowed
};

// Convenience method - put's output to stdout.
void DIFcompare(const COLstring& FileName1, const COLstring& FileName2);

#endif // end of defensive include
