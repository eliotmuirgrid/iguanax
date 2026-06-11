//------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module:  CMDlineParser
// 
// Description
//
// Implementation of a utility class which makes it easy to get command line
// arguments.  
//
// Author: Eliot Muir
// Date:   Thu 10/03/2002 
//
//---------------------------------------------------------------------------
#include "CMDlineParser.h"

#include <COL/COLarray.h>
#include <COL/COLlist.h>
#include <COL/COLref.h>

COL_LOG_MODULE;

// NOTE https://willus.com/mingw/_globbing.shtml
// 0 turns off globbing; 1 turns it on
int _CRT_glob = 0; 

class CMDextraParamDescription{
public:
   CMDextraParamDescription() 
    : ParamRequired(false){}
   CMDextraParamDescription& operator=(const CMDextraParamDescription& Orig)
   {
      ParamName = Orig.ParamName;
      ParamDescription = Orig.ParamDescription;
      ParamRequired = Orig.ParamRequired;
      return *this;
   }
   CMDextraParamDescription(const CMDextraParamDescription& Orig)
   {
      operator=(Orig);
   }
   CMDextraParamDescription(const COLstring& Name, 
                            const COLstring& Description,
                            bool       IsRequired) :
      ParamName(Name), 
      ParamDescription(Description),
      ParamRequired(IsRequired) {}
 
   COLstring ParamName;
   COLstring ParamDescription;
   bool ParamRequired;
};


class CMDlineFlag : public COLrefCounted
{
public:
   CMDlineFlag(const COLstring& iName)
    : Name(iName),
      IsHidden(false),
      IsPresent(false),
      WasExpected(false),
      HasArgument(false),
      ArgumentName(""),
      WasRepeated(false),
      RepeatOk(false)
   {
   }

   COLstring   Name;
   COLstring   Description;
   bool  HasArgument;
   bool  IsPresent;
   bool  WasExpected;
   COLstring   Argument;
   COLstring   ArgumentName;
   bool  IsHidden;
   bool  WasRepeated;
   COLarray<COLstring> ValueList;
   bool  RepeatOk;

private:
   CMDlineFlag(const CMDlineParser& Orig);
   CMDlineFlag operator=(const CMDlineParser& Orig);
};


//Number of spaces between the Name of the Argument and its description columns
static const COLuint32 CMD_NUMBER_OF_PADDING_SPACES = 2;
//Maximum width of output when showing usage
static const COLuint32 CMD_SHOW_USAGE_WIDTH = 75;

//TODO: the TODO below is not really neccessary since isFlagPresent takes care
//of the problem nicely
//TODO - put in check that parseArgs has been invoked before
//allowing LineParser.flagPresent to be called etc.

class CMDlineParserPrivate
{
public:
   CMDlineParserPrivate()
      : HasParsed(false)
   {
   }

   COLstring Description;
   COLstring ProgramName;

   bool HasParsed;

   bool HasFileList;

   COLarray<COLstring> ExtraArgs;

   COLarray<CMDextraParamDescription> ParamDescription; 

   COLarray<COLstring> FileList;

   COLlist< COLref<CMDlineFlag> > FlagList;

   // we use this for passing empty strings back to applications
   // since we're not allowed to pass temporary strings back in methods.
   COLstring NullString;  

   bool isFlag(const char* pArgument) const;

   COLref<CMDlineFlag> createFlagIfNotInList(const COLstring& FlagName);
   COLref<CMDlineFlag> addPresentFlag(const char* pArgument);

   COLref<CMDlineFlag> flag(const COLstring& FlagName);
   bool isFlagInList(const COLstring& FlagName);

   void addArgument(const char* pArgument);

   //sets which flags will be counted as help flags, eg --help
   bool isHelpArgument(const COLstring& FlagName) const;
   bool isHelpFlag(const COLstring& FlagName) const;
private:
   CMDlineParserPrivate(const CMDlineParserPrivate& Orig);
   CMDlineParserPrivate& operator=(const CMDlineParserPrivate& Orig);
};

