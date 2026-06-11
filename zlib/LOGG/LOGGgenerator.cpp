// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LOGGgenerator
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Wednesday 20 September 2023 - 03:24PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <LOGG/LOGGgenerator.h>
#include <LOGG/LOGGgeneratorStatus.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKloop.h>
#include <SDB/SDBcapi.h>
#include <SDB/SDBdir.h>
#include <SDB/SDBlogWrite.h>
#include <SDB/SDBmessage.h>
#include <SDB/SDBsystem.h>
#include <WEB/WEBrequest.h>
#include <WEB/WEBresponse.h>
COL_LOG_MODULE;

#define LOGG_SHAKESPEARE_URL "https://downloads.interfaceware.com/resources/Shakespeare.txt"
#define LOGG_SEARCH_STRING "My magicString 123%$"

static void LOGGsetOldestLogAge(time_t& Date, int Months){
   COL_FUNCTION(LOGGsetOldestLogAge);
   tm* Now = localtime(&Date);
   Now->tm_mon -= Months;
   while (Now->tm_mon < 0) { // Ensure the month value is within the valid range
      Now->tm_mon += 12;
      Now->tm_year--;
   }
   Date = ::mktime(Now); // Convert modified tm struct back to time_t
   // struct tm* TimeInfo = localtime(&Date);
   // char Buffer[100]; // Generous, but time zones on windows can get long.
   // int Written = strftime(Buffer, sizeof(Buffer), "%A %b %d %Y %H:%M", TimeInfo);
   // COLcout << "The oldest log generated will be " << Date << " which is " << Buffer << newline;
}

static SDBmessage* LOGGcreateLogMessage(const COLstring& ComponentId, const COLstring& iData, COLuint64 DateTime, bool Special){
   // COL_FUNCTION(createLogMessage);
   COLstring Data = iData;
   if (Special) { Data.append(LOGG_SEARCH_STRING); }
   return new SDBmessage(ComponentId, Data, DateTime, 1);
}

static COLstring LOGGgetLogMessage(const COLstring& Data, int LogSize){
   // COL_FUNCTION(getLogMessage);
   int StartPos = rand() % (Data.size() - LogSize + 1); // start at a random position, ensuring the substring will be ok
   return Data.substr(StartPos, LogSize);
}

static void LOGGwriteLogs(const COLstring& ComponentId, COLauto<COLlist<std::shared_ptr<SDBmessage>>> pMessageList, const COLstring& LogDir, SDBsystem* pLogQueue){
   COL_FUNCTION(LOGGwriteLogs);
   COL_VAR(LogDir);
   if (pLogQueue->ComponentMap.count(ComponentId)){
      pLogQueue->ComponentMap[ComponentId].LastJournaledId = pMessageList.get()->back().get()->Id;
   }
   SDBlogTaskData Data;
   Data.pMessages = pMessageList;
   Data.FlushPosition = false;
   SDBlogHandle Handle;
   SDBflushLogMessages(Handle, Data, pLogQueue->LogEncryptionKey, LogDir);
}

static int LOGGdaysDifference(const time_t& StartDate){
   COL_FUNCTION(LOGGdaysDifference);
   time_t CurrentDate    = time(NULL);
   time_t TimeDifference = CurrentDate - StartDate;// Calculate the difference in seconds
   int DaysDifference    = TimeDifference / (24 * 60 * 60); // Convert the time difference to days (1 day = 24 * 60 * 60 seconds)
   return DaysDifference;
}

static void LOGGgenerateMessageList(int Months, const COLstring& ComponentId,
                                time_t& StartDate, const COLstring& Data, int Rarity, int Volume, int LogSize, SDBsystem* pLogQueue, SCKloop* pLoop) {
   COL_FUNCTION(LOGGgenerateMessageList);
   int SizeLimit         = 50000; // flush every 50,000 messages (~50mb of ram used)
   int Count             = LOGGdaysDifference(StartDate) * Volume;
   int MonthCount        = 1;
   int SearchStringCount = 0;
   COLuint64 Date        = StartDate * 1000;
   int DateIncrement     = 86400 * 1000 / Volume; // time between messages = number of milliseconds in day / number of messages per day
   bool Special;
   COLlist<COLstring> SpecialStringDates;
   COLauto<COLlist<std::shared_ptr<SDBmessage>>> pMessages = new COLlist<std::shared_ptr<SDBmessage>>();
   for (int i = 1; i < Count + 1; ++i) {
      if (i % Rarity == 0) {
         Special = true;
         SearchStringCount++;
         SpecialStringDates.push_back(COLuint64ToString(Date));
      } else {
         Special = false;
      }
      SDBmessage* pMessage = LOGGcreateLogMessage(ComponentId, LOGGgetLogMessage(Data, LogSize), Date, Special);
      pMessage->Id.SequenceId = i;
      pMessage->Id.QueueId = i;
      pMessages->push_back(std::shared_ptr<SDBmessage>(pMessage));
      Date = Date + DateIncrement;
      if (i % (30 * Volume) == 0) { // increment month every 30 days
         MonthCount++;
         pLoop->post(COLnewClosure0(&LOGGgenerationMonthDone));
      }
      if (i % SizeLimit == 0) { // In order to prevent using too much memory, flush to disk everytime we process 250,000 log messages
         COL_TRC("Flushing");
         LOGGwriteLogs(ComponentId, pMessages, SDBroot(), pLogQueue);  // flushes messages and frees memory
         pMessages = new COLlist<std::shared_ptr<SDBmessage>>();  // prep for next batch of messages
      }
   }
   if(pMessages->size() != 0) { LOGGwriteLogs(ComponentId, pMessages, SDBroot(), pLogQueue); } // this flushes whatever was left over
   COL_TRC("Generation of log messages list complete");
}

