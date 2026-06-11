#ifndef __SDB_MAGIC_MARKER_H__
#define __SDB_MAGIC_MARKER_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBmagicMarker
//
// Description:
//
// A potential content type - quite simple - really just exists for the purposes
// of proving out the content model.
//
// Author: Eliot Muir
// Date:   Friday 24 February 2023 - 10:31AM
// ---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <SDB/SDBcontent.h>

class COLstring;

class SDBmagicMarker{
public:
   COLuint64 Type = SDB_MARKER;
};

bool operator==(const SDBmagicMarker& lhs, const SDBmagicMarker& rhs);
inline bool operator!=(const SDBmagicMarker& lhs, const SDBmagicMarker& rhs) { return !(lhs == rhs); }
COLostream& operator<<(COLostream& Stream, const SDBmagicMarker& Obj);

int SDBmagicMarkerSize (const SDBmagicMarker* pMarker);
int SDBwriteMagicMarker(const SDBmagicMarker* pMarker, char* pBuffer, int BufferSize);
int SDBreadMagicMarker (SDBmagicMarker* pMarker, const char* pBuffer, int BufferSize);

// SDBcontent interface
int         SDBmagicMarkerSize (const SDBcontent* pMarker);
int         SDBwriteMagicMarker(const SDBcontent* pMarker,       char* pBuffer, int BufferSize);
int         SDBreadMagicMarker (      SDBcontent* pMarker, const char* pBuffer, int BufferSize);
COLostream& SDBprintMagicMarker(const SDBcontent* pMarker, COLostream& Stream);
inline SDBcontent* SDBmagicMarkerCreate() { return (SDBcontent*)new SDBmagicMarker(); }
inline void SDBmagicMarkerDelete(SDBcontent* pObject) { delete (SDBmagicMarker*)pObject; }

// Finds the offset to next magic marker in pBuffer.
int SDBreadNextMagicMarkerOffset(const char* pBuffer, int BufferSize);

#endif // end of defensive include
