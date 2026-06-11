//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMDtest.cpp
//
// Description:
//
// Implementation
//
// Author: Roman Leifer
// Date:   Tue 08/17/2004
//
//---------------------------------------------------------------------------
#include <CMDexample/CMDexamplePrecomp.h>
#pragma hdrstop

#include <CMDexample/CMDtest.h>
#include <COL/COLarray.h>
#include <CMDexample/CMDtestStringArray.h>

//Test the main parseArgs() function that parses the command line arguments
void CMDtest::parseArgsTest()
{
   int Argc = 0;
   const char** ppArgv;
   ArgMatrix.clear();

   addCommandLine("CMDexample --flag1 arg1");
   addCommandLine("CMDexample");
   addCommandLine("CMDexample arg1");
   addCommandLine("CMDexample arg1 arg2"); 
   addCommandLine("CMDexample arg1 arg2 arg3");
   addCommandLine("CMDexample --flag1 --flag2");
   addCommandLine("CMDexample --flag1 arg1 --flag2 arg2");
   addCommandLine("CMDexample --flag1 --flag2 arg2");
   addCommandLine("CMDexample --flag1 arg1 --flag2");
   addCommandLine("CMDexample --flag1 arg1 --flag2 arg2 arg3");
   addCommandLine("CMDexample arg1 --flag1 --flag2 arg2 arg3");

   addCommandLine("CMDexample ---flag1 arg1");
   addCommandLine("CMDexample -flag1 arg1");
   addCommandLine("CMDexample -");
   addCommandLine("CMDexample --");
   addCommandLine("CMDexample ---");
   addCommandLine("CMDexample - flag1");
   addCommandLine("CMDexample -f flag1");


   for(COLuint32 ArgvIndex = 0; ArgvIndex < ArgMatrix.size(); ArgvIndex++)
   {
      CMDlineParser LineParser;
      Argc = ArgMatrix[ArgvIndex].Vector.size();
      ppArgv = ArgMatrix[ArgvIndex].getArray();

      COLcout << "Argv:         " ;
      for(COLuint32 ParamIndex = 0; ParamIndex < ArgMatrix[ArgvIndex].Vector.size(); ParamIndex++)
      {
         COLcout << ppArgv[ParamIndex] << " ";
      }
      COLcout << newline;

      COLcout << "Argc:         " << Argc << newline;

      //Flag test
      if(ArgMatrix[ArgvIndex].has("flag1"))
      {
         LineParser.addFlagWithArgument("flag1", "arg1");
      }

      //Parse the command line
      LineParser.parseArgs(Argc, ppArgv);

      //If the flag had an argument, put it here
      if(ArgMatrix[ArgvIndex].has("flag1") && LineParser.isFlagPresent("flag1"))
      {
         COLcout << "Arg to flag1: " << LineParser.flagArgument("flag1") << newline;
      }

      //See what arguments there were that are not to recognized flags

      COLcout << "ExtraArgs:    ";
      for(COLuint32 ArgumentIndex = 0; ArgumentIndex < LineParser.countOfExtraArgument(); ArgumentIndex++)
      {
         COLcout << LineParser.extraArgument(ArgumentIndex) << " ";
      }
      COLcout << newline << newline;
   }
}

//Create an array of strings that will make it easy
//to add new command lines and pass them as char** argv
void CMDtest::addCommandLine(const COLstring& CommandLine)
{
   CMDtestStringArray StringArray;
   StringArray.addCommandLine(CommandLine);

   ArgMatrix.push_back(StringArray);
}

void CMDtest::showUsageTest()
{
   CMDlineParser LineParser;
   ArgMatrix.clear();

   LineParser.setDescription("CMDexample - tests the CMD library");
   LineParser.addFlagWithArgument("flag1", "arg1");
   LineParser.addFlagWithoutArgument("flag2", "description of flag2");
   LineParser.addExtraParamDescription("param1", "description of param1");
   LineParser.addExtraParamDescriptionRequired("param2", "description of required param 2");

   LineParser.showUsage(COLcout);
   COLcout << newline;

   LineParser.hideFlag("flag1");
   LineParser.hideFlag("flag2");
   LineParser.showUsage(COLcout);
}

