#ifndef __CMD_TEST_H__
#define __CMD_TEST_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMDtest.h
//
// Description:
//
// A set of tests of the CMD library.
//
// Author: Roman Leifer
// Date:   Tue 08/17/2004
//
//---------------------------------------------------------------------------

class TSTtestable;
class CMDlineParser;
class CMDprogressMeter;

#include <CMDexample/CMDtestStringArray.h>

class CMDtest
{
public:
   CMDtest() {};
   virtual ~CMDtest() {};

   static CMDtest* create(){ return new CMDtest; }

   static TSTtestable* tests();

   void parseArgsTest();
   void parseFileListTest();
   void showUsageTest();
   void helpFlagTest();
   void progressMeterTest();

private:
   void addCommandLine(const COLstring& CommandLine);
   COLarray<CMDtestStringArray>   ArgMatrix; 
   
   CMDtest(const CMDtest& );  
   CMDtest& operator=(const CMDtest& );
};

TSTtestable* CMDTlineParserTests();

#endif     // end of defensive include
