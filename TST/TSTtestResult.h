#ifndef __TST_TEST_RESULT_H__
#define __TST_TEST_RESULT_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestResult
//
// Description: To avoid having multiple result classes per TSTtestable type,
// this class is used. It shows the names and messages of TSTtestables, but 
// not the TSTtype of the test (which is usually not of much interest anyway).
//
// Author: Nasron Cheong
// Date:   Feb 2004 
//
//---------------------------------------------------------------------------

class TSTtestResultPrivate;

class TSTtestResult
{
public:
   TSTtestResult();
   virtual ~TSTtestResult();
   TSTtestResult(const TSTtestResult& Orig);
   TSTtestResult& operator=(const TSTtestResult& Orig);

   bool succeeded() const;
   void setSucceeded(bool Succeeded);

   //the test that was run
   const COLstring& testName() const;
   void setTestName(const COLstring& TestName);

   //message regarding the result of the test.
   const COLstring& message() const;
   void setMessage(const COLstring& Message);

   COLuint32 countOfSubTestResult() const;
   TSTtestResult& subTestResult(COLuint32 TestIndex) const;
   TSTtestResult& push_back();
   void clear();

   COLuint32 countOfFailure() const;

private:
   TSTtestResultPrivate* pMember;
}; 

void TSTmergeResults(const TSTtestResult& Source, TSTtestResult& Target);

#endif   // end of defensive include