COLref<CMDlineFlag> CMDlineParserPrivate::createFlagIfNotInList(const COLstring& FlagName)
{
   COLref<CMDlineFlag> pFlag = flag(FlagName);
   if (NULL == pFlag.get())
   {
      pFlag = new CMDlineFlag(FlagName);
      FlagList.add(pFlag);
   }
   return pFlag;
}

COLref<CMDlineFlag> CMDlineParserPrivate::addPresentFlag(const char* pArgument)
{
   COLstring FlagName = pArgument;
   FlagName = FlagName.strip(COLstring::Leading, '-');

   COLref<CMDlineFlag> pFlag = flag(FlagName);
   if (NULL == pFlag.get())
   {
      pFlag = new CMDlineFlag(FlagName);
      FlagList.add(pFlag);
   }
   else if (pFlag->IsPresent)
   {
      pFlag->WasRepeated = true;
   }
   pFlag->IsPresent = true;

   return pFlag;
}

COLref<CMDlineFlag> CMDlineParserPrivate::flag(const COLstring& FlagName)
{
   for (COLlistPlace Place = FlagList.first(); Place != NULL; Place = FlagList.next(Place))
   {
      if (FlagList[Place]->Name == FlagName)
      {
         return FlagList[Place];
      }
   }

   return NULL;
}

bool CMDlineParserPrivate::isFlagInList(const COLstring& FlagName)
{
   return NULL != flag(FlagName).get();
}

void CMDlineParserPrivate::addArgument(const char* pArgument)
{
   ExtraArgs.push_back(pArgument);  
}

CMDlineParser::CMDlineParser()
{
   pMember = new CMDlineParserPrivate();
}

CMDlineParser::~CMDlineParser()
{
   delete pMember;
}

bool CMDlineParserPrivate::isFlag(const char* pArgument) const
{
   // flags begin with at least 1 dash (ie. -FlagName, --FlagName).
   if (strlen(pArgument) <= 1)
   {
      return false;
   }
   if (pArgument[0] == '-')
   {
      COLstring FlagString(pArgument);
      if (FlagString.strip(COLstring::Leading, '-').is_null())
      {
         return false;
      }
      else
      {
         return true;
      }
   }
   else
   {
      return false;
   }
}

void CMDlineParser::addExtraParamDescriptionRequired(const COLstring& Parameter, const COLstring& Description)
{
   pMember->ParamDescription.push_back(CMDextraParamDescription(Parameter, Description, true));
}

void CMDlineParser::addExtraParamDescription
(
   const COLstring& Parameter, 
   const COLstring& Description
)
{
   pMember->ParamDescription.push_back(CMDextraParamDescription(Parameter, Description, false));
}

bool CMDlineParser::findExtraParam(const COLstring& Parameter, int& ParamIndexOut) const
{
   for (COLuint32 ExtraArgIndex=0; ExtraArgIndex < static_cast<COLuint32>(pMember->ParamDescription.size()); ++ExtraArgIndex)
   {
      if (Parameter == pMember->ParamDescription[ExtraArgIndex].ParamName)
      {
         ParamIndexOut = ExtraArgIndex;
         return true;
      }
   }

   return false;
}

void CMDlineParser::setDescription(const COLstring& Description)
{
   pMember->Description = Description;
}

void CMDlineParser::setHasFileList(const bool HasFileList, const COLstring& FileListDescription)
{
   pMember->HasFileList = HasFileList;
   addExtraParamDescription("Files to be processed", FileListDescription);
}

