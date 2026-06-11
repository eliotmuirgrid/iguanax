#ifndef __TST_TESTABLE_OPTIONS_H__ 
#define __TST_TESTABLE_OPTIONS_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2005 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestableOptions
//
// Description: Describes how to treat a testable class when the test is running 
//
// Author: Nasron Cheong
// Date:   Feb 2004 
//
//---------------------------------------------------------------------------

class TSTtestableOptionsPrivate;
#include <COL/COLminimumInclude.h>


class TSTtestableOptions
{
public:
   TSTtestableOptions();
   virtual ~TSTtestableOptions();

   //resets to initialized
   void clear();

   //if testable is a collection, all contents are run if this is set.
   //if testable is not a collection, it is run if this is set
   void setRunAll(bool Enable); 
   bool runAll() const;

   //if the testable is a collection, this allows the control over what 
   //contents should be used. runAll must be false for this to 
   //have any effect.
   void addSubTest(const COLstring& SubTest);
   COLuint32 const countOfSubTest() const;
   const COLstring& subTest(COLuint32 TestIndex) const;

private:
   TSTtestableOptionsPrivate* pMember;
   TSTtestableOptions(const TSTtestableOptions& Orig);
   TSTtestableOptions& operator=(const TSTtestableOptions& Orig);
};

#endif // end of defensive include