void CMDtest::helpFlagTest()
{
   ArgMatrix.clear();

   addCommandLine("CMDexample /?");
   addCommandLine("CMDexample -help");

   for(COLuint32 ArgvIndex = 0; ArgvIndex < ArgMatrix.size(); ArgvIndex++)
   {
      CMDlineParser LineParser;
      LineParser.setDescription("Help flag detected");
      
      COLuint32 Argc = ArgMatrix[ArgvIndex].Vector.size();
      const char** ppArgv = ArgMatrix[ArgvIndex].getArray();

      COLcout << newline;
      COLcout << "Argv:         " ;
      for(COLuint32 ParamIndex = 0; ParamIndex < ArgMatrix[ArgvIndex].Vector.size(); ParamIndex++)
      {
         COLcout << ppArgv[ParamIndex] << " ";
      }
      COLcout << newline << newline;

      LineParser.parseArgs(Argc, ppArgv);

      COLuint32 Count =   LineParser.countOfExtraArgument();

      if(LineParser.parsingErrorsPresent(COLcerr))
      {
         LineParser.showUsage(COLcout);
      }
      else
      {
         TST_ASSERT_MESSAGE(false, "Error: Help flag not detected");
      }
   }
}

// collection of test methods
TSTtestable* CMDtest::tests() {
   TST_START_FIXTURE(CMDtest)
      TST_ADD_TEST_METHOD(parseArgsTest);
      TST_ADD_TEST_METHOD(showUsageTest);
      TST_ADD_TEST_METHOD(helpFlagTest);
   TST_END_FIXTURE
}


//////////////////////////////////////////////////////////////////////////
// New unit tests that actually check the results, rather than printing //
// them to standard out.                                                //
//////////////////////////////////////////////////////////////////////////

void CMDTparseCommandLine(CMDlineParser& LineParser, const char* CommandLine)
{
   // First, populate a vector with the argument(s)
   COLarray<COLstring> Vector;
   COLstring SubCommandLine = CommandLine;
   //if there are no spaces, only one param
   if(SubCommandLine.find(' ') == npos)
   {
      Vector.push_back(SubCommandLine);
   }
   else
   {
      while(SubCommandLine.find(' ') != npos)
      {
         COLstring Param;
         SubCommandLine.split(Param, SubCommandLine, " ");
         Vector.push_back(Param);
         SubCommandLine = SubCommandLine.strip(COLstring::Leading);
      }

      Vector.push_back(SubCommandLine);
   }

   // Now populate an array
   COLarray<const char*> Array;
   Array.resize(Vector.size());
   for (COLuint32 ArgIndex = 0; ArgIndex < Vector.size(); ArgIndex++)
   {
      Array[ArgIndex] = Vector[ArgIndex].c_str();
   }

   LineParser.parseArgs(Array.size(), Array.getArray());
}

void testParseArgsNoArgs()
{
   CMDlineParser LineParser;
   CMDTparseCommandLine(LineParser, "CMDexample");

   TST_ASSERT_EQUALS(0, LineParser.countOfExtraArgument());
   TST_ASSERT_EQUALS(false, LineParser.parsingErrorsPresent(COLcerr));
}

void testParseArgsOneExtraArg()
{
   CMDlineParser LineParser;
   CMDTparseCommandLine(LineParser, "CMDexample extra_arg");

   TST_ASSERT_EQUALS(1, LineParser.countOfExtraArgument());
   TST_ASSERT_EQUALS("extra_arg", LineParser.extraArgument(0));
   TST_ASSERT_EQUALS(false, LineParser.parsingErrorsPresent(COLcerr));
}