#ifdef COL_ENABLE_TRACING
void CMDlineParser::addRuntimeConditionalLoggingFlags()
{
   if (pMember->HasParsed) return;
   // the following flags are used by COLlog and are to be ignored by CMD:
   addFlagWithArgument("out"  , "<filename>", "COLlog filename");
   addFlagWithArgument("d"    , "<modules>", "COLlog DBG");
   addFlagWithArgument("trace", "<modules>", "COLlog TRC");
   addFlagWithArgument("i"    , "<modules>", "COLlog INF");
   addFlagWithArgument("w"    , "<modules>", "COLlog WRN");
   addFlagWithArgument("e"    , "<modules>", "COLlog ERR");
   addFlagWithArgument("F"    , "true|false", "COLlog flush");
   addFlagWithArgument("n"    , "true|false", "COLlog indenting");
}
#endif

void CMDlineParser::parseArgs(int argc, const char** ppArg){
#ifdef COL_ENABLE_TRACING
   COL_LOG_INIT(argc, ppArg);
#endif
   parseArgsNoTrace(argc,ppArg); 
}

void CMDlineParser::parseArgsNoTrace(int argc, const char** ppArg){
#ifdef COL_ENABLE_TRACING
   addRuntimeConditionalLoggingFlags(); // must be called before HasParsed is set.
#endif
   if (pMember->HasParsed){
      COL_ERROR_STREAM_PLAIN("CMDlineParser::parseArgs() has been called more than once.", COLerror::PreCondition);
   }
   pMember->HasParsed = true;
   COLPRECONDITION(argc != 0);
   pMember->ProgramName = ppArg[0];
   if (argc == 1) {
      return;
   }
   bool InFlagMode = true;
   COLint32 ArgIndex = 1;
   while (InFlagMode && (ArgIndex < argc)){
      // In the case of a help flag, eg -help, /? that's not a standard flag,
      // add it to the table anyways and set it to be unexpected,
      // so a program can test for unknownFlagPresent and do a showUsage().
      if(pMember->isHelpArgument(ppArg[ArgIndex])){
         COLref<CMDlineFlag> pFlag = pMember->addPresentFlag(ppArg[ArgIndex]);
         pFlag->WasExpected = false;
         ArgIndex++;
      }

      // Otherwise it's a normal flag, add it to the table if it doesn't
      // already exist
      else if (pMember->isFlag(ppArg[ArgIndex])) {
         COLref<CMDlineFlag> pFlag = pMember->addPresentFlag(ppArg[ArgIndex]);
         if (pFlag->HasArgument){
            // at present, a flag can have at most one argument following it
            ArgIndex++;
            if (ArgIndex < argc){ // we can't assume that the the last flag's argument will be there if needed!
               pFlag->Argument = ppArg[ArgIndex];
               pFlag->ValueList.push_back() = ppArg[ArgIndex];
            } else {
               COL_ERROR_STREAM_PLAIN("Argument missing for flag " << ppArg[ArgIndex-1], COLerror::SystemError);
            }
         }
         ArgIndex++;
      } else{ // Not a flag
         InFlagMode = false;
      }      
   }
   
   while (ArgIndex < argc) // Add the plain arguments to the list
   {
      pMember->addArgument(ppArg[ArgIndex]);
      ArgIndex++;
   }   
}

const COLstring& CMDlineParser::programName() const {
   return pMember->ProgramName;
}

// This is the count of the files passed in, with wildcards expanded
const int CMDlineParser::countOfFile() const
{
   return pMember->FileList.size();
}

// This returns the filename at the given position
const COLstring& CMDlineParser::fileName(const int FileIndex) const
{
   return pMember->FileList[FileIndex];
}

// This is the count of extra arguments actually found 
int CMDlineParser::countOfExtraArgument() const
{
   return pMember->ExtraArgs.size();
}

// retrieve the value of an extra argument
const COLstring& CMDlineParser::extraArgument(int ArgumentIndex) const {
   return pMember->ExtraArgs[ArgumentIndex]; 
}

