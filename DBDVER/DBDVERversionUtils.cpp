//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDVERversionString
//
// Description:
//
// Implementation.
//
// Author: Vismay Shah
//---------------------------------------------------------------------------
#include <DBDVER/DBDVERversion.h>
#include <DBDVER/DBDVERversionUtils.h>
#include <DBDVER/DBDVERgitInfo.h>
#include <DBDVER/DBDVERtime.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring DBDVERversionString(){
   COL_FUNCTION(DBDVERversionString);
   COLstring VersionString;
   COLostream Stream(VersionString);
   Stream << DBD_VERSION_MAJOR << '.' << DBD_VERSION_MINOR << '.' << DBD_BUILD_NUMBER;// << '-' << DBDVERbuildTypeString();
   return VersionString;
}

COLstring DBDVERstartupVersionInfo() {
   COL_FUNCTION(DBDVERstartupVersionInfo);
   COL_VAR(DBDVER_TIME_STAMP);
   COLstring Message;
   COLostream Stream(Message);
   Stream << "Version:         " << DBDVERversionString() << newline;
   Stream << "Source DateTime: " << DBDVERconvertEpoch() << newline;
   Stream << "Source Tag:      " << DBDVER_COMMIT_ID << newline;
   Stream << "Comment:         " << DBDVER_COMMIT_MSG << newline;
   return Message;
}

COLstring DBDVERversionTag(){
   COL_FUNCTION(DBDVERversionTag);
   COLstring Result;
   COLostream Stream(Result);
   Stream << DBD_VERSION_MAJOR << "_" << DBD_VERSION_MINOR << "_" << DBD_BUILD_NUMBER;
   COL_VAR(Result);
   return Result;
}

COLstring DBDVERstartupMessageWithVersionInfo(const COLstring& ConfigurationDirectory){
   COLstring StartMsg;
   COLostream Stream(StartMsg);
   Stream << "Starting the Iguana Service." << newline << DBDVERstartupVersionInfo();
   return StartMsg;
}