void testParseArgsManyExtraArgs()
{
   CMDlineParser LineParser;
   CMDTparseCommandLine(LineParser, "CMDexample extra_arg1   extra_arg2  extra_arg3         extra_arg4");

   TST_ASSERT_EQUALS(4, LineParser.countOfExtraArgument());
   TST_ASSERT_EQUALS("extra_arg1", LineParser.extraArgument(0));
   TST_ASSERT_EQUALS("extra_arg2", LineParser.extraArgument(1));
   TST_ASSERT_EQUALS("extra_arg3", LineParser.extraArgument(2));
   TST_ASSERT_EQUALS("extra_arg4", LineParser.extraArgument(3));
   TST_ASSERT_EQUALS(false, LineParser.parsingErrorsPresent(COLcerr));
}

void testParseArgsOneExtraNamedArg()
{
   CMDlineParser LineParser;
   LineParser.addExtraParamDescription("extra_arg1", "extra_arg1");
   CMDTparseCommandLine(LineParser, "CMDexample extra_arg");
   int ExtraParamIndex = 0;

   TST_ASSERT_EQUALS(1, LineParser.countOfExtraArgument());
   TST_ASSERT_EQUALS("extra_arg", LineParser.extraArgument(0));
   TST_ASSERT_EQUALS("extra_arg", LineParser.extraArgument("extra_arg1"));
   TST_ASSERT(LineParser.findExtraParam("extra_arg1", ExtraParamIndex));
   TST_ASSERT(ExtraParamIndex == 0);
   TST_ASSERT_EQUALS(false, LineParser.parsingErrorsPresent(COLcerr));
}

void testParseArgsManyExtraNamedArgs()
{
   CMDlineParser LineParser;
   LineParser.addExtraParamDescription("extra_arg1", "extra_arg1");
   LineParser.addExtraParamDescription("extra_arg2", "extra_arg2");
   LineParser.addExtraParamDescription("extra_arg3", "extra_arg3");
   LineParser.addExtraParamDescription("extra_arg4", "extra_arg4");
   CMDTparseCommandLine(LineParser, "CMDexample extra_arg1   extra_arg2  extra_arg3         extra_arg4");
   int ExtraParamIndex = 0;

   TST_ASSERT_EQUALS(4, LineParser.countOfExtraArgument());
   TST_ASSERT_EQUALS("extra_arg1", LineParser.extraArgument(0));
   TST_ASSERT_EQUALS("extra_arg2", LineParser.extraArgument(1));
   TST_ASSERT_EQUALS("extra_arg3", LineParser.extraArgument(2));
   TST_ASSERT_EQUALS("extra_arg4", LineParser.extraArgument(3));

   TST_ASSERT_EQUALS("extra_arg1", LineParser.extraArgument("extra_arg1"));
   TST_ASSERT_EQUALS("extra_arg2", LineParser.extraArgument("extra_arg2"));
   TST_ASSERT_EQUALS("extra_arg3", LineParser.extraArgument("extra_arg3"));
   TST_ASSERT_EQUALS("extra_arg4", LineParser.extraArgument("extra_arg4"));

   TST_ASSERT(LineParser.findExtraParam("extra_arg1", ExtraParamIndex));
   TST_ASSERT(ExtraParamIndex == 0);
   TST_ASSERT(LineParser.findExtraParam("extra_arg2", ExtraParamIndex));
   TST_ASSERT(ExtraParamIndex == 1);
   TST_ASSERT(LineParser.findExtraParam("extra_arg3", ExtraParamIndex));
   TST_ASSERT(ExtraParamIndex == 2);
   TST_ASSERT(LineParser.findExtraParam("extra_arg4", ExtraParamIndex));
   TST_ASSERT(ExtraParamIndex == 3);

   TST_ASSERT_EQUALS(false, LineParser.parsingErrorsPresent(COLcerr));
}

