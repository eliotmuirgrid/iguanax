#ifndef _SDBULOGFILE_H
#define _SDBULOGFILE_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SDBUlogFile.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-09-03, 11:54a.m.
//  ---------------------------------------------------------------------------
#include <COL/COLlist.h>
#include <COL/COLstring.h>
#include <SDB/SDBfile.h>
#include <SDB/SDBfilter2.h>
class SDBcontent;
class SDBfile;
class SDBmessageId;

SDBfile SDBUfindOldestLogFile(const COLstring& ComponentId);
SDBfile SDBUfindStartingLogFile(const SDBmessageId& StartMessage);

void SDBUmergeFileToList(COLlist<SDBcontent*>& List1, COLlist<SDBcontent*>& List2, COLlist<SDBcontent*>* pList);
SDBfile SDBUfindNextFileToRead(const SDBfile& LogFile, const COLstring& ComponentId, bool DirectionNext);
COLuint64 SDBUlogFileTime(const SDBfile& LogFile, const COLstring& ComponentId, bool DirectionNext);

bool SDBUloadFileToList(const SDBfile& LogFile, const SDBfilterInfo2& Filter, COLlist<SDBcontent*>* pList);


#endif  // end of defensive include
