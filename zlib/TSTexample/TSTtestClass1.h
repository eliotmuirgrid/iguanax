#ifndef __TST_TEST_CLASS_1_H__
#define __TST_TEST_CLASS_1_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2005 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestClass1
//
// Description: Some String Tests to be used in TSTexampleMain
//
// Author: Nasron Cheong
// Date:   Febuary 2004 
//
//---------------------------------------------------------------------------

class TSTtestable;

//A single function that tests appending a single character
void appendSingleCharacter();

//A single function that tests appending a single character
void appendString();

//creates a test collection with all our tests 
//and functions
TSTtestable* tests1();

#endif   // end of defensive include
