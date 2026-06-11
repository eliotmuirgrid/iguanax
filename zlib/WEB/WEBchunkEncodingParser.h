#ifndef __WEB_CHUNK_ENCODING_PARSER_H__
#define __WEB_CHUNK_ENCODING_PARSER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: WEBchunkEncodingParser
//
// Author: Eliot Muir
//
// Description:
//
// Chunk encoding parser
//---------------------------------------------------------------------------

#include <COL/COLstring.h>
#include <COL/COLauto.h>
#include <COL/COLclosure.h>

class WEBchunkEncodingParser {
public:
   WEBchunkEncodingParser();
   ~WEBchunkEncodingParser();
   bool parseData(const char* pExtraData, int Size, COLstring* pChunk);
   void reset();
   void setChunkCallback(COLauto< COLclosure2<COLstring*, bool> > pChunkCallback);
private:
   COLstring m_Data;
   COLauto<COLclosure2<COLstring*, bool> > m_pChunkCallback;
}; 

int WEBfindNextChunk(COLstring& Data, COLstring* pResult);

#endif // end of defensive include
