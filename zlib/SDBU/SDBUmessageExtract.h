#ifndef __SDBU_MESSAGE_EXTRACT_H__
#define __SDBU_MESSAGE_EXTRACT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBUmessageExtract
//
// Description:
//
// Routine to extract one message.
//
// Author: Eliot Muir
// Date:   Sunday 14 May 2023 - 05:59PM
// ---------------------------------------------------------------------------
#include <COL/COLauto.h>
#include <COL/COLclosure.h>
#include <COL/COLlist.h>
#include <time.h>

class COLstring;
class SDBcontent;
class SDBmessageId;

// TODO - messy as far as memory management goes.
bool SDBUmessageExtractSingle(const COLstring& LogFile, const SDBmessageId& MessageId,                          COLlist<SDBcontent*>* pList);
bool SDBUmessageExtract      (const COLstring& LogFile, COLauto<COLclosure2<const SDBcontent*, bool*>> pFilter, COLlist<SDBcontent*>* pList);
#endif // end of defensive include
