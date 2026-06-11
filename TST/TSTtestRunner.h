#ifndef __TST_TEST_RUNNER_H__ 
#define __TST_TEST_RUNNER_H__ 
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestRunner
//
// Description: Runs a given test, and writes the test result into a TSTtestResult
//
// Author: Nasron Cheong
// Date:   Feb 2004 
//
//---------------------------------------------------------------------------
#include <COL/COLminimumInclude.h>

class TSTtestRunnerPrivate;
class TSTtestResult;
class TSTtestable;
class TSTtestSpecification;
class TSTflags;

class TSTtestRunner
{
public:
   TSTtestRunner();
   virtual ~TSTtestRunner();

   //indicates if all tests succeeded or failed
   //function runs all tests starting at Test
   bool runAll(TSTtestable& Test, TSTtestResult& Result, const TSTflags& Flags, COLuint32 MaxDepth = (COLuint32)-1);

   //allows running of tests given by Test, and the specification.
   //exception is thrown if the specification does not resolve properly to the test
   bool runSpecifiedTests(TSTtestable& Test, TSTtestResult& Result, const TSTtestSpecification& Specification, const TSTflags& Flags);

   //convenience function, which uses a TSTspecificationParser for the string
   bool runSpecifiedTestsInString(TSTtestable& Test, TSTtestResult& Result, const COLstring& Specification, const TSTflags& Flags);

   //Events
   //fired on each test being run that is not a collection. 
   virtual void onNextTest(const COLstring& TestPath, const COLstring& TestName){};

private:
   bool runAll(const COLstring& TestPath, TSTtestable& Test, TSTtestResult& Result, const TSTflags& Flags, COLuint32 MaxDepth = (COLuint32)-1);

   TSTtestRunnerPrivate* pMember;
   TSTtestRunner(const TSTtestRunner& Orig); 
   TSTtestRunner& operator=(const TSTtestRunner& Orig);   
};

bool TSTverifyResultSuccess(TSTtestResult& Result);

#endif // end of defensive include
