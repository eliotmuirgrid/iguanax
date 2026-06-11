// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: main.cpp
//
//  Description: Command line utility to generate log data
//
//  Author: Vismay Shah
//  Date:   Tuesday September 5, 2023
// ---------------------------------------------------------------------------
#include <cstdlib>
#include <cstdio>
#include <memory> // TODO - get rid of this
#include <time.h>

#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILdirEnumerator.h>

#include <CMD/CMDlineParser.h>
#include <DIR/DIRpath.h>
#include <SCK/SCKutils.h>

#include <SDB/SDBflushLog.h>
#include <SDB/SDBmessage.h>
#include <SDB/SDBdir.h>

#include <COL/COLauto.h>
#include <COL/COLerror.h>
#include <COL/COLlist.h>
#include <COL/COLmap.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#define SEARCH_STRING "My magicString 123%$"

static SDBmessage* createLogMessage(const COLstring& ComponentId, const COLstring& iData, COLuint64 DateTime, bool Special){
   // COL_FUNCTION(createLogMessage);
   COLstring Data = iData;
   if (Special) { Data.append(SEARCH_STRING); }
   return new SDBmessage(ComponentId, Data, DateTime, 1);
}

static void setOldestLogAge(time_t& Date, int Months){
   COL_FUNCTION(setOldestLogAge);
   int SecondsInAMonth = 30 * 24 * 60 * 60;
   Date = Date - (Months * SecondsInAMonth);
   struct tm* TimeInfo = localtime(&Date);
   char Buffer[100]; // Generous, but time zones on windows can get long.
   int Written = strftime(Buffer, sizeof(Buffer), "%A %b %d %Y %H:%M", TimeInfo);
   COLcout << "The oldest log generated will be " << Date << " which is " << Buffer << newline;
}

static COLstring getLogMessage(const COLstring& Data){
   // COL_FUNCTION(getLogMessage);
   int StartPos = rand() % 5000000;       // start at a random position anywhere in the first 5mb of Shakespeare.txt
   return Data.substr(StartPos, 1000);
}

static void generateMessageList(COLauto<COLlist<std::shared_ptr<SDBmessage>>> pMessages, int Months, const COLstring& ComponentId, 
                                time_t& StartDate, const COLstring& Data, COLuint64 Rarity, int Volume) {
   COL_FUNCTION(generateMessageList);
   pMessages.setIsOwner(false);
   int Count             = Months * 30 * Volume; //rough approx.
   int MonthCount        = 1;
   int SearchStringCount = 0;
   COLuint64 Date        = StartDate * 1000;
   int DateIncrement     = 86400 * 1000 / Volume; // time between messages = number of milliseconds in day / number of messages per day
   bool Special;
   COLlist<COLstring> SpecialStringDates;
   for (int i = 1; i < Count + 1; ++i) {
      if (i % Rarity == 0) { 
         Special = true;
         SearchStringCount++;
         SpecialStringDates.push_back(COLuint64ToString(Date));
      } else {
         Special = false;
      }
      pMessages->push_back(std::shared_ptr<SDBmessage>(createLogMessage(ComponentId, getLogMessage(Data), Date, Special)));
      Date = Date + DateIncrement;
      if (i % (30 * Volume) == 0) { // increment month every 30 days
         COLcout << "Month "<< MonthCount << " of " << Months << " done" << newline; 
         MonthCount++;
      }
   }
   COLcout << "Generated a total of " << Count << " messages, with " << SearchStringCount << " occurances of the magic string \'" << SEARCH_STRING << "\'" << newline;
   COLcout << "Magic string written on following timestamps (in milliseconds): " << newline;
   auto it = SpecialStringDates.cbegin();
   for (it; it != SpecialStringDates.cend(); it++){
      COLcout << it->c_str() << newline;
   }
}

static void writeLogs(COLstring ComponentId, COLauto<COLlist<std::shared_ptr<SDBmessage>>> pMessageList, const COLstring& LogDir){
   COL_FUNCTION(writeLogs);
   COL_VAR(LogDir);
   SDBflushLogMessages(ComponentId, pMessageList, LogDir);
}

