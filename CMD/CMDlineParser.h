#ifndef __CMD_LINE_PARSER_H__
#define __CMD_LINE_PARSER_H__
//------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module:  CMDlineParser
// 
// Description
//
// This is utility class which makes it easy to get command line
// arguments.  
//
// The command line has the following general form:
//
// ApplicationName [--flag1 [argument1]] [--flag2 [argument2]] ... [--flagN [argumentN]]
//    [extraArgument1] [extraArgument2] ... [extraArgumentM]
//
// where the square brackets denote that the value is optional.  Flags are denoted by
// two dashes '--', and at present a flag may have zero or one arguments following it.
// A string in the command line which is not a flag, and which is not the argument of
// a flag, is classified as an 'extra argument'.  All such extra arguments should appear
// in the command line after all the flags, as shown.
//
// Best usage: 
//
// For each flag which your parser handles, use addFlagWithoutArgument(FlagName, Description)
// or addFlagWithArgument(FlagName, ArgumentName, Description) to register the flag with
// the parser.  Then, for each registered flag, have a conditional:
// if (LineParser.isFlagPresent("FlagName"))
// {
//    implementation details
// } 
// (Avoid using flagPresent to implicitly register flags without arguments with the parser, since there
// is no equivalent method for flags with arguments, and it leads to code that is more confusing than
// it needs to be.)  

// The interface for the CMDlineParser class also refers to 'parameters'.  Parameters are
// arguments which are not associated with a flag.  That is, they look just like 'extra
// arguments' on the command line, but they can have a description associated with them,
// and they can be required or optional.  
//
// TODO - it is probably needlessly confusing to have flags, arguments, parameters, 
// and extra arguments.  Perhaps parameters could be recast as flags with arguments.
//
// TODO - An effort has been made to allow the parser to handle wildcards as 'extra arguments',
// but instead of being called 'extra arguments', they are called 'files'.  This functionality 
// is not yet working.  Is there any good reason to treat expanded wildcards differently than
// 'extra arguments'?
//
// TODO
//  - Would be nice if optional flags/arguments could be assigned default values.
//  - Would be nice if flags/arguments could be typed (at least for basic built-in types).
//  - Treating help flags as unknown arguments is not user friendly.
//  - Neither parseArgs, nor parsingErrorsPresent check for required "extra arguments".
//  - Cleaner object model would have three classes: schema -> parser -> results.
//    See Boost's Program Options library for an example.
//
// Author: Eliot Muir
// Date:   Thu 10/03/2002 
//
//---------------------------------------------------------------------------
#ifdef CMD_LINE_PARSER_FULL
#include <COL/COLarray.h>
#endif

#include <COL/COLlog.h>

class CMDlineParserPrivate;
class COLstring;
class COLostream;

class CMDlineParser{
public:
   CMDlineParser();
   virtual ~CMDlineParser();

   // Parses the command line.  It registers the flags that are present, and builds
   // a vector of 'extra arguments'.
   // This method will return false it if fails - it will also
   // output an error message to stderr - the user should check the return
   // value of this class.
   void parseArgs(int argc, const char** ppArg);
   void parseArgsNoTrace(int argc, const char** ppArg);
   
   // returns the program name - argv[0] - which was stored upon parsing above
   const COLstring& programName() const;

   // Returns the number of extra arguments found 
   int countOfExtraArgument() const;

   // Retrieves the value of an extra argument
   const COLstring& extraArgument(int ArgumentIndex) const;

   // Retrieves the value of an extra argument
   const COLstring& extraArgument(const COLstring& Parameter) const;

   // Returns the number of files that were passed in as parameters
   const int countOfFile() const;

   // Returns the file at the given index
   const COLstring& fileName(const int FileIndex) const;

   // Is a flag present? - true/false -- new one, use this one from now on.
   // Differs from flagPresent in that it only checks the FlagTable for the
   // presence of FlagName, and does not try to register the flag.
   bool isFlagPresent(const COLstring& FlagName) const;


   // Registers a flag without any arguments.  The Description is used
   // by showUsage().
   void addFlagWithoutArgument(const COLstring& FlagName,
                               const COLstring& Description);

   // Registers a flag which has an argument. 
   // There seems to be some confusion in the libraries about the use
   // of this method.  ArgumentName is not a substitute for a flag description,
   // though it has sometimes been erroneously used that way.  Instead, 
   // ArgumentName should give the form of the argument.  A description can
   // - and should - be added using flagPresent(FlagName, Description) or, better
   // yet, use the method below which takes 3 parameters.  Both
   // the ArgumentName and the Description are used by showUsage().
   //
   // For instance, suppose we have a flag '--mailto' which, when present, means
   // that the output of the command line utility is mailed to a particular
   // email address.  In this case FlagName = "mailto", ArgumentName might be
   // something like "user@host", and Description might be "Mail output to 
   // specified email address."
   void addFlagWithArgument(const COLstring& FlagName, 
                            const COLstring& ArgumentName);

   // Register a flag which has an argument, and provide a usage description.
   void addFlagWithArgument(const COLstring& FlagName,
                            const COLstring& ArgumentName,
                            const COLstring& Description);

   // sets RepeatOk for the given flag
   void setFlagRepeatOk(const COLstring& FlagName, bool RepeatOk);

   // number of repeated arguments for the given flag
   int flagRepeatCount(const COLstring& FlagName) const;

   // get a given repeated argument for the given flag
   const COLstring& flagRepeatValue(const COLstring& FlagName, int RepeatIndex) const;

   //get argument for a flag. returns "" if flag doesn't have arguments
   void flagArgument(const COLstring& FlagName, COLstring& Argument) const;

   const COLstring& flagArgument(const COLstring& FlagName) const;

   // This command will display the list of flags that the class knows about
   void showUsage(COLostream& OutStream) const;

   // hides a flag from the showUsage and --kissbook call
   void hideFlag(const COLstring& FlagName);
   
   // If an error was encountered while parsing the arguments, return true.
   // Also, will write a summary of the error to ErrorStream.
   // Possible errors are:
   //  * An undefined flag was present
   //  * A flag was used more than once
   bool parsingErrorsPresent(COLostream& ErrorStream) const;

   // Set whether the program expects a list of files and a description for them
   void setHasFileList(const bool HasFileList, const COLstring& FileListDescription);

   void addExtraParamDescription(const COLstring& Parameter, const COLstring& Description);
   void addExtraParamDescriptionRequired(const COLstring& Parameter, const COLstring& Description);
   bool findExtraParam(const COLstring& Parameter, int& ParamIndexOut) const;
   void setDescription(const COLstring& Description);

#ifdef COL_ENABLE_TRACING
   void addRuntimeConditionalLoggingFlags(); // for COLlog support
#endif

private:
   CMDlineParserPrivate* pMember;
   // disallowed operations
   CMDlineParser(const CMDlineParser& Orig);
   CMDlineParser& operator=(const CMDlineParser& Orig);
};

#ifdef CMD_LINE_PARSER_FULL
void CMDlineParserRepeatValueList(COLarray<COLstring>& ValueList, const CMDlineParser& LineParser, const COLstring& FlagName);
#endif

#endif // end of defensive include
