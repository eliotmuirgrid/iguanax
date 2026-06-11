//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTconsoleApplication
//
// Description:
//
// Implementation
//
// Author: Nasron Cheong
// Date:   March 2004 
//
//---------------------------------------------------------------------------

#include "TSTprecomp.h"
#pragma hdrstop

#include <COL/COLarray.h>
#include <COL/COLdateTime.h>
#include <COL/COLutils.h>
#include <CMD/CMDlineParser.h>

#include "TSThelpers.h"
#include "TSTtestRunner.h"
#include "TSTtestFormatter.h"
#include "TSTtestResultFormatter.h"
#include "TSTconsoleApplication.h"
#include "TSTtestCollection.h"
#include "TSTflags.h"

const COLstring TST_ARG_TEST = "test"; // (formerly --testspec)
const COLstring TST_ARG_TESTFILE = "testfile";
const COLstring TST_ARG_GREP = "grep"; // run tests matching the given substring
const COLstring TST_ARG_GLOB = "glob"; // run tests matching the given glob pattern
const COLstring TST_ARG_NOCASE = "nocase"; // ignore case when doing grep
const COLstring TST_ARG_SHOWTESTS = "showtests"; 
const COLstring TST_ARG_SHOWTREE = "showtree";
const COLstring TST_ARG_LASTFAILED = "lastfailed";

const COLstring TST_LAST_FAILED_FILE_NAME("lastfailed.tmp");
typedef COLarray<COLstring> TTestSpecList;

void TSTloadTestSpecsFromFile(const COLstring& FileName, TTestSpecList& TestSpecList)
{
   TestSpecList.clear();
   if (!TSTfileExists(FileName))
   {
      return;
   }
   TSTfileReadLines(FileName,TestSpecList);
   
   {//strip chars 
      for (TTestSpecList::iterator Iter = TestSpecList.begin();
           Iter != TestSpecList.end(); ++Iter)
      {
         Iter->stripAll('\r');
         Iter->stripAll('\n');
         Iter->stripAll('"');
      }
   }
   {//remove blank lines
      COLuint32 LineIndex = 0;
      while(LineIndex < TestSpecList.size())
      {
         if (TestSpecList[LineIndex].is_null())
         {
            TestSpecList.remove(LineIndex);
         }
         else
         {
            LineIndex++;
         }
      }
   }
}

void TSTlistTests(COLarray<COLstring>& TestList, TSTtestable& Test, const COLstring& Prefix = "", const TSTtestCollection* ParentTest = 0)
{
   COLstring NextPrefix = Prefix;
   if (Test.testType().isDerivedFrom(TSTtypeTestCollection::instance()))
   {
      TSTtestCollection* pCollection = (TSTtestCollection*)&Test;
      COLstring Parentage = ParentTest ? Prefix + pCollection->testName() + '/' : "";
      for (COLuint32 TestIndex = 0; TestIndex < pCollection->countOfTest(); TestIndex++)
      {
         TSTlistTests(TestList, *pCollection->test(TestIndex), Parentage, pCollection);
      }
   }
   else if (Test.testType().isDerivedFrom(TSTtypeTestable::instance()))
   {
      COLstring TestString;
      COLostream TestStream(TestString);
      TestStream << Prefix << '[' << Test.testName() << ']';
      TestList.push_back() = TestString;
         
   }
   else
   {
      COL_ERROR_STREAM("Invalid Type", COLerror::PreCondition);
   }
}


void TSTwriteLastFailedFile(const COLstring& FileName, const TSTtestResult& TestResult)
{
   COLstring ResultString;
   COLostream ResultStream(ResultString);
   TSTshowRunnableFailures(TestResult,ResultStream);
   TSTfileWrite(FileName,ResultString);
}