static bool LOGGhaveDataFile(const COLstring& DataFile){
   COL_FUNCTION(LOGGhaveDataFile);
   COL_TRC("Checking file: " + DataFile);
   return FILfileExists(DataFile);
}

static bool LOGGdownloadData(const COLstring& DataFile){
   COL_FUNCTION(LOGGdownloadData);
   WEBrequest Request;
   WEBresponse Response;
   Request.Url = LOGG_SHAKESPEARE_URL;
   WEBrequestBlockingGet(Request, &Response);
   COLstring Filename = DataFile;
   if (Response.ReturnCode == 200){
      COL_TRC("Writing result to " << Filename << ", size = " << Response.Body.size());
      FILwriteFile(Filename, Response.Body);
      COL_TRC("Download complete");
      return LOGGhaveDataFile(DataFile);
   } else {
      COL_TRC("Failed to download file");
      return false;
   }
}

void LOGGeneratorWorker(COLvar Args, SDBsystem* pLogQueue, SCKloop* pLoop){
   COL_FUNCTION(LOGGeneratorWorker);
   COLstring Component = COLvarRequiredString(Args, "component");
   int Months          = COLvarRequiredInt(Args, "months");
   int Volume          = COLvarRequiredInt(Args, "volume");
   int Rarity          = COLvarRequiredInt(Args, "rarity");
   int LogSize         = COLvarGet(Args, "size", 1000);
	COLstring DataFile = FILpathAppend(DIRresourcesDir(), "Shakespeare.txt");
   COL_TRC("Checking for data file");
   if (!LOGGhaveDataFile(DataFile)){
      COL_TRC("Data file not found, downloading...");
      if (!LOGGdownloadData(DataFile)) {
         pLoop->post(COLnewClosure0(&LOGGreportError, COLstring("Failed to download file. Please ensure you are connected to the internet.")));
         return;
      }
   }
   COLstring ShakespeareData;
   FILreadFile(DataFile, &ShakespeareData);
   COL_TRC("Updating download status");
   pLoop->post(COLnewClosure0(&LOGGdownloadDone));
   time_t Date = time(NULL);
   LOGGsetOldestLogAge(Date, Months);
   COL_TRC("Generating messages");
   LOGGgenerateMessageList(Months, Component, Date, ShakespeareData, Rarity, Volume, LogSize, pLogQueue, pLoop);
   COL_TRC("Updating generating messages status");
   pLoop->post(COLnewClosure0(&LOGGgenerationDone));
}

void LOGGgeneratorStart(const COLwebRequest& Request, SDBsystem* pLogQueue, SCKloop* pLoop){
   COL_FUNCTION(LOGGgeneratorStart);
   COLvar Args = Request.Data;
   COL_VAR(Args);
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::LOG_GENERATOR, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::LOG_GENERATOR));
   }
   COLstring Component = COLvarRequiredString(Request.Data, "component");
   int Months          = COLvarRequiredInt(Request.Data, "months");
   int Volume          = COLvarRequiredInt(Request.Data, "volume");
   int Rarity          = COLvarRequiredInt(Request.Data, "rarity");
   int LogSize         = COLvarGet(Request.Data, "size", 1000);
   if (Months < 1){ return COLrespondError(Request.Address, "Months must be greater than or equal to 1"); }
   if (Volume < 0){ return COLrespondError(Request.Address, "Volume must be greater than 0"); }
   if (Rarity < 0){ return COLrespondError(Request.Address, "Rarity must be greater than 0"); }
   if (LogSize <= 0){ return COLrespondError(Request.Address, "Log Size must be greater than 0"); }
   if (LogSize > 5000000) { return COLrespondError(Request.Address, "Log Size must be 5,000,000 or less"); }

   LOGGeneratorStatusReset();
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&LOGGeneratorWorker, Args, pLogQueue, pLoop));
   COLrespondSuccess(Request.Address);
}

void LOGGgeneratorStatus(const COLwebRequest& Request){
   // COL_FUNCTION(LOGGgeneratorStatus);
   COLvar Result;
   LOGGstatusVar(&Result);
   // COL_VAR(Result);
   COLrespondSuccess(Request.Address, Result);
}