static COLstring readDataFile(){
   COL_FUNCTION(readDataFile);
   COLstring Out;
   FILreadFile("Shakespeare.txt", &Out);
   return Out;
}

COLstring serverDirectory(){
   COL_FUNCTION(serverDirectory);
   FILdirEnumerator DirEnumerator(DIRconfigDirDefault() + "*", true, true);
   COLstring Dir;
   while (DirEnumerator.getFile(Dir)){
      if ((Dir[0] != '_')){
         return DIRconfigDirDefault() + Dir + FIL_DIR_SEPARATOR;
      } else {
         COL_TRC("Ignoring " << Dir);
      }
   }
   return "";
}

int main(int argc, const char** argv) {
   SCKinitWinsock();
   CMDlineParser Parser;
   COLcout << "Log Generator Tool 1.0." << newline;
   COLcout << "This tool generates log messages that are about 1kb in size using Shakespeare's public works. 1000 log messages are created per day. One in 10000 messages has a magic string appended to it." << newline;
   // required arg
   Parser.addFlagWithArgument("component", "The component guid to generate the logs for");
   // optional args
   Parser.addFlagWithArgument("months",    "How many months worth of logs to generate");
   Parser.addFlagWithArgument("directory", "The directory to output the logs to");
   Parser.addFlagWithArgument("rarity",    "Number of logs between writes of the magic string. Default: 10000");
   Parser.addFlagWithArgument("volume",    "Number of log messages to generate per day. Each log message is 1 kb. Default: 1000.");

   try {
      Parser.parseArgs(argc, argv);
      if(Parser.parsingErrorsPresent(COLcout)) {
         Parser.showUsage(COLcout);
         return EXIT_FAILURE;
      }
      // Command line arguments
      int Months = 36;
      if (Parser.isFlagPresent("months"))    { Months = atoi(Parser.flagArgument("months").c_str()); }
      if (!(Months > 0)) {
         COLcout << "If using \"--months\" the value must be greater than 0." << newline;
         return EXIT_FAILURE;
      }

      COLstring Component;
      if (!Parser.isFlagPresent("component")){
         COLcout << "must use --component to specify the component guid" << newline; 
         return EXIT_FAILURE;
      } else { Component = Parser.flagArgument("component"); }

      COLstring LogDirectory;
      if (Parser.isFlagPresent("directory")) { 
         LogDirectory = FILexpand(Parser.flagArgument("directory"));
         COLcout << "Will output the logs to " << LogDirectory << newline;
      } else {
         LogDirectory = serverDirectory();
         LogDirectory = FILpathAppend(LogDirectory, SDBroot());
         COLcout << "Will output the logs to " << LogDirectory << newline;
      }

      COLuint64 Rarity = 10000;
      if (Parser.isFlagPresent("rarity")) { 
         Rarity = atoll((Parser.flagArgument("rarity").c_str()));
         COLcout << "Will write magic string every " << Rarity << " logs" << newline;
      }

      int Volume = 1000;
      if (Parser.isFlagPresent("volume")) { 
         Volume = atoi((Parser.flagArgument("volume").c_str()));
         COLcout << "Will generate " << Volume << " log messages per day" << newline;
      }
      COL_VAR4(Component, Months, LogDirectory, Rarity);
      // implementation
      COLcout << "Reading in Shakespeare" << newline;
      COLstring Data = readDataFile();
      COLcout << "Generating " << Months << " months of logs" << newline;
      COLcout << "Use search string: " << SEARCH_STRING << newline;
      time_t Date = time(NULL);
      setOldestLogAge(Date, Months);
      COLauto<COLlist<std::shared_ptr<SDBmessage>>> pMessages = new COLlist<std::shared_ptr<SDBmessage>>;
      SDBcontentMapInit();
      COLcout << "Generating messages..." << newline;
      generateMessageList(pMessages, Months, Component, Date, Data, Rarity, Volume);
      FILmakeFullDir(LogDirectory, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
      COLcout << "Writing logs to " << LogDirectory << newline;
      writeLogs(Component, pMessages, LogDirectory);
      COLcout << "Finished." << newline;
   } catch(const COLerror& Error) {
      COLcout << Error.description() << newline;
      return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
}