// retrieve the value of an extra argument
const COLstring& CMDlineParser::extraArgument(const COLstring& Parameter) const {
   int ArgumentIndex = 0;
   if (findExtraParam(Parameter, ArgumentIndex)) {
      return pMember->ExtraArgs[ArgumentIndex]; 
   } else {
      static const COLstring EmptyString;
      return EmptyString;
   }
}

// Is a flag present - true/false -- new one, use this one from now on
// i.e. --BLAH
bool CMDlineParser::isFlagPresent(const COLstring& FlagName) const {
   if (!pMember->isFlagInList(FlagName)) {
      // This flag has not been installed by the users of this class
      COL_ERROR_STREAM("EXCEPTION: isFlagPresent was asked to check for the presence of flag " << FlagName << " which was not installed. "
                        << "A flag should be first installed using either addFlagWithArgument or addFlagWithoutArgument before it is checked for.", COLerror::PreCondition);
   }
   return pMember->flag(FlagName)->IsPresent;
}

// Add a flag with argument 
void CMDlineParser::addFlagWithArgument ( const COLstring& FlagName, const COLstring& ArgumentName) {
   COLref<CMDlineFlag> pFlag = pMember->createFlagIfNotInList(FlagName);
   pFlag->WasExpected = true;
   pFlag->HasArgument = true;
   pFlag->ArgumentName = ArgumentName;
}

// Add a flag with argument, and provide a description 
void CMDlineParser::addFlagWithArgument(const COLstring& FlagName, const COLstring& ArgumentName, const COLstring& Description) {
   COLref<CMDlineFlag> pFlag = pMember->createFlagIfNotInList(FlagName);
   pFlag->WasExpected = true;
   pFlag->HasArgument = true;
   pFlag->ArgumentName = ArgumentName;
   pFlag->Description = Description;
}

void CMDlineParser::setFlagRepeatOk(const COLstring& FlagName, bool RepeatOk) {
   COLref<CMDlineFlag> pFlag = pMember->flag(FlagName);
   COLMSGPRECONDITION(pFlag.get(), "setFlagRepeatOk(): Flag " << FlagName << " not yet defined");
   COLMSGPRECONDITION(pFlag->HasArgument, "setFlagRepeatOk(): Flag " << FlagName << " doesn't have argument");
   pFlag->RepeatOk = RepeatOk;
}

// Add a flag without any arguments
void CMDlineParser::addFlagWithoutArgument ( const COLstring& FlagName, const COLstring& Description) {
   COLref<CMDlineFlag> pFlag = pMember->createFlagIfNotInList(FlagName);
   pFlag->WasExpected = true;
   pFlag->HasArgument = false;
   pFlag->Description = Description;   
}

