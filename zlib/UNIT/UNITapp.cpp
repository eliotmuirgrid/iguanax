//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: UNITapp
//
// Author: Eliot Muir
//
// Date: Mon 15 Mar 2021 08:40:28 EDT
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------
#include "UNITapp.h"
#include "UNITfilter.h"
#include "UNITlastFailed.h"
#include "UNITrun.h"
#include "UNITlistTests.h"

#include <CMD/CMDlineParser.h>

#include <SCK/SCKutils.h> // needed for windows.

#include <COL/COLlog.h>
COL_LOG_MODULE;

void UNITapp::add(const COLstring& Name, UNITtest pFunc){
   COL_METHOD(UNITapp::add);
   if (m_TestCollection.count(Name) > 0){
      COLcout << "Test: " << Name + " already defined.  Please select a unique name." << newline;
      abort();
   }
   m_TestCollection.add(Name, COLnewClosure0(pFunc));
}

void UNITapp::addClosure(const COLstring& Name, COLclosure0* pFunc){
   COL_METHOD(UNITapp::addClosure);
   if (m_TestCollection.count(Name) > 0){
      COLcout << "Test: " << Name + " already defined.  Please select a unique name." << newline;
      abort();
   }
   m_TestCollection.add(Name, pFunc);
}

bool UNITinit(int argc,const char** argv, COLmap<COLstring, COLauto<COLclosure0> >* pTestCollection){
   COL_FUNCTION(UNITinit);
   SCKinitWinsock();
   CMDlineParser LineParser;
   LineParser.addFlagWithArgument("test", "Glob expression to use.");
   LineParser.addFlagWithoutArgument("lastfailed", "Rerun last failed tests from lastfailed.tmp file.");
   LineParser.addFlagWithoutArgument("showtests", "List the tests availale.");
   LineParser.parseArgs(argc,argv);
   if (LineParser.parsingErrorsPresent(COLcout)){
      LineParser.showUsage(COLcout);
      return false;
   }
   if (LineParser.isFlagPresent("showtests")){
      UNITlistTests(*pTestCollection);
      return false;
   }
   if (LineParser.isFlagPresent("test")){
      UNITfilterTests(LineParser.flagArgument("test"), pTestCollection);
      if (pTestCollection->size() == 0){
         COLcout << "No tests matched specification.  Use --showtests to show the available tests." << newline;
         return false;
      }
   }
   if (LineParser.isFlagPresent("lastfailed")){
      UNITfilterFailedTests(pTestCollection);
   }
   return true;
}

int UNITapp::run(int argc,const char** argv){
   COL_METHOD(UNITapp::run);
   try{
       if (!UNITinit(argc, argv, &m_TestCollection)){
          return EXIT_FAILURE;
       }
       COLcout << "Running with threads " << m_TestCollection.size() << " test" << (m_TestCollection.size() != 1 ? "s." : ".") << newline;
       return UNITrunTestCollection(m_TestCollection);
   }catch(COLerror& Error){
       COLcout << Error << newline;
       return EXIT_FAILURE;
   }catch (...){
       COLcout << "Unhandled exception." << newline;
       return EXIT_FAILURE;
   }
}

int UNITapp::runSingleThreaded(int argc,const char** argv){
   COL_METHOD(UNITapp::runSingleThreaded);
   try{
       if (!UNITinit(argc, argv, &m_TestCollection)){
          return EXIT_FAILURE;
       }
       COLcout << "Running on single thread " << m_TestCollection.size() << " test" << (m_TestCollection.size() != 1 ? "s." : ".") << newline;
       return UNITrunSingleThreadedTestCollection(m_TestCollection);
   }catch(COLerror& Error){
       COLcout << Error << newline;
       return EXIT_FAILURE;
   }catch (...){
       COLcout << "Unhandled exception." << newline;
       return EXIT_FAILURE;
   }
}
