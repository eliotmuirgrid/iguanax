#ifndef __SDM_LOG_EVENT_H__
#define __SDM_LOG_EVENT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDMlogEvent
//
// Description:
//
// Log message to system log for POSIX or Windows Event Log for Windoze
//
// Author: Eliot Muir
// Date:   Friday 04 August 2023 - 03:33PM
// ---------------------------------------------------------------------------

class COLstring;

void SDMlogEvent(const COLstring& ServiceName, const COLstring& Log);
void SDMlogErrorEvent(const COLstring& ServiceName, const COLstring& Log);

#endif // end of defensive include