class TSTconsoleTestRunner : public TSTtestRunner
{
public:
   TSTconsoleTestRunner(const COLstring& iAppName): AppName(iAppName){};
   virtual ~TSTconsoleTestRunner(){};

protected:
   virtual void onNextTest(const COLstring& TestPath, const COLstring& TestName)
   {
      // display unit tests being run as log-style entries
      // with timestamp, FOOexample and full testspec
      // so it's possible to tell without looking any further
      // what failed and when

      COLdateTime DateTime = COLdateTime::currentTime();
      COLcout 
         << "[" << DateTime.format("%Y/%m/%d %H:%M:%S") << "] [" << AppName << "] "
         << "Running: " << TestPath << "/[" << TestName << "]" << flush; 

   }
   COLstring AppName;
};


struct TSTconsoleApplicationPrivate
{
   TSTconsoleApplicationPrivate(CMDlineParser& iLineParser,TSTtestCollection& iCollection)
      : LineParser(iLineParser), Collection(iCollection)
   {
   }
      
   CMDlineParser& LineParser;
   TSTtestCollection& Collection;
   COLstring AppName;
};

TSTconsoleApplication::TSTconsoleApplication(const COLstring& iAppName,TSTtestCollection& Collection, CMDlineParser& LineParser)
{
   pMember = new TSTconsoleApplicationPrivate(LineParser,Collection);
   pMember->AppName = iAppName; 

   LineParser.addFlagWithArgument(TST_ARG_TEST, "testSpecification", "Only run the specified test" );  
   LineParser.addFlagWithArgument(TST_ARG_TESTFILE,"Runs a list of unit tests specified in the file. One test per line, in the same format as --test");
   LineParser.addFlagWithArgument(TST_ARG_GREP, "Run all tests that contain the given substring");
   LineParser.addFlagWithArgument(TST_ARG_GLOB, "Run all tests that contain the given glob");
   LineParser.addFlagWithoutArgument(TST_ARG_NOCASE, "Ignore case for grep or glob");
   LineParser.addFlagWithoutArgument(TST_ARG_SHOWTREE, "Shows the test tree");
   LineParser.addFlagWithoutArgument(TST_ARG_SHOWTESTS, "Show test names in --test form");
   LineParser.addFlagWithoutArgument(TST_ARG_LASTFAILED, "Reruns the tests that failed last.");
   LineParser.setDescription(Collection.testName());
}

TSTconsoleApplication::~TSTconsoleApplication()
{
   delete pMember;
}


bool TSTconsoleApplication::run()
{
   // This is for backward compatibility
   TSTflags Flags;
   return runWithFlags(Flags);
}

COLstring TSTprogramNameOnly(const COLstring& ProgramName)
{
   // figure out the program name sans path or extension,
   // e.g. /what/ever/FOOexample.exe -> FOOexample
   // for the sake of using it as part of log-style entries 
   // displayed before running each unit test above

   COLstring NameOnly = ProgramName;

   NameOnly.replace("\\", "//"); // fix slashes to be unix style for splitting purposes

   COLstring BeforeSlash, AfterSlash;
   while(NameOnly.split(BeforeSlash, AfterSlash, "/"))
   {
      NameOnly = AfterSlash;
   }
   
   COLstring BeforeDot, AfterDot;
   if(NameOnly.split(BeforeDot, AfterDot, "."))
   {
      NameOnly = BeforeDot;
   }
   return NameOnly;
}

bool TSTrunTestList(TSTtestResult& SummaryResult, const TTestSpecList& TestSpecList, TSTtestCollection& FullCollection, TSTtestRunner& TestRunner, const TSTflags& Flags)
{
   bool OverallSuccess = true;

   for (TTestSpecList::const_iterator Iter = TestSpecList.begin();
        Iter != TestSpecList.end(); ++Iter)
   {
      TSTtestResult SingleTestResult;
      COLcout << newline << "Using Test: " << *Iter << newline;
      bool ThisSucceeded = TestRunner.runSpecifiedTestsInString(FullCollection,
                                                                      SingleTestResult,
                                                                      *Iter, 
                                                                      Flags);
      
      OverallSuccess = OverallSuccess && ThisSucceeded;
      //merge in results with main tree
      TSTmergeResults(SingleTestResult, SummaryResult);
   }
   return OverallSuccess;
}