void testParseArgsOneFlagWithoutArgument()
{
   {
      CMDlineParser LineParser;
      LineParser.addFlagWithoutArgument("flag1", "A flag with no argument.");
      CMDTparseCommandLine(LineParser, "CMDexample");

      TST_ASSERT_EQUALS(false, LineParser.isFlagPresent("flag1"));
      TST_ASSERT_EQUALS(false, LineParser.parsingErrorsPresent(COLcerr));
   }

   {
      CMDlineParser LineParser;
      LineParser.addFlagWithoutArgument("flag1", "A flag with no argument.");
      CMDTparseCommandLine(LineParser, "CMDexample --flag1");

      TST_ASSERT_EQUALS(0, LineParser.countOfExtraArgument());
      TST_ASSERT_EQUALS(true, LineParser.isFlagPresent("flag1"));
      TST_ASSERT_EQUALS(false, LineParser.parsingErrorsPresent(COLcerr));
   }
}

void testParseArgsUnknownFlagWithoutArgument()
{
   CMDlineParser LineParser;
   LineParser.addFlagWithoutArgument("flag", "A flag that is almost called \"flag1\".");
   CMDTparseCommandLine(LineParser, "CMDexample --flag1");

   TST_ASSERT_EQUALS(0, LineParser.countOfExtraArgument());
   TST_ASSERT_EQUALS(false, LineParser.isFlagPresent("flag"));
   TST_ASSERT_EQUALS(true, LineParser.isFlagPresent("flag1"));
   TST_ASSERT_EXCEPTION(LineParser.isFlagPresent("flag2"), COLerror("EXCEPTION: isFlagPresent was asked to check for the presence of flag flag2 which was not installed. A flag should be first installed using either addFlagWithArgument or addFlagWithoutArgument before it is checked for.", COLerror::PreCondition));
   COLstring Output;
   COLostream OutputStream(Output);
   TST_ASSERT_EQUALS(true, LineParser.parsingErrorsPresent(OutputStream));
   COLstring ExpectedOutput;
   COLostream(ExpectedOutput) << "Unknown flag flag1 encountered." << newline;
   TST_ASSERT_EQUALS(ExpectedOutput, Output);
}

void testParseArgsManyFlagsWithoutArguments()
{
   CMDlineParser LineParser;
   LineParser.addFlagWithoutArgument("flag1", "A flag with no argument.");
   LineParser.addFlagWithoutArgument("flag2", "Another flag with no argument.");
   LineParser.addFlagWithoutArgument("flag3", "The final flag with no argument.");
   CMDTparseCommandLine(LineParser, "CMDexample --flag1  --flag3");

   TST_ASSERT_EQUALS(0, LineParser.countOfExtraArgument());
   TST_ASSERT_EQUALS(true, LineParser.isFlagPresent("flag1"));
   TST_ASSERT_EQUALS(false, LineParser.isFlagPresent("flag2"));
   TST_ASSERT_EQUALS(true, LineParser.isFlagPresent("flag3"));
   TST_ASSERT_EQUALS(false, LineParser.parsingErrorsPresent(COLcerr));
}

void testParseArgsRepeatedFlag()
{
   CMDlineParser LineParser;
   LineParser.addFlagWithoutArgument("flag1", "A flag with no argument.");
   LineParser.addFlagWithoutArgument("flag2", "Another flag with no argument.");
   LineParser.addFlagWithoutArgument("flag3", "The final flag with no argument.");
   CMDTparseCommandLine(LineParser, "CMDexample --flag1 --flag2 --flag1");

   TST_ASSERT_EQUALS(true, LineParser.isFlagPresent("flag1"));
   TST_ASSERT_EQUALS(true, LineParser.isFlagPresent("flag2"));
   TST_ASSERT_EQUALS(false, LineParser.isFlagPresent("flag3"));
   COLstring Output;
   COLostream OutputStream(Output);
   TST_ASSERT_EQUALS(true, LineParser.parsingErrorsPresent(OutputStream));
   COLstring ExpectedOutput;
   COLostream(ExpectedOutput) << "Flag flag1 used more than once." << newline;
   TST_ASSERT_EQUALS(ExpectedOutput, Output);
}

