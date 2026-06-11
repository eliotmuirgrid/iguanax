// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBAlogExportSample
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Sunday 14 May 2023 - 05:51PM
// ---------------------------------------------------------------------------
#include <SDBU/SDBUutils.h>
#include <COL/COLlist.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SDB/SDBcontent.h>
#include <SDBA/SDBAlogExportSample.h>
COL_LOG_MODULE;

static bool SDBAlogExportCheckSampleName(const COLstring& ToComponent, const COLstring& Name){
   COL_FUNCTION(SDBAlogExportCheckSampleName);
   COLstring Check = FILpathAppend(DIRcomponentSampleDir(ToComponent), Name);
   COL_TRC("Check for " << Check);
   return FILfileExists(Check);
}

static void SDBAlogExportSampleImp(const COLstring& Component, const COLstring& ToComponent, COLuint64 Time, int Index, const COLstring& Name, const COLstring* pMessage, const COLaddress& Address){
   COL_FUNCTION(SDBAlogExportSampleImp);
   COLstring File = Component + COLint64ToString(Time) + COLint64ToString(Index);
   if (!Name.is_null()){
      COL_TRC("Saving as " << Name);
      File = Name;
   }
   COLstring OutFile = DIRcomponentSampleDir(ToComponent) + File;
   COLstring Data, Err;
   if (pMessage){
      Data = *pMessage;
   } else if(!SDBUextractMessage(Component, Index, Time, &Data, &Err)) {
      return COLrespondError(Address, Err);
   }
   if(!FILfileExists(DIRcomponentSampleDir(ToComponent))) { FILmakeFullDir(DIRcomponentSampleDir(ToComponent), FIL_USER_RWX); }
   FILwriteFile(OutFile, Data);
   COLrespondSuccess(Address, File);
}

// /log/export/sample
void SDBAlogExportSample(const COLwebRequest& Request){
   COL_FUNCTION(SDBAlogExportSample);
   const COLvar& Data    = Request.Data;
   int       Index       = COLvarRequiredInt(Data, "id");
   COLstring Component, Error;
   if(!SDBUcomponentId(COLvarGetString(Request.Data, "component", ""), &Component, &Error)){ 
      return COLrespondError(Request.Address, Error); 
   }
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, Component, ROLid::EXPORT_SAMPLE_DATA, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::EXPORT_SAMPLE_DATA));
   }

   COLstring ToComponent = COLvarRequiredString(Data, "to");
   COLstring TimeString = COLvarGetString(Data, "time", "0");
   COLuint64 Time = atoll(TimeString.c_str());
   COLstring Name = COLvarGet(Data, "name", "");
   COLstring Message = COLvarGetString(Data, "message", "");
   bool HasEditedMessage = Data.exists("message");
   COL_VAR(Name);
   if(Time < 0) {
      return COLrespondError(Request.Address, "Time parameter must be positive");
   }
   COL_VAR4(Component, ToComponent, Index, Time);
   if (!Name.is_null() && SDBAlogExportCheckSampleName(ToComponent, Name)){
      return COLrespondError(Request.Address, "This component already has a sample data file with this name. Please use a different name.");
   }
   SDBAlogExportSampleImp(Component, ToComponent, Time, Index, Name, HasEditedMessage ? &Message : nullptr, Request.Address);
}