bool TSTconsoleApplication::runWithFlags(const TSTflags& Flags)
{
   if (pMember->LineParser.isFlagPresent(TST_ARG_SHOWTREE))
   {
      COLcout << newline << "Tests: " << newline;
      TSTtestFormatter TestFormatter;
      TestFormatter.printOn(pMember->Collection,*COLcout.sink());
      COLcout << newline << flush;
      return true;
   }

   if (pMember->LineParser.isFlagPresent(TST_ARG_SHOWTESTS) &&
      !pMember->LineParser.isFlagPresent(TST_ARG_GREP) &&
      !pMember->LineParser.isFlagPresent(TST_ARG_GLOB))
   {
      // note that we're not doing this when --grep or --glob is specified -
      // when --grep or --glob is specified, we fall through to the grep branch,
      // list the tests while searching for them, and then quit if we see the "showtests" flag
      COLcout << "\ntests: \n\n";
      TSTtestFormatter TestFormatter;
      TestFormatter.printFull(pMember->Collection,*COLcout.sink());
      COLcout << newline << flush;
      return true;
   }

   if (pMember->LineParser.parsingErrorsPresent(COLcout))
   {
      pMember->LineParser.showUsage(COLcout);
      // Print out a little section on the proper usage of the --testspec flag and try to make it fit
      // with the "sectioned" look of the showusage output.
      COLcout << "Using the --" << TST_ARG_TEST << " flag:" << newline;
      COLcout << "  Sample: ";
      generateExampleTestSpec(COLcout);
      return true;
   }

   //run the tests
   TSTconsoleTestRunner TestRunner( TSTprogramNameOnly(pMember->LineParser.programName()) );
   TSTtestResult TestResult;
   bool Succeeded = false;
      
   if (pMember->LineParser.isFlagPresent(TST_ARG_TEST))
   {
      try
      {
         COLstring TestSpec;
         pMember->LineParser.flagArgument(TST_ARG_TEST, TestSpec);
         COLcout << newline << "Using Test Specification: " << TestSpec << newline;
         Succeeded = TestRunner.runSpecifiedTestsInString(pMember->Collection,TestResult,TestSpec, Flags);
      }
      catch(COLerror& Error)
      {
         COLcout << "Bad --" << TST_ARG_TEST << " argument: " << Error.description() << newline << newline;
         COLcout << "The available tests are:" << newline;
         TSTtestFormatter TestFormatter;
         // TestFormatter.printOn(pMember->Collection,*COLcout.sink());
         // showing tests, not tree, when invalid test(s) specified
         TestFormatter.printFull(pMember->Collection,*COLcout.sink());
         COLcout << newline;
         COLcout << "--" << TST_ARG_TEST << " usage:" << newline;
         
         generateExampleTestSpec(COLcout);    
         return false;
      }
   }
   else if (pMember->LineParser.isFlagPresent(TST_ARG_GREP)
         || pMember->LineParser.isFlagPresent(TST_ARG_GLOB))
   {
      // true means grep, false means glob
      bool IsGrep = pMember->LineParser.isFlagPresent(TST_ARG_GREP);
      COLstring Pattern = pMember->LineParser.flagArgument(IsGrep ? TST_ARG_GREP : TST_ARG_GLOB);
      if(pMember->LineParser.isFlagPresent(TST_ARG_NOCASE))
      {
         Pattern.toLowerCase();
      }
      COLarray<COLstring> TestList;
      TSTlistTests(TestList, pMember->Collection);
      
      // pick tests that contain the given substring
      COLarray<COLstring> SelectedTestList;
      for(COLindex TestListIndex=0; TestListIndex < TestList.size(); TestListIndex++)
      {
         COLstring TestStringToMatch = TestList[TestListIndex];
         if(pMember->LineParser.isFlagPresent(TST_ARG_NOCASE))
         {
            TestStringToMatch.toLowerCase();
         }
         if((IsGrep && TestStringToMatch.find(Pattern) != npos)
            || (!IsGrep && COLglobMatch(Pattern.c_str(), TestStringToMatch.c_str())))
         {
            COLcout << TestList[TestListIndex] << newline;
            SelectedTestList.push_back() = TestList[TestListIndex];
         }
      }
      
      if(!SelectedTestList.size())
      {
         COLcout << "Error: no tests found that match \"" << Pattern << "\"" << newline;
         return false;
      }
      COLcout << "Found " << SelectedTestList.size() << " matching tests." << newline << newline;

      if(pMember->LineParser.isFlagPresent(TST_ARG_SHOWTESTS))
      {
         // not running the tests, just displaying them
         return true;
      }

      Succeeded = TSTrunTestList(TestResult, SelectedTestList, pMember->Collection, TestRunner, Flags);
      TSTverifyResultSuccess(TestResult);
   }
   else if (pMember->LineParser.isFlagPresent(TST_ARG_LASTFAILED) ||
            pMember->LineParser.isFlagPresent(TST_ARG_TESTFILE))
   {
      COLstring TestSpecFileName(TST_LAST_FAILED_FILE_NAME);
      if (pMember->LineParser.isFlagPresent(TST_ARG_TESTFILE))
      {
         TestSpecFileName = pMember->LineParser.flagArgument(TST_ARG_TESTFILE);
      }
      try
      {
         TTestSpecList TestSpecList;
         TSTloadTestSpecsFromFile(TestSpecFileName, TestSpecList);

         if (!TestSpecList.size())
         {
            COLcout << "No tests in " << TestSpecFileName << ", consider running all tests" << newline;
            return false;
         }
         Succeeded = TSTrunTestList(TestResult, TestSpecList, pMember->Collection, TestRunner, Flags);
         TSTverifyResultSuccess(TestResult);
      }
      catch(const COLerror& Error)
      {
         COLcout << "Error while running tests: " << Error.description() << newline;
         return false;
      }    
   }
   else
   {
      Succeeded = TestRunner.runAll(pMember->Collection,TestResult, Flags);
   }

   //format results
   COLcout << newline << "Test Results: " << newline;
   TSTtestResultFormatter ResultFormatter;
   ResultFormatter.printOn(TestResult,*COLcout.sink());
   ResultFormatter.showFailures(TestResult, COLcout);
   TSTwriteLastFailedFile(TST_LAST_FAILED_FILE_NAME,TestResult);

   return Succeeded;
}