void testParseArgsRepeatedFlagOk()
{
   CMDlineParser LineParser;
   LineParser.addFlagWithArgument("flag1", "A flag with no argument.");
   LineParser.setFlagRepeatOk("flag1", true);
   LineParser.addFlagWithoutArgument("flag2", "Another flag with no argument.");

   CMDTparseCommandLine(LineParser, "CMDexample --flag1 value1 --flag2 --flag1 value2");

   TST_ASSERT_EQUALS(true, LineParser.isFlagPresent("flag1"));
   TST_ASSERT_EQUALS(true, LineParser.isFlagPresent("flag2"));
   TST_ASSERT_EQUALS(2, LineParser.flagRepeatCount("flag1"));
   TST_ASSERT_EQUALS(COLstring("value1"), LineParser.flagRepeatValue("flag1", 0));
   TST_ASSERT_EQUALS(COLstring("value2"), LineParser.flagRepeatValue("flag1", 1));
   COLarray<COLstring> ValueList;
   CMDlineParserRepeatValueList(ValueList, LineParser, "flag1");
   TST_ASSERT_EQUALS(2, ValueList.size());
   TST_ASSERT_EQUALS(COLstring("value1"), ValueList[0]);
   TST_ASSERT_EQUALS(COLstring("value2"), ValueList[1]);
   COLstring Output;
   COLostream OutputStream(Output);
   TST_ASSERT_EQUALS(false, LineParser.parsingErrorsPresent(OutputStream));
}

void testParseArgsRepeatedFlagWithArgument()
{
   CMDlineParser LineParser;
   LineParser.addFlagWithArgument("flag1", "A flag with an argument.");
   CMDTparseCommandLine(LineParser, "CMDexample --flag1 arg1 --flag1 arg2");

   TST_ASSERT_EQUALS(true, LineParser.isFlagPresent("flag1"));
   COLstring Output;
   COLostream OutputStream(Output);
   TST_ASSERT_EQUALS(true, LineParser.parsingErrorsPresent(OutputStream));
   COLstring ExpectedOutput;
   COLostream(ExpectedOutput) << "Flag flag1 used more than once." << newline;
   TST_ASSERT_EQUALS(ExpectedOutput, Output);
}

void testParseArgsFlagsWithMoreOrLessDashes()
{
   CMDlineParser LineParser;
   LineParser.addFlagWithoutArgument("flag1", "A flag with no argument.");
   LineParser.addFlagWithoutArgument("flag2", "Another flag with no argument.");
   LineParser.addFlagWithoutArgument("flag3", "The final flag with no argument.");
   // We accept any number of dashes
   CMDTparseCommandLine(LineParser, "CMDexample -flag1 ---flag2 -----------flag3");

   TST_ASSERT_EQUALS(true, LineParser.isFlagPresent("flag1"));
   TST_ASSERT_EQUALS(true, LineParser.isFlagPresent("flag2"));
   TST_ASSERT_EQUALS(true, LineParser.isFlagPresent("flag3"));
   TST_ASSERT_EQUALS(false, LineParser.parsingErrorsPresent(COLcerr));
}

void testParseArgsFlagWithNoDashes() // a flag with no dashes is just an extra argument
{
   CMDlineParser LineParser;
   LineParser.addFlagWithoutArgument("flag1", "A flag with no argument.");
   CMDTparseCommandLine(LineParser, "CMDexample flag1");

   TST_ASSERT_EQUALS(false, LineParser.isFlagPresent("flag1"));
   TST_ASSERT_EQUALS(1, LineParser.countOfExtraArgument());
   TST_ASSERT_EQUALS("flag1", LineParser.extraArgument(0));
}

void testParseArgsJustDashes()
{
   CMDlineParser LineParser;
   CMDTparseCommandLine(LineParser, "CMDexample ---");

   TST_ASSERT_EQUALS(1, LineParser.countOfExtraArgument());
   TST_ASSERT_EQUALS("---", LineParser.extraArgument(0));
}