// This command will display the list of flags that the class knows about.
// TODO - clean this up!
void CMDlineParser::showUsage(COLostream& OutStream) const {
   // Output the usage line that displays all the possible flags/arguments
   // TODO - this output does not wrap, but the output for "known flags" wraps. Create a printWrap function?
   OutStream << pMember->Description;
   OutStream << newline << "Usage:" << newline;
   OutStream << pMember->ProgramName;

   COLstring FlagName;
   COLref<CMDlineFlag> pCommandLineFlag;

   COLuint32 MaxFlagLength = 0;
   COLuint32 MaxDescriptionLength = 0;

   if (pMember->FlagList.size() > 0)
   {
      for (COLlistPlace Place = pMember->FlagList.first(); Place != NULL; Place = pMember->FlagList.next(Place))
      {
         pCommandLineFlag = pMember->FlagList[Place];
         FlagName = pCommandLineFlag->Name;
         if (pCommandLineFlag->WasExpected && !pCommandLineFlag->IsHidden)
         {
            if (pCommandLineFlag->HasArgument) {
               OutStream << "  [--" << FlagName << ' ' << pCommandLineFlag->ArgumentName << ']'; 
            } else {
               OutStream << "  [--" << FlagName << ']'; 
            }
            
            //Get the maximum length of the command line flag and its description; used to
            //pad the output columns to make a nicer output
            if(FlagName.size() > MaxFlagLength) {
               MaxFlagLength = static_cast<COLuint32>(FlagName.size());
            }
            if(pCommandLineFlag->Description.size() > MaxDescriptionLength) {
               MaxDescriptionLength = static_cast<COLuint32>(pCommandLineFlag->Description.size());
            }

         }
      } 
   }
   int ExtraArgIndex=0;
   for (ExtraArgIndex=0; ExtraArgIndex < static_cast<COLuint32>(pMember->ParamDescription.size()); ExtraArgIndex++) {
      if (pMember->ParamDescription[ExtraArgIndex].ParamRequired) {
         OutStream << ' ' << pMember->ParamDescription[ExtraArgIndex].ParamName;
      } else {
         OutStream << " <" << pMember->ParamDescription[ExtraArgIndex].ParamName << '>';
      }
   }

   OutStream << newline;
   for (ExtraArgIndex=0; ExtraArgIndex < static_cast<COLuint32>(pMember->ParamDescription.size()); ExtraArgIndex++) {
      OutStream << "   " << pMember->ParamDescription[ExtraArgIndex].ParamName << 
                   " - " << pMember->ParamDescription[ExtraArgIndex].ParamDescription << newline;
   }
   if (pMember->FlagList.size() == 0) {
      return;
   }
   
   // Print out the flags together with their descriptions
   OutStream << "Known flags:" << newline;

   COLuint32 MaxLineLength = 70; // width of output TODO - can we get this to be dynamic?

   for (COLlistPlace Place = pMember->FlagList.first(); Place != NULL; Place = pMember->FlagList.next(Place)) {
      pCommandLineFlag = pMember->FlagList[Place];
      FlagName = pCommandLineFlag->Name;
      if (pCommandLineFlag->WasExpected && !pCommandLineFlag->IsHidden) {
         if(FlagName.size() <= MaxFlagLength) {
            COLstring PaddingSpaces(MaxFlagLength - FlagName.size() + CMD_NUMBER_OF_PADDING_SPACES, ' ');
            COLstring FlagInfo = "  --" + FlagName + PaddingSpaces;
            COLstring FlagDescription = pCommandLineFlag->Description;
            if (pCommandLineFlag->HasArgument) {
               // If the flag has a non empty description, we need a space!
               FlagDescription += FlagDescription.compare("") == 0 ? "": ". ";
               FlagDescription += "Requires argument " + pCommandLineFlag->ArgumentName + ".";
            }
            
            OutStream << FlagInfo;

            COLstring RemainingDescription = FlagDescription;
            COLstring DescriptionToStream;
            while (FlagInfo.size() + RemainingDescription.size() > MaxLineLength) {
               // find first carriage return in RemainingDescription
               if (RemainingDescription.find("\n") < (MaxLineLength - FlagInfo.size())) {
                  RemainingDescription.split(DescriptionToStream, RemainingDescription, "\n");
               } else {
                  // Description must be split onto two or more lines for proper formatting
                  COLuint32 SplitIndex = static_cast<COLuint32>(RemainingDescription.rfind(" ", MaxLineLength - FlagInfo.size()));
                  RemainingDescription.splitAtIndex(DescriptionToStream, RemainingDescription, SplitIndex + 1);
               }

               OutStream << DescriptionToStream << newline << COLstring(FlagInfo.size(), ' ');
            }
            
            if (RemainingDescription.find("\n") < RemainingDescription.size()) {
               // there is a carriage return somewhere in the middle of this last line
               RemainingDescription.split(DescriptionToStream, RemainingDescription, "\n");
               OutStream << DescriptionToStream << newline << COLstring(FlagInfo.size(), ' ');
            }
            OutStream << RemainingDescription << newline;
         } else {
            COL_ERROR_STREAM("Flag " << FlagName << " length exceeds the maximum flag length of "<< MaxFlagLength,COLerror::PreCondition)
         }
      }
   }
}

