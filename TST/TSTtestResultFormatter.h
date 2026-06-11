#ifndef __TST_TEST_RESULT_FORMATTER_H__
#define __TST_TEST_RESULT_FORMATTER_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTresultFormatter
//
// Description: Formats a Result object into a string.
//
// Author: Nasron Cheong
// Date:   Feb 2004 
//
//---------------------------------------------------------------------------
class TSTtestResultFormatterPrivate;
class TSTtestResult;
class COLsink;

//base class will format results as a normal string
class TSTtestResultFormatter
{
public:
   TSTtestResultFormatter();
   virtual ~TSTtestResultFormatter();

   virtual void printOn(TSTtestResult& TestResult, COLsink& Sink);

   void showFailures(TSTtestResult& TestResult, COLostream& Sink);
private:
   TSTtestResultFormatterPrivate* pMember; 
   TSTtestResultFormatter(const TSTtestResultFormatter& Orig);
   TSTtestResultFormatter& operator=(const TSTtestResultFormatter& Orig);
};

void TSTshowRunnableFailures
(
   const TSTtestResult&   TestResult,
   COLostream&      Stream
);

#endif   // end of defensive include
