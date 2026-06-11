#ifndef SDBALOGDIRECTORY_H
#define SDBALOGDIRECTORY_H
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBAlogDirectory
//
// Description:
//
// Web API to get log system stats.
//
// Author: Eliot Muir
// Date:   Tuesday 09 May 2023 - 04:26PM
// ---------------------------------------------------------------------------

class COLwebRequest;

void SDBAdirCheck(const COLstring& Dir, COLvar& Result);
void SDBAlogDirectory(const COLwebRequest& Request);

#endif // end of defensive include
