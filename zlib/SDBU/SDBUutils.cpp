//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SDBUutils.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-09-03, 11:48 a.m.
//  ---------------------------------------------------------------------------
#include <SDBU/SDBUmessageExtract.h>
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <HASH/HASHutils.h>
#include <SDB/SDBcontent.h>
#include <SDB/SDBcontentDelete.h>
#include <SDB/SDBdir.h>
#include <SDB/SDBmessage.h>
#include <SDB/SDBreader.h>
#include <SDB/SDBserviceLog.h>
#include <SDBU/SDBUutils.h>
COL_LOG_MODULE;

bool SDBUcomponentId(const COLstring& Input, COLstring* pResolvedId, COLstring* pErr){
   COL_FUNCTION(SDBUcomponentId);
   if (Input.is_null() || Input.isWhitespace()){
      COL_TRC("Component is empty str so defaulting to " << SDB_SERVICE_COMPONENT_ID);
      *pResolvedId = SDB_SERVICE_COMPONENT_ID;
      return true;
   }
   COL_VAR(Input);
   CFGmap Map = CFGmapGet();
   CFGconfig* pTransNode = CFGmapNode(&Map, Input);
   if (!pTransNode){
      *pResolvedId = Input;
      *pErr = "Component '" + Input + "' does not exist.";
      return false;
   } 
   *pResolvedId = pTransNode->CoreConfig.Guid;
   return true;
}

bool SDBUcomponentExists(const CFGmap* pMap, const COLstring& Component) {
   COL_FUNCTION(SDBUcomponentExists);
   return pMap->ConfigMap.count(Component) == 1;
}

void SDBUcontentToVar(const SDBmessage& Message, COLvar* pOut){
   COL_FUNCTION(SDBUcontentToVar);
   COLvar& Result = *pOut;
   Result["type"]      = SDBtypeAsString ((SDBtype) Message.Type);
   Result["time"]      = COLint64ToString(Message.Id.MessageId.DateTime);
   Result["id"]        = COLint64ToString(Message.Id.MessageId.Index);
   Result["size"]      = COLintToString  (Message.Data.size());  // For consistency I encode size as a string.
   Result["comp"]      = Message.Id.MessageId.ComponentId;
   Result["rel_comp"]  = Message.RelatedId.ComponentId;
   Result["rel_time"]  = COLint64ToString(Message.RelatedId.DateTime);
   Result["rel_id"]    = COLint64ToString(Message.RelatedId.Index);
   Result["sid"]       = COLint64ToString(Message.Id.SequenceId);
   Result["qid"]       = COLint64ToString(Message.Id.QueueId);
   HASHstringToVar(Message.Tags, &Result["tags"]);
}

static SDBmessage* SDBUextractMessageFromLogs(const COLstring& Component, int Index, COLuint64 Time, COLstring* pErr, const COLstring& LogDir) {
   COL_FUNCTION(SDBUextractMessageFromLogs);
   COLstring LogFile = SDBfindLogFile(Component, Time, LogDir);
   COL_VAR(LogFile);
   if (LogFile.is_null()){
      *pErr = "Log File " + LogFile + " not found";
      return NULL;
   }
   COLlist<SDBcontent*> List;
   SDBmessageId Id(Component, Time, Index);
   if (!SDBUmessageExtractSingle(LogFile, Id, &List)){
      SDBcontentDelete(&List); // TODO - fragile design with memory management.
      *pErr = "Couldn't find this message.";
      return NULL;
   }
   return (SDBmessage*) *List.begin();
}

SDBmessage* SDBUextractMessage(const COLstring& Component, int Index, COLuint64 Time, COLstring* pErr, const COLstring& LogDir, const COLstring& LogDir2) {
   COL_FUNCTION(SDBUextractMessage);
   // TODO: fetch from both log dirs in parallel if needed
   SDBmessage* pMessage = SDBUextractMessageFromLogs(Component, Index, Time, pErr, LogDir);
   if (SDBroot2().is_null()) { return pMessage; }
   if (pMessage) { return pMessage; }
   return SDBUextractMessageFromLogs(Component, Index, Time, pErr, LogDir2);
}

bool SDBUextractMessage(const COLstring& Component, int Index, COLuint64 Time, COLstring* pData, COLstring* pErr) {
   SDBmessage* pMessage = SDBUextractMessage(Component, Index, Time, pErr);
   if (pMessage) { pMessage->Data.swap(pData); }
   delete pMessage;
   return pMessage != NULL;
}

bool SDBUbadBounds(const SDBmessageId& Start, const SDBmessageId& End, const bool Forward) {
   COL_FUNCTION(SDBUbadBounds);
   return Forward ? Start > End : End > Start;
}
