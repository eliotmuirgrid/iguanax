#ifndef __TST_TEST_CLASS_2_H__
#define __TST_TEST_CLASS_2_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2005 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestClass2
//
// Description: Defines some string functions to be tested in TSTexampleMain 
//
// Author: Nasron Cheong
// Date:   Febuary 2004 
//
//---------------------------------------------------------------------------

#include <COL/COLarray.h>
#include <COL/COLstring.h>


class TSTtestable;


//this demonstrates how to use a fixture
//A test fixture is a single instance of a class where several test methods of the 
//class are executed on that one instance. This allows state to be kept between
//test function calls

class TSTtestClass2 
{
public:
   TSTtestClass2(){};
   virtual ~TSTtestClass2(){};

   //this static function is required by the fixture, so it 
   //knows how to create the instance for testing.
   static TSTtestClass2* create(){return new TSTtestClass2; };

   //Pushes back a string into a vector
   void pushBackString();

   //pops the string
   void popString();

   //creates a test collection with all our tests 
   //and functions
   static TSTtestable* tests2();

private:
   
   COLarray<COLstring> StringVector;
   
};

#endif   // end of defensive include