void testParseArgsFlagWithArgument()
{
   CMDlineParser LineParser;
   LineParser.addFlagWithArgument("flag1", "value1", "A flag with an argument.");
   LineParser.addFlagWithArgument("flag2", "value2", "Another flag with an argument.");
   CMDTparseCommandLine(LineParser, "CMDexample --flag1 arg1");

   TST_ASSERT_EQUALS(0, LineParser.countOfExtraArgument());
   TST_ASSERT_EQUALS(true, LineParser.isFlagPresent("flag1"));
   TST_ASSERT_EQUALS("arg1", LineParser.flagArgument("flag1"));
   COLstring Flag1Arg;
   LineParser.flagArgument("flag1", Flag1Arg);
   TST_ASSERT_EQUALS("arg1", Flag1Arg);

   TST_ASSERT_EQUALS(false, LineParser.isFlagPresent("flag2"));
}

void testParseArgsManyFlagsWithArguments()
{
   CMDlineParser LineParser;
   LineParser.addFlagWithArgument("flag1", "value1", "A flag with an argument.");
   LineParser.addFlagWithArgument("flag2", "value2", "Another flag with an argument.");
   CMDTparseCommandLine(LineParser, "CMDexample    -flag1 arg1 ---flag2  arg2");

   TST_ASSERT_EQUALS(0, LineParser.countOfExtraArgument());
   TST_ASSERT_EQUALS(true, LineParser.isFlagPresent("flag1"));
   TST_ASSERT_EQUALS("arg1", LineParser.flagArgument("flag1"));
   TST_ASSERT_EQUALS(true, LineParser.isFlagPresent("flag2"));
   TST_ASSERT_EQUALS("arg2", LineParser.flagArgument("flag2"));
}

void testParseArgsFlagWithFlagArgument()
{
   CMDlineParser LineParser;
   LineParser.addFlagWithArgument("flag1", "value", "A flag with an argument.");
   LineParser.addFlagWithoutArgument("flag2", "A flag without an argument.");
   CMDTparseCommandLine(LineParser, "CMDexample --flag1 --flag2");

   TST_ASSERT_EQUALS(0, LineParser.countOfExtraArgument());
   TST_ASSERT_EQUALS(true, LineParser.isFlagPresent("flag1"));
   TST_ASSERT_EQUALS("--flag2", LineParser.flagArgument("flag1"));
   TST_ASSERT_EQUALS(false, LineParser.isFlagPresent("flag2"));
}

void testParseArgsFlagWithFlagArgument2()
{
   CMDlineParser LineParser;
   LineParser.addFlagWithArgument("flag1", "value", "A flag with an argument.");
   LineParser.addFlagWithArgument("flag2", "Another flag with an argument.");
   CMDTparseCommandLine(LineParser, "CMDexample --flag1 --flag2 arg1");

   TST_ASSERT_EQUALS(1, LineParser.countOfExtraArgument());
   TST_ASSERT_EQUALS("arg1", LineParser.extraArgument(0));
   TST_ASSERT_EQUALS(true, LineParser.isFlagPresent("flag1"));
   TST_ASSERT_EQUALS("--flag2", LineParser.flagArgument("flag1"));
   TST_ASSERT_EQUALS(false, LineParser.isFlagPresent("flag2"));
   TST_ASSERT_EQUALS("", LineParser.flagArgument("flag2"));
}

void testParseArgsAllArgumentTypes()
{
   CMDlineParser LineParser;
   LineParser.addFlagWithArgument("flag1", "value", "A flag with an argument.");
   LineParser.addFlagWithoutArgument("flag2", "A flag without an argument.");
   CMDTparseCommandLine(LineParser, "CMDexample --flag1 arg1 --flag2 extra_arg1 extra_arg2");

   TST_ASSERT_EQUALS(2, LineParser.countOfExtraArgument());
   TST_ASSERT_EQUALS("extra_arg1", LineParser.extraArgument(0));
   TST_ASSERT_EQUALS("extra_arg2", LineParser.extraArgument(1));
   TST_ASSERT_EQUALS(true, LineParser.isFlagPresent("flag1"));
   TST_ASSERT_EQUALS("arg1", LineParser.flagArgument("flag1"));
   TST_ASSERT_EQUALS(true, LineParser.isFlagPresent("flag2"));
}

