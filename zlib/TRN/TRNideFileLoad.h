#ifndef __TRN_IDE_FILE_LOAD_H__
#define __TRN_IDE_FILE_LOAD_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNideFileLoad
//
// Description:
//
// Load a file in the translator
//
// Author: Eliot Muir
// Date:   Tuesday 25 April 2023 - 09:30AM
// ---------------------------------------------------------------------------

class COLstring;
class COLwebRequest;

void TRNideFileLoad(const COLwebRequest& Request);

// unit test
bool TRNisBinaryContent(const COLstring& Content);

   #endif // end of defensive include
