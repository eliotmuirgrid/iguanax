#ifndef __TST_TEST_FORMATTER_H__
#define __TST_TEST_FORMATTER_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestFormatter
//
// Description: A class that prints the structure of a TSTtestable. It typechecks 
// and recurses through collections.
//
// Author: Nasron Cheong
// Date:   Feb 2004 
//
//---------------------------------------------------------------------------
class TSTtestFormatterPrivate;
class TSTtestable;
class COLsink;

//base class will format results as a normal string
class TSTtestFormatter
{
public:
   TSTtestFormatter();
   virtual ~TSTtestFormatter();

   virtual void printOn(TSTtestable& Test, COLsink& Sink);
   virtual void printFull(TSTtestable& Test, COLsink& Sink);

private:
   TSTtestFormatterPrivate* pMember; 
   TSTtestFormatter(const TSTtestFormatter& Orig);
   TSTtestFormatter& operator=(const TSTtestFormatter& Orig);
};

#endif   // end of defensive include
