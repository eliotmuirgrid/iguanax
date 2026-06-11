//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Application: SFIexample 
//
// Description:
//
// SFIexample is a unit test program for the SFI library. 
// 
// Author: Matt Lawrence
// Date:   Friday, August 4th, 2006 @ 09:36:35 AM
//
//---------------------------------------------------------------------------
#include "SFIexamplePrecomp.h"
#pragma hdrstop

#include <CMD/CMDlineParser.h>
#include <TST/TSTconsoleApplication.h>
#include <TST/TSTtestCollection.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <SFIT/SFITbase32.h>
#include <SFIT/SFITbase64.h>
#include <SFIT/SFIThex.h>
#include <SFIT/SFITuri.h>
#include <SFIT/SFITxml.h>
#include <SFIT/SFITjson.h>
#include <SFIT/SFIThtmlCrop.h>
#include <SFIT/SFIThtmlEscape.h>
#include <SFIT/SFITmd5.h>
#include <SFIT/SFITcrypt3.h>
#include <SFIT/SFITcrc.h>
#include <SFIT/SFITrandom.h>
#include <SFIT/SFITevpEncryptDecrypt.h>

int main(int argc, const char** argv){
   try{
      TSTtestCollection Collection("SFI Unit Tests");
      Collection.addTest(SFITbase32());
      Collection.addTest(SFITbase64());
      Collection.addTest(SFIThex());
      Collection.addTest(SFITuri());
      Collection.addTest(SFITxml());
      Collection.addTest(SFITjson());
      Collection.addTest(SFIThtmlCrop());
      Collection.addTest(SFIThtmlEscape());
      Collection.addTest(SFITmd5());
      Collection.addTest(SFITcrypt3());
      Collection.addTest(SFITcrc());
      Collection.addTest(SFITrandom());
      Collection.addTest(SFITevpEncryptDecrypt());

      CMDlineParser LineParser;

      TSTconsoleApplication Application("SFI Unit Tests", Collection, LineParser);
      LineParser.parseArgs(argc,argv);

      if (Application.run()){
         return EXIT_SUCCESS;
      }
      else{
         return EXIT_FAILURE;
      }
   } catch (const COLerror& Error){
      COLcerr << Error.description() << newline;
      return EXIT_FAILURE;
   } catch (...){
      COLcerr << "Unhandled Exception" << newline;
      return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
}
