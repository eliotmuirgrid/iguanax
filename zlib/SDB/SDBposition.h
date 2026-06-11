#ifndef __SDB_POSITION_H__
#define __SDB_POSITION_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBposition
//
// Description:
//
// Functions and data structure for queue position
//
// Author: John Qi
// Date:   Tuesday 14 March 2023 - 03:14PM
// ---------------------------------------------------------------------------

#include <SDB/SDBcontent.h>
#include <SDB/SDBid.h>
#include <COL/COLminimumInclude.h>
#include <COL/COLstring.h>

#include <time.h>

class SDBmessage;
class SDBmessageId;

class SDBposition{
public:
   SDBposition() {};
   SDBposition(const COLstring& ConsumerId, const SDBid& Position);

   COLuint64 Type = SDB_POSITION;   // Must be the first data member to allow casting from or to SDBcontent*
   SDBid Position;
   COLstring ConsumerId;
};

bool operator< (const SDBposition& lhs, const SDBposition& rhs);
bool operator==(const SDBposition& lhs, const SDBposition& rhs);
inline bool operator> (const SDBposition& lhs, const SDBposition& rhs) { return rhs < lhs; }
inline bool operator<=(const SDBposition& lhs, const SDBposition& rhs) { return !(lhs > rhs); }
inline bool operator>=(const SDBposition& lhs, const SDBposition& rhs) { return !(lhs < rhs); }
inline bool operator!=(const SDBposition& lhs, const SDBposition& rhs) { return !(lhs == rhs); }
COLostream& operator<<(COLostream& Stream, const SDBposition& Obj);

int SDBpositionSize (const SDBposition* pPosition);
int SDBwritePosition(const SDBposition* pPosition, char* pBuffer, int BufferSize);
int SDBreadPosition (SDBposition* pPosition, const char* pBuffer, int BufferSize);

// SDBcontent interface
int         SDBpositionSize (const SDBcontent* pPosition);
int         SDBwritePosition(const SDBcontent* pPosition,       char* pBuffer, int BufferSize);
int         SDBreadPosition (      SDBcontent* pPosition, const char* pBuffer, int BufferSize);
COLostream& SDBprintPosition(const SDBcontent* pPosition, COLostream& Stream);
inline SDBcontent* SDBcreatePosition() { return (SDBcontent*) new SDBposition(); }
inline void SDBdeletePosition(SDBcontent* pObject) { delete (SDBposition*)pObject; }

#endif // end of defensive include