void testParseArgsCalledTwiceThrowsException()
{
   CMDlineParser LineParser;
   LineParser.addFlagWithoutArgument("flag1", "A flag without an argument.");
   CMDTparseCommandLine(LineParser, "CMDexample --flag1");
   TST_ASSERT_EXCEPTION(CMDTparseCommandLine(LineParser, "CMDexample --flag1"), COLerror("CMDlineParser::parseArgs() has been called more than once.", COLerror::PreCondition));
}

void testShowUsage()
{
   CMDlineParser LineParser;
   LineParser.addFlagWithArgument("flag1", "value", "A flag with an argument");
   LineParser.addFlagWithoutArgument("flag2", "A flag without an argument");
   LineParser.addExtraParamDescriptionRequired("extra_arg1", "A required extra argument");
   LineParser.addExtraParamDescription("extra_arg2", "An optional extra argument");
   CMDTparseCommandLine(LineParser, "CMDexample --flag3");

   COLstring Usage;
   COLostream UsageStream(Usage);

   TST_ASSERT(LineParser.parsingErrorsPresent(UsageStream));
   LineParser.showUsage(UsageStream);

   // HACK: This stuff depends on STDOUT/ERR from the command itself, which can
   // change if debugging is enabled. Tweak the expected usage message appropriately.
   COLstring ExpectedUsage;
   COLostream(ExpectedUsage) << "Unknown flag flag3 encountered." << newline << newline
                             << "Usage:" << newline
#if defined(COL_ENABLE_TRACING)
                             << "CMDexample  [--flag1 value]  [--flag2]  [--out <filename>]  [--d <modules>]"
                             << "  [--trace <modules>]  [--i <modules>]  [--w <modules>]  [--e <modules>]"
                             << "  [--F true|false]  [--n true|false] extra_arg1 <extra_arg2>" << newline
#else
                             << "CMDexample  [--flag1 value]  [--flag2] extra_arg1 <extra_arg2>" << newline
#endif
                             << "   extra_arg1 - A required extra argument" << newline
                             << "   extra_arg2 - An optional extra argument" << newline
                             << "Known flags:" << newline
                             << "  --flag1  A flag with an argument. Requires argument value." << newline
                             << "  --flag2  A flag without an argument" << newline
#if defined(COL_ENABLE_TRACING) 
                             << "  --out    COLlog filename. Requires argument <filename>." << newline
                             << "  --d      COLlog DBG. Requires argument <modules>." << newline
                             << "  --trace  COLlog TRC. Requires argument <modules>." << newline
                             << "  --i      COLlog INF. Requires argument <modules>." << newline
                             << "  --w      COLlog WRN. Requires argument <modules>." << newline
                             << "  --e      COLlog ERR. Requires argument <modules>." << newline
                             << "  --F      COLlog flush. Requires argument true|false." << newline
                             << "  --n      COLlog indenting. Requires argument true|false." << newline
#endif
                              ;


   TST_ASSERT_EQUALS(ExpectedUsage, Usage);
}

