#ifndef __SDB_LOG_H__
#define __SDB_LOG_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBlog
//
// Description:
//
// Struct for a message
//
// Author: Eliot Muir
// Date:   Thursday 16 February 2023 - 03:44PM
// ---------------------------------------------------------------------------

#include <COL/COLstring.h>
#include <COL/COLlist.h>
#include <COL/COLvar.h>
#include <SDB/SDBcontent.h>
#include <SDB/SDBepochTime.h>
#include <SDB/SDBid.h>

#include <time.h>

class SDBmessage{
public:
   SDBmessage(); // Plain constructor - does nothing
   SDBmessage(const COLstring& ComponentId, const COLstring& iData, COLuint64 DateTime = SDBtime(), COLuint64 Index = 1);

   COLuint64 Type = SDB_MESSAGE;    // Must be the first data member to allow casting from or to SDBcontent*
   SDBid Id;
   SDBmessageId RelatedId;
   COLstring Tags;
   COLstring Data;
   COLvar Meta;
};

bool operator==(const SDBmessage& lhs, const SDBmessage& rhs);
inline bool operator!=(const SDBmessage& lhs, const SDBmessage& rhs) { return !(lhs == rhs); }
COLostream& operator<<(COLostream& Stream, const SDBmessage& Obj);

int SDBmessageSize(const SDBmessage* pMessage);
int SDBwriteMessage(const SDBmessage* pMessage, char* pBuffer, int BufferSize);
int SDBreadMessage(SDBmessage* pMessage, const char* pBuffer, int BufferSize);

bool SDBisMessage(COLuint64 Type);
bool SDBisMessage(const SDBcontent* pContent);

// returns true if Tags contains Tag
bool SDBhasTag(COLstring Tags, const COLstring& Tag);

// SDBcontent interface
int         SDBmessageSize (const SDBcontent* pMessage);
int         SDBwriteMessage(const SDBcontent* pMessage,       char* pBuffer, int BufferSize);
int         SDBreadMessage (      SDBcontent* pMessage, const char* pBuffer, int BufferSize);
COLostream& SDBprintMessage(const SDBcontent* pMessage, COLostream& Stream);
inline SDBcontent* SDBmessageCreate() { return (SDBcontent*)new SDBmessage(); }
inline void SDBmessageDelete(SDBcontent* pObject) { delete (SDBmessage*)pObject; }

#endif // end of defensive include