void TSTconsoleApplication::generateExampleTestSpec(COLostream& Stream)
{
   // Used to display wildcard usage. If there is no test name, the string remains empty.
   COLstring TestName = "";
   Stream << " --" << TST_ARG_TEST << " \"";

   TSTtestable* pTest = &pMember->Collection;

   if (pMember->Collection.countOfTest() > 0)
   {
      pTest = pMember->Collection.test(0);
   }
   else
   {
      Stream << "category/[*]" << newline;
      return;
   }

   while (pTest)
   {
      if (pTest->testType().isDerivedFrom(TSTtypeTestCollection::instance()))
      {
         // If a collection of tests exists within the unit test, remember it's name for
         // future use.
         TestName = pTest->testName();
         TestName += '/';
         Stream << TestName;
         TSTtestCollection* pCollection = (TSTtestCollection*)pTest;

         if (pCollection->countOfTest() == 0)
         {
            // We're kind of screwed here since this collection doesn't have a test
            // for us to give an example of
            COLcout << "[*]";
            pTest = NULL;
         }
         else
         {
            pTest = pCollection->test(0);
         }
      }
      else
      {
         Stream << '[' << pTest->testName() << "]\"" << newline;
         pTest = NULL;
      }
   }
   Stream << "The '*' wildcard can be used to run all tests. i.e. \"" << TestName << "[*]\"" << newline;
   Stream << "The 'and' keyword can be used to run several selected tests within the same collection (the quotation marks are required)" << newline;
}