void testShowUsageFlagOrderPreserved()
{
   CMDlineParser LineParser;
   LineParser.addFlagWithoutArgument("foobar", "A flag without an argument");
   LineParser.addFlagWithoutArgument("foo", "A flag without an argument");
   LineParser.addFlagWithoutArgument("C", "A flag without an argument");
   LineParser.addFlagWithoutArgument("A", "A flag without an argument");
   LineParser.addFlagWithoutArgument("B", "A flag without an argument");
   CMDTparseCommandLine(LineParser, "CMDexample --?");

   COLstring Usage;
   COLostream UsageStream(Usage);

   TST_ASSERT(LineParser.parsingErrorsPresent(UsageStream));
   LineParser.showUsage(UsageStream);

   // HACK: This stuff depends on STDOUT/ERR from the command itself, which can
   // change if debugging is enabled. Tweak the expected usage message appropriately.
   COLstring ExpectedUsage;
   COLostream(ExpectedUsage) << newline
                             << "Usage:" << newline
#if defined(COL_ENABLE_TRACING)
                             << "CMDexample  [--foobar]  [--foo]  [--C]  [--A]  [--B]  [--out <filename>]"
                             << "  [--d <modules>]  [--trace <modules>]  [--i <modules>]  [--w <modules>]"
                             << "  [--e <modules>]  [--F true|false]  [--n true|false]" << newline
#else
                             << "CMDexample  [--foobar]  [--foo]  [--C]  [--A]  [--B]" << newline
#endif
                             << "Known flags:" << newline
                             << "  --foobar  A flag without an argument" << newline
                             << "  --foo     A flag without an argument" << newline
                             << "  --C       A flag without an argument" << newline
                             << "  --A       A flag without an argument" << newline
                             << "  --B       A flag without an argument" << newline
#if defined(COL_ENABLE_TRACING) 
                             << "  --out     COLlog filename. Requires argument <filename>." << newline
                             << "  --d       COLlog DBG. Requires argument <modules>." << newline
                             << "  --trace   COLlog TRC. Requires argument <modules>." << newline
                             << "  --i       COLlog INF. Requires argument <modules>." << newline
                             << "  --w       COLlog WRN. Requires argument <modules>." << newline
                             << "  --e       COLlog ERR. Requires argument <modules>." << newline
                             << "  --F       COLlog flush. Requires argument true|false." << newline
                             << "  --n       COLlog indenting. Requires argument true|false." << newline
#endif
                              ;

   TST_ASSERT_EQUALS(ExpectedUsage, Usage);
}


TSTtestable* CMDTlineParserTests()
{
   TST_START_COLLECTION("CMDTtestLineParser");
      TST_ADD_TEST_FUNCTION(testParseArgsNoArgs);
      TST_ADD_TEST_FUNCTION(testParseArgsOneExtraArg);
      TST_ADD_TEST_FUNCTION(testParseArgsManyExtraArgs);
      TST_ADD_TEST_FUNCTION(testParseArgsOneExtraNamedArg);
      TST_ADD_TEST_FUNCTION(testParseArgsManyExtraNamedArgs);
      TST_ADD_TEST_FUNCTION(testParseArgsOneFlagWithoutArgument);
      TST_ADD_TEST_FUNCTION(testParseArgsUnknownFlagWithoutArgument);
      TST_ADD_TEST_FUNCTION(testParseArgsManyFlagsWithoutArguments);
      TST_ADD_TEST_FUNCTION(testParseArgsRepeatedFlag);
      TST_ADD_TEST_FUNCTION(testParseArgsRepeatedFlagWithArgument);
      TST_ADD_TEST_FUNCTION(testParseArgsRepeatedFlagOk);
      TST_ADD_TEST_FUNCTION(testParseArgsFlagsWithMoreOrLessDashes);
      TST_ADD_TEST_FUNCTION(testParseArgsFlagWithNoDashes);
      TST_ADD_TEST_FUNCTION(testParseArgsJustDashes);
      TST_ADD_TEST_FUNCTION(testParseArgsFlagWithArgument);
      TST_ADD_TEST_FUNCTION(testParseArgsManyFlagsWithArguments);
      TST_ADD_TEST_FUNCTION(testParseArgsFlagWithFlagArgument);
      TST_ADD_TEST_FUNCTION(testParseArgsFlagWithFlagArgument2);
      TST_ADD_TEST_FUNCTION(testParseArgsAllArgumentTypes);
      TST_ADD_TEST_FUNCTION(testParseArgsCalledTwiceThrowsException);

      TST_ADD_TEST_FUNCTION(testShowUsage);
      TST_ADD_TEST_FUNCTION(testShowUsageFlagOrderPreserved);
   TST_END_COLLECTION
}
