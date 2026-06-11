#ifndef __DOM_EXAMPLE_TESTS_H__
#define __DOM_EXAMPLE_TESTS_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2004 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOMexampleTests.h
//
// Description:
//
// A set of utilities to test the DOM
//
// Author: Rob Moyse
// Date:   November 11, 2004
//
//
//---------------------------------------------------------------------------

class TSTtestable;

class DOMexampleTestsPrivate;

class DOMexampleTests
{
public:

   DOMexampleTests();
   virtual ~DOMexampleTests();

   // create a pointer to a new testDOM object
   static DOMexampleTests* create(){ return new DOMexampleTests(); }

   void loadDocumentOne();

   void traversePatientNames();
   void traversePatientTests();

   void modifyPatientTest();
   void modifyPatientTestAppend();
   void modifyPatientTestAdd();

   void saveDocumentOne();

   void isWellFormed();

   static TSTtestable* allTests();

private:

   DOMexampleTestsPrivate* pMember;

   DOMexampleTests(const DOMexampleTests& ArrayTest);  
   DOMexampleTests& operator=(const DOMexampleTests& ArrayTest);
   
};

#endif   // end of defensive include