const COLstring& CMDlineParser::flagArgument(const COLstring& FlagName) const {
   if (!pMember->isFlagInList(FlagName)) {
      COL_ERROR_STREAM(FlagName << " not defined.", COLerror::PreCondition);
   }
   if(pMember->flag(FlagName)->HasArgument) {
      return pMember->flag(FlagName)->Argument;
   }
   return pMember->NullString;
}

//get argument for a flag. returns "" if flag doesn't have arguments
void CMDlineParser::flagArgument(const COLstring& FlagName,COLstring& Argument) const {
   if (!pMember->isFlagInList(FlagName)) {
      COL_ERROR_STREAM(FlagName << " not defined.", COLerror::PreCondition);
   } else {
      if(pMember->flag(FlagName)->HasArgument) {
         Argument = pMember->flag(FlagName)->Argument;
      } else {
         Argument = "";
      }
   }
}

int CMDlineParser::flagRepeatCount(const COLstring& FlagName) const {
   if (!pMember->isFlagInList(FlagName)) {
      COL_ERROR_STREAM(FlagName << " not defined.", COLerror::PreCondition);
   }
   return pMember->flag(FlagName)->ValueList.size();
}

const COLstring& CMDlineParser::flagRepeatValue(const COLstring& FlagName, int RepeatIndex) const {
   if (!pMember->isFlagInList(FlagName)) {
      COL_ERROR_STREAM(FlagName << " not defined.", COLerror::PreCondition);
   }
   COLPRECONDITION(RepeatIndex >= 0 && RepeatIndex < pMember->flag(FlagName)->ValueList.size());
   return pMember->flag(FlagName)->ValueList[RepeatIndex];
}

bool CMDlineParser::parsingErrorsPresent(COLostream& ErrorStream) const {
   COLstring FlagName;
   COLref<CMDlineFlag> pCommandLineFlag;

   for (COLlistPlace Place = pMember->FlagList.first(); Place != NULL; Place = pMember->FlagList.next(Place)) {
      pCommandLineFlag = pMember->FlagList[Place];
      FlagName = pCommandLineFlag->Name;
      if (!pCommandLineFlag->WasExpected) {
         // #10739 We don't want to see this warning when a help flag is found, and only show the usage.
         // This is not an ideal solution, but the help flag is only known to the command line parser
         // and we don't want to change the code for all our command line applications.
         if (!pMember->isHelpFlag(pCommandLineFlag->Name)) {
            ErrorStream << "Unknown flag " << FlagName << " encountered." << newline;
         }
         return true;
      }
      if (pCommandLineFlag->WasRepeated && !pCommandLineFlag->RepeatOk) {
         ErrorStream << "Flag " << FlagName << " used more than once." << newline;
         return true;
      }
   }
   return false;
}

void CMDlineParser::hideFlag(const COLstring& FlagName) {
   if (pMember->isFlagInList(FlagName)) {
      pMember->flag(FlagName)->IsHidden = true;
   }
}

bool CMDlineParserPrivate::isHelpArgument(const COLstring& FlagString) const {
   if(FlagString == "--help"|| FlagString == "-help" || FlagString == "-?" || FlagString == "--?"|| FlagString == "/?"|| FlagString == "-h") {
      return true;
   }
   return false;
}

bool CMDlineParserPrivate::isHelpFlag(const COLstring& FlagString) const {
   if(FlagString == "help"|| FlagString == "?" || FlagString == "/?"|| FlagString == "h") {
      return true;
   }
   return false;
}

void CMDlineParserRepeatValueList(COLarray<COLstring>& ValueList, const CMDlineParser& LineParser, const COLstring& FlagName){
   for(int ValueIndex=0; ValueIndex < LineParser.flagRepeatCount(FlagName); ValueIndex++){
      ValueList.push_back() = LineParser.flagRepeatValue(FlagName, ValueIndex);
   }
}